# Переход к stateful и stateless акторам

- Статус: Черновик
- Область: `tegia-node` runtime
- Связанный ADR: `docs/adr/ADR-0001-actor-execution-model.md`

## 1. Цель

Разделить акторы на два явно различающихся класса выполнения:

1. **Stateful actor** — актор с внутренним изменяемым состоянием. Runtime гарантирует последовательную обработку сообщений для одного actor instance.
2. **Stateless actor** — актор без изменяемого состояния actor instance. Runtime допускает параллельную обработку нескольких сообщений одним actor instance.

Цель перехода — убрать необходимость в `mutex` для защиты обычного состояния stateful-акторов, но сохранить текущий уровень параллелизма для обработчиков, которые действительно не зависят от состояния экземпляра.

## 2. Текущая модель

Сейчас `actors::map_t::send_message(...)` после поиска или создания actor instance сразу ставит каждое сообщение как независимую задачу в общий `threads::pool_t`.

Следствия:

- один и тот же actor instance может одновременно выполнять несколько `action`;
- все изменяемое состояние actor instance должно защищаться вручную;
- авторы акторов вынуждены использовать `mutex` даже там, где последовательная mailbox-семантика была бы естественной;
- ошибки синхронизации не видны в контракте типа актора.

## 3. Целевая модель

### 3.1 Stateful actor

Stateful actor владеет состоянием actor instance. Для него runtime гарантирует:

- для одного actor instance одновременно выполняется не больше одного `action`;
- сообщения одного actor instance обрабатываются в порядке постановки в mailbox;
- разные actor instances продолжают выполняться параллельно в общем worker pool;
- callback chain текущего сообщения считается частью обработки текущего сообщения.

Важное ограничение: stateful actor не получает постоянный выделенный OS thread. Сообщения одного actor instance могут выполняться разными worker threads, но не одновременно.

### 3.2 Stateless actor

Stateless actor не должен хранить изменяемое состояние actor instance между сообщениями.

Для него runtime сохраняет текущую модель:

- несколько сообщений одного actor instance могут выполняться параллельно;
- ответственность за thread-safety внешних ресурсов остается на авторе актора;
- локальные переменные внутри `action` безопасны;
- shared-ресурсы, кеши, соединения и singleton-объекты должны иметь собственный потокобезопасный контракт.

Stateless actor подходит для адаптеров, маршрутизаторов, чистых вычислений, легких proxy-обработчиков и операций, где состояние полностью приходит в `message`.

## 4. Публичный API для actor types

Рекомендуемая форма API — два базовых класса:

```cpp
class my_stateful_actor_t : public tegia::actors::stateful_actor_t
{
public:
    explicit my_stateful_actor_t(const std::string &name)
        : stateful_actor_t("MY::STATEFUL", name) {}
};
```

```cpp
class my_stateless_actor_t : public tegia::actors::stateless_actor_t
{
public:
    explicit my_stateless_actor_t(const std::string &name)
        : stateless_actor_t("MY::STATELESS", name) {}
};
```

Оба класса наследуются от существующего `actor_t`, поэтому сигнатура `action` и регистрация через `type_t<Actor>` остаются прежними.

Для совместимости на первом этапе `actor_t` должен сохранить старое поведение. Новые акторы должны явно выбирать `stateful_actor_t` или `stateless_actor_t`.

## 5. Runtime-модель dispatch

### 5.1 Общая структура

`threads::pool_t` остается общим executor. Для каждого actor instance вводится единая runtime-абстракция `mailbox`.

`mailbox` отвечает за прием сообщения actor instance и решение, когда это сообщение можно отдать в общий `pool_t`.

Единый принцип для всех actor instance:

- сообщение сначала попадает во внутреннюю FIFO-очередь mailbox;
- mailbox передает item из очереди в `pool_t` только если число уже переданных и еще не завершенных item меньше `max_inflight`;
- после завершения item mailbox освобождает один слот и пытается передать следующий item из очереди.

Разница между stateful и stateless actor задается только значением `max_inflight`:

1. **Stateful mailbox** — `max_inflight = 1`. Одновременно в `pool_t` может находиться только один item этого actor instance.
2. **Stateless mailbox** — `max_inflight = thread_count(pool_t)`. Одновременно в `pool_t` может находиться не больше item этого actor instance, чем запущено worker threads в общем пуле.

Общие поля `mailbox`:

- ссылка или указатель на actor instance;
- режим выполнения: `parallel` или `serial`;
- `max_inflight` — лимит item этого actor instance, одновременно переданных в `pool_t`;
- `running` — число item, уже переданных в `pool_t` и еще не завершенных;
- FIFO-очередь сообщений, ожидающих передачи в `pool_t`;
- счетчик active-сообщений, который учитывает queued + running;
- общий метод постановки сообщения, например `enqueue(...)`.

`max_inflight` должен быть больше нуля. Значение `0` запрещено: runtime wiring/config validation должен отклонять такую настройку, а mailbox защитно нормализует `0` к `1`.

Элемент mailbox должен хранить:

- `actor_t *`;
- `action_t *`;
- `std::shared_ptr<message_t>`;
- `std::shared_ptr<tegia::user>`;
- `priority`;
- время постановки в очередь для будущей observability.

`actors::map_t::send_message(...)` не должен напрямую решать, можно ли сразу ставить задачу в `pool_t`. Его задача — найти или создать actor instance, найти `action_t`, выполнить базовую валидацию и передать элемент в mailbox actor instance.

Mailbox не поддерживает двухфазную настройку через `configure(...)`. Он должен создаваться сразу с mode, `pool_t`, dispatch-функцией, `max_inflight` и лимитом очереди. Это исключает перенастройку mailbox во время обработки сообщений.

### 5.2 Mailbox для stateless actor

Stateless actor допускает параллельное выполнение, но mailbox ограничивает число item, переданных в `pool_t`.

1. Найти или создать actor instance.
2. Найти `action_t`.
3. Проверить JSON schema, если она задана.
4. Создать элемент mailbox.
5. Вызвать `mailbox.enqueue(...)`.
6. `mailbox` добавляет item во внутреннюю FIFO-очередь.
7. Если `running < max_inflight`, mailbox извлекает item из очереди, увеличивает `running` и ставит `action_func(...)` в `pool_t`.
8. Если `running == max_inflight`, item остается в очереди до завершения одной из уже переданных задач.
9. После завершения `action_func(...)` mailbox уменьшает `running` и пытается передать следующий item из очереди.

Для stateless actor значение `max_inflight` должно равняться числу реально запущенных worker threads в `pool_t`. Это ограничивает вклад одного stateless actor instance в общую очередь задач.

### 5.3 Mailbox для stateful actor

Stateful actor использует тот же mailbox, но с `max_inflight = 1`.

1. Найти или создать actor instance.
2. Найти `action_t`.
3. Проверить JSON schema, если она задана.
4. Создать элемент mailbox.
5. Вызвать `mailbox.enqueue(...)`.
6. `mailbox` добавляет item во внутреннюю FIFO-очередь.
7. Если `running == 0`, mailbox извлекает item из очереди, выставляет `running = 1` и ставит `action_func(...)` в `pool_t`.
8. Если `running == 1`, item остается в очереди.

После завершения `action_func(...)`:

1. Выполнить callback chain текущего сообщения.
2. Уменьшить `running`.
3. Если очередь mailbox не пуста:
   - извлечь следующий элемент;
   - снова выставить `running = 1`;
   - поставить следующий `action_func(...)` в `pool_t`.
4. Если очередь mailbox пуста, actor instance считается свободным.

Этот порядок важен: следующий message того же actor instance не должен стартовать до завершения callback chain текущего сообщения.

## 6. Callback chain

Текущий `message_t::callback` позволяет action добавить следующий адрес.

Для stateful actor callback считается частью обработки текущего сообщения, потому что он может менять `message` и продолжать бизнес-сценарий. Поэтому уменьшение `running` должно происходить после чтения и отправки callback.

Если callback адресован тому же actor instance, новое сообщение попадет в mailbox и будет выполнено после текущего сообщения.

## 7. Приоритеты

На первом этапе рекомендуется сохранить простую модель:

- внутри mailbox actor instance используется FIFO;
- `priority` применяется при постановке извлеченного item в общий `pool_t`;
- `priority` не меняет порядок извлечения item из mailbox.

`priority` жестко нормализуется на входе в mailbox:

- если `priority < 0`, используется `0`;
- если `priority > 63`, используется `63`.

Причина: priority внутри mailbox может нарушить причинный порядок сообщений одного stateful actor instance. Если приоритеты внутри mailbox понадобятся позже, это должно быть отдельным ADR, потому что меняется семантика actor state.

## 8. Backpressure

Минимальная версия должна поддерживать лимит очереди mailbox на actor instance.

Рекомендуемые параметры:

- глобальный default queue limit в runtime config;
- возможность переопределить лимит на actor type;
- контролируемый отказ при переполнении mailbox.

Значение `0` для queue limit запрещено. Runtime-конфигурация с нулевым лимитом должна отклоняться на этапе wiring/config validation. Внутри mailbox нулевое значение должно защитно нормализоваться к минимальному рабочему значению `1`, чтобы actor instance не становился навсегда непринимающим сообщения.

Для stateless actor лимит очереди все равно нужен: если actor instance уже передал в `pool_t` `thread_count(pool_t)` item, новые сообщения должны ожидать в mailbox, а не бесконечно раздувать общий pool.

При переполнении:

- сообщение не добавляется в mailbox;
- счетчик сообщений не увеличивается или откатывается;
- для HTTP-сценариев выставляется `429`;
- в лог пишется событие с actor name, action, mailbox depth и limit.

## 9. Ошибки и гарантированное освобождение mailbox

Платформа не должна использовать исключения как механизм управления ошибками. Dispatch-функция mailbox должна возвращать код выполнения, а не бросать exception.

На первом этапе mailbox может игнорировать код возврата dispatch-функции. Это временное ограничение класса `actor_mailbox_t`: обработка ненулевых кодов dispatch остается отдельной доработкой runtime.

Завершение `action_func(...)` должно гарантировать:

- уменьшение счетчика сообщений;
- запуск следующего элемента mailbox;
- уменьшение `running`;
- сохранение текущего поведения callback chain при успешном выполнении;
- отсутствие `throw` из mailbox и worker path.

Если action завершилась ошибкой, ошибка должна передаваться кодом возврата, статусом сообщения и лог-событием. Даже при ошибке mailbox обязан освободить один слот `max_inflight` и продолжить продвижение очереди.

Если item уже извлечен из FIFO-очереди mailbox, но не был поставлен в `pool_t`, mailbox не возвращает его обратно в очередь. В этом случае mailbox откатывает учет `active/running`, возвращает код ошибки наверх, а решение о retry/drop/логировании принимает вызывающий слой runtime.

Для корректной интеграции `pool_t::add_task(...)` должен иметь надежный error contract: если задача не поставлена в очередь, метод обязан вернуть ненулевой код и не оставлять утечек ресурсов. Сейчас это отдельная обязательная доработка платформы.

## 10. Unload actor

`actors::map_t::unload(...)` сейчас ориентируется на счетчик `actor_t::messages`.

После добавления mailbox правило должно быть таким:

- actor instance можно выгружать только если нет выполняемого сообщения и mailbox пуст;
- `actor_t::messages` должен быть согласован с mailbox и учитывать queued + running;
- удаление должно освобождать и actor instance, и mailbox этого actor instance;
- проверка unload должна выполняться под тем же mutex или через согласованное состояние actor instance.

Текущий класс mailbox только предоставляет `idle()`, `active()`, `queued()` и `running()`. Интеграция с `actor_t::messages` и изменение логики удаления actor instance остаются отдельной доработкой runtime.

Логику удаления акторов нужно переделать полностью: runtime должен удалять единую actor-entry структуру, которая владеет и `actor_t`, и mailbox. Нельзя удалять `actor_t` отдельно от mailbox, потому что задачи в `pool_t` могут еще ссылаться на mailbox и actor через mailbox item.

Mailbox-задачи, переданные в `pool_t`, не должны переживать mailbox. На этапе интеграции нужно ввести один из двух контрактов:

1. actor-entry удаляется только после `mailbox.idle() == true` под общим lock actor map;
2. mailbox управляется через shared ownership, а worker-задачи держат безопасную ссылку на runtime state.

## 11. Миграционный план

### Этап 1. Runtime primitives

1. Добавить enum режима выполнения actor instance.
2. Добавить `stateful_actor_t` и `stateless_actor_t`.
3. Оставить `actor_t` в legacy-режиме для обратной совместимости.
4. Добавить mailbox как обязательный runtime-компонент actor instance.
5. Добавить в mailbox лимит `max_inflight`.
6. Для stateful actor задавать `max_inflight = 1`.
7. Для stateless actor задавать `max_inflight = thread_count(pool_t)`.

### Этап 2. Scheduler

1. Вынести общий код поиска `action`, schema validation и создания mailbox-элемента в небольшие внутренние функции.
2. Перевести `map_t` на передачу mailbox-элемента в `mailbox.enqueue(...)`.
3. Реализовать универсальный dispatch из mailbox в `pool_t` до лимита `max_inflight`.
4. Добавить завершение action с освобождением одного running-слота и запуском следующего mailbox-элемента.
5. Покрыть завершение action без исключений: dispatch-функция возвращает код, mailbox всегда освобождает running-слот.
6. Добавить способ получить число запущенных worker threads из `pool_t` или передавать это значение при создании mailbox.
7. Согласовать `actor_t::messages` с mailbox active/running/queued.
8. Доработать `pool_t::add_task(...)`: метод должен возвращать реальную ошибку постановки задачи в очередь.
9. Переделать логику удаления акторов на удаление actor-entry (`actor_t + mailbox`) с проверкой `mailbox.idle()`.
10. Зафиксировать lifetime-контракт mailbox-задач, чтобы исключить use-after-free при raw-ссылках из worker tasks.

### Этап 3. Backpressure v1

1. Добавить лимит очереди mailbox.
2. Добавить отказ при переполнении.
3. Добавить лог-события переполнения.

### Этап 4. Миграция системных акторов

1. `ws_t` перевести в stateful, потому что он владеет `_system`, `data` и membership-состоянием.
2. `worker_t` перевести в stateful или удалить внутренний `run_mutex` после подтверждения новой семантики.
3. Stateless оставить только для акторов, которые явно не имеют изменяемого состояния instance.

### Этап 5. SDK-документация

После реализации runtime обновить `/include/tegia/docs`:

- описать разницу `stateful_actor_t` и `stateless_actor_t`;
- добавить правила выбора класса;
- добавить предупреждение, что stateless actor не должен хранить изменяемое состояние instance;
- добавить пример миграции с `actor_t` на `stateful_actor_t`.

## 12. Тестирование

Минимальный набор проверок:

1. Для одного stateful actor instance два долгих action не выполняются одновременно.
2. Для двух разных stateful actor instances action выполняются параллельно.
3. Stateless actor через mailbox выполняет параллельно не больше `thread_count(pool_t)` action одного actor instance.
4. Callback на тот же stateful actor instance не стартует параллельно с текущим action.
5. `unload(...)` не удаляет actor instance с непустым mailbox или ненулевым `running`.
6. Переполнение mailbox возвращает контролируемую ошибку.
7. Исключение внутри action не блокирует mailbox навсегда.

## 13. Нерешенные вопросы

1. Какой режим должен быть default после завершения миграции: безопасный `stateful` или совместимый `stateless`?
2. Нужен ли режим на уровне action, или достаточно режима на уровне actor type?
3. Нужно ли поддерживать priority внутри mailbox одного actor instance?
4. Какой default queue limit mailbox принять для production?
5. Должен ли `actor_t` остаться допустимым базовым классом после миграционного периода?
6. Где хранить `thread_count(pool_t)`: как публичный метод pool, как параметр `node::run()` или как часть runtime config?
