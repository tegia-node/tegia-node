# Task-007: Приоритетные очереди внутри `actor_mailbox_t`

- Статус: Запланировано
- Дата: 2026-07-05
- Основание: open issue из `docs/adr/ADR-0001-actor-execution-model.md` и раздел `7. Приоритеты` в `docs/actor-execution-transition.md`
- Цель: прозрачно для акторов перевести `actor_mailbox_t` с FIFO-очереди на priority FIFO: ожидающие сообщения одного mailbox выбираются по `priority`, а внутри одного priority сохраняется порядок постановки.
- Готовность: готова к реализации.

## 1. Контекст

Сейчас `actor_mailbox_t` принимает `priority` в `actor_mailbox_item_t`, нормализует его в диапазон `[0, 63]` и передает дальше в общий `threads::pool_t`.

Внутри самого mailbox ожидающие сообщения хранятся в FIFO-очереди:

1. `enqueue(...)` кладет ожидающие сообщения в `std::deque`.
2. `dispatch_available()` всегда извлекает `front()`.
3. `priority` применяется только после извлечения item из mailbox, при постановке задачи в `pool_t`.

Целевое поведение меняет это правило: `priority` должен применяться уже внутри mailbox. Акторы и actor types не должны включать новую семантику вручную. Все actor instances, у которых есть mailbox, получают priority ordering прозрачно.

## 2. Нормативный контракт

1. `priority = 0` считается самым высоким приоритетом.
2. `priority = 63` считается самым низким приоритетом.
3. Значения вне диапазона нормализуются как сейчас:
   - `priority < 0` становится `0`;
   - `priority > 63` становится `63`.
4. Mailbox использует strict priority:
   - сначала выбирается минимальный номер priority;
   - внутри одного priority сохраняется FIFO.
5. Приоритет влияет только на сообщения, которые еще находятся во внутренней очереди mailbox.
6. Сообщение, уже переданное из mailbox в `pool_t`, не переупорядочивается и не вытесняется.
7. Для `stateful` actor сохраняется single-flight: одновременно выполняется не больше одного action одного actor instance.
8. FIFO по порядку поступления между разными priority больше не является контрактом mailbox.
9. Публичный API actor type не меняется: авторы акторов не вызывают opt-in методы и не меняют конфигурации.

## 3. Priority FIFO

`actor_mailbox_t` должен перейти с единой FIFO-очереди на набор очередей по priority.

Поведение:

1. `enqueue(...)` нормализует `priority` и помещает item в очередь этого priority.
2. `dispatch_available()` выбирает item из самой приоритетной непустой очереди.
3. Внутри очереди одного priority item извлекаются в порядке постановки.
4. При dispatch в `pool_t` item передается с тем же priority.

Ограничение v1:

1. Priority FIFO не содержит aging или weighted fairness.
2. При постоянном потоке high-priority сообщений low-priority сообщения могут ожидать неограниченно долго.
3. Это принимается как известное ограничение v1 и должно быть отражено в документации.
4. Follow-up по fairness/aging вынесен в roadmap: [Task 007 - Case 001](../technical/roadmap/cases/task-007-case-001-priority-fairness-aging.md).

## 4. Stateful actor и причинный порядок

Priority ordering может изменить порядок обработки сообщений одного `stateful` actor instance, если сообщения имеют разные priority.

Решение для этой задачи:

1. Runtime переходит на priority ordering для всех mailbox-backed actor instances.
2. Для `stateful` actor сохраняется гарантия отсутствия параллельного выполнения (`max_inflight = 1`).
3. Runtime больше не гарантирует FIFO между сообщениями разных priority внутри одного actor instance.
4. Это изменение должно быть явно описано в `/docs` и `/include/tegia/docs`.

Следствие:

1. Если бизнес-сценарий stateful actor требует причинный порядок, отправитель должен использовать одинаковый priority для causally связанных сообщений.
2. Если сообщения имеют разные priority, runtime имеет право обработать более высокий priority раньше более низкого.

## 5. Callback priority propagation

Текущая callback chain в `map_t::action_func(...)` отправляет следующий message без явного priority, поэтому callback получает default `0`. После перехода mailbox на priority ordering это превращает любой callback в high-priority сообщение.

В рамках задачи нужно реализовать правило:

1. Callback наследует priority текущего mailbox item.
2. Callback является обычным новым сообщением с inherited priority.
3. Если callback адресован тому же actor instance, он попадает в mailbox и конкурирует с уже ожидающими сообщениями по общему priority FIFO правилу.
4. Для передачи priority `map_t::action_func(...)` должен получить `current_priority` или callback dispatch должен быть перенесен в слой, где доступен `actor_mailbox_item_t`.
5. При отправке callback использовать:

```cpp
tegia::message::send(callback.actor, callback.action, message, current_priority);
```

Не входит в v1:

1. Расширение `message_t::callback` отдельным priority.
2. Отдельная политика priority для callback chain.

Follow-up по расширенной callback policy вынесен в roadmap: [Task 007 - Case 003](../technical/roadmap/cases/task-007-case-003-callback-priority-policy.md).

## 6. Объем работ

Входит:

1. Заменить внутреннюю FIFO-очередь `actor_mailbox_t` на priority queues.
2. Сохранить текущую нормализацию priority в диапазон `[0, 63]`.
3. Сохранить `max_inflight`, `active`, `queued`, `running`, `idle` semantics.
4. Обновить `enqueue(...)`, `dispatch_available()`, `queued()` и связанные private helper-методы.
5. Исправить callback priority propagation.
6. Обновить внутреннюю документацию runtime в `/docs`.
7. Обновить SDK-документацию для авторов конфигураций в `/include/tegia/docs`.

Не входит:

1. Изменение `threads::queue`.
2. Изменение публичного API `type_base_t`.
3. Opt-in/opt-out настройка actor type.
4. Weighted scheduling или aging priority. Follow-up: [Task 007 - Case 001](../technical/roadmap/cases/task-007-case-001-priority-fairness-aging.md).
5. Preemption или reprioritization задач, уже переданных в `pool_t`. Follow-up: [Task 007 - Case 002](../technical/roadmap/cases/task-007-case-002-pool-preemption-reprioritization.md).
6. Изменение remote delivery semantics.
7. Изменение формата `message_t::callback`. Follow-up: [Task 007 - Case 003](../technical/roadmap/cases/task-007-case-003-callback-priority-policy.md).
8. Fair scheduling между actor instances. Follow-up: [Task 007 - Case 004](../technical/roadmap/cases/task-007-case-004-actor-instance-fair-scheduling.md).

## 7. Детали реализации

### 7.1 `src/actors/actor_mailbox.h`

Заменить:

```cpp
std::deque<actor_mailbox_item_t> _queue;
```

на:

```cpp
std::array<std::deque<actor_mailbox_item_t>, 64> _priority_queues;
std::bitset<64> _priority_mask;
std::size_t _queued_messages = 0;
```

Добавить private helper-методы, вызываемые только под уже захваченным `_mutex`:

```cpp
bool queue_empty_locked() const;
std::size_t queue_size_locked() const;
void queue_push_locked(actor_mailbox_item_t item);
actor_mailbox_item_t queue_pop_locked();
```

Требования:

1. `_queued_messages` является единственным источником размера ожидающей очереди.
2. `queued()` возвращает `_queued_messages`.
3. `idle()` учитывает `_queued_messages == 0`, `_running_messages == 0` и `_active_messages == 0`.
4. `queue_pop_locked()` выбирает минимальный установленный priority.
5. После извлечения последнего item из priority-очереди соответствующий бит в `_priority_mask` очищается.
6. Постоянный memory overhead от 64 `std::deque` на mailbox принимается как цена v1 ради простоты и предсказуемости реализации.

### 7.2 `src/actors/actor_mailbox.cpp`

Изменить:

1. `enqueue(...)`:
   - сохранить нормализацию priority до постановки в очередь;
   - заменить прямые обращения к `_queue` на helper-методы;
   - overflow проверять через `queue_size_locked() >= _max_queue_size`;
   - fast path разрешен только если `queue_empty_locked() == true` и есть свободный running-slot.

2. `dispatch_available()`:
   - заменить `front()/pop_front()` на `queue_pop_locked()`;
   - сохранить текущую гарантию `running < max_inflight`;
   - при ошибке `dispatch(...)` выполнять существующий rollback.

3. `queued()`:
   - возвращать `_queued_messages` под mutex.

4. `rollback_dispatch()`:
   - не возвращать item в очередь;
   - откатывать `active/running` как сейчас;
   - сохранять инвариант `_queued_messages`: item, уже извлеченный из mailbox, не должен учитываться как queued.

### 7.3 `src/actors/map.cpp`

Для callback:

1. Расширить `map_t::action_func(...)` параметром `current_priority`, либо перенести callback dispatch в уровень, где доступен `actor_mailbox_item_t`.
2. При callback-send передавать `current_priority`.
3. Fallback path без mailbox должен сохранить текущую семантику передачи priority напрямую в `pool_t`; внутренняя priority-очередь относится только к mailbox-backed actor entries.

### 7.4 Файлы, которые не должны меняться

Не требуется менять:

1. `include/tegia/actors/type.h`.
2. Публичный API actor type.
3. Конфигурации actor types.
4. `actor_entry_t`, если его конструктор не нуждается в изменениях для новой структуры mailbox.

## 8. Документация

Обновить `/docs`:

1. `docs/actor-execution-transition.md`:
   - заменить описание FIFO внутри mailbox на priority FIFO;
   - описать, что FIFO сохраняется только внутри одного priority;
   - зафиксировать strict priority limitation;
   - зафиксировать, что `stateful` actor сохраняет single-flight, но не FIFO между разными priority.

2. При необходимости добавить update к `docs/adr/ADR-0001-actor-execution-model.md`, потому что меняется семантика порядка сообщений actor instance.

Обновить `/include/tegia/docs`:

1. Документацию для авторов actor types.
2. Предупреждение, что порядок сообщений одного actor instance теперь зависит от priority.
3. Рекомендацию использовать одинаковый priority для сообщений, которые должны сохранять причинный порядок.

## 9. Тест-план

Минимальные проверки:

1. `priority_fifo_orders_by_priority`:
   - предварительное условие: проверяемые сообщения должны находиться во внутренней очереди mailbox, а не уже быть переданными в `pool_t`;
   - ожидающие сообщения с priority `0` извлекаются раньше priority `10`, а `10` раньше `63`.

2. `priority_fifo_preserves_fifo_inside_same_priority`:
   - несколько сообщений с одинаковым priority выполняются в порядке постановки.

3. `priority_normalization`:
   - `priority < 0` становится `0`;
   - `priority > 63` становится `63`.

4. `priority_overflow_uses_total_queued_size`:
   - overflow считается по суммарному числу ожидающих сообщений во всех priority queues.

5. `mailbox_counters_are_consistent`:
   - `active()`, `queued()`, `running()`, `idle()` корректны для priority mailbox.

6. `serial_mailbox_keeps_single_running_item`:
   - для serial mailbox `running()` никогда не превышает `1`.

7. `callback_inherits_priority`:
   - callback, созданный high/low-priority сообщением, отправляется с тем же priority.

8. `callback_same_actor_is_regular_priority_message`:
   - callback на тот же actor instance попадает в mailbox как обычное сообщение с inherited priority и выбирается по общему priority FIFO правилу.

9. `pool_dispatch_error_rolls_back_counters`:
   - при ошибке постановки в `pool_t` счетчики mailbox откатываются;
   - item, уже извлеченный из mailbox, не возвращается в очередь;
   - `_queued_messages` не расходится с фактическим числом ожидающих item.

10. `parallel_mailbox_orders_only_queued_items`:
   - для parallel mailbox priority ordering применяется только к item, которые еще ожидают внутри mailbox;
   - item, уже переданные в `pool_t`, не вытесняются.

Проверка сборки:

1. Запускать `make` из каталога `/build`.
2. Не использовать `npm run build` для `tegia-node`.

## 10. Критерии приемки

1. `actor_mailbox_t` больше не использует единую FIFO-очередь для ожидающих сообщений.
2. Ожидающие сообщения выбираются по strict priority с FIFO внутри одного priority.
3. Значения priority нормализуются в диапазон `[0, 63]`.
4. Callback chain наследует priority текущего сообщения.
5. Callback на тот же actor instance является обычным сообщением с inherited priority.
6. Backpressure через `max_queue_size` работает по суммарному размеру priority queues.
7. `active/queued/running/idle` не расходятся при enqueue, dispatch, complete и rollback.
8. `stateful` actor не теряет single-flight гарантию: одновременно выполняется не больше одного action одного actor instance.
9. Публичный API actor types не меняется.
10. Документация в `/docs` и `/include/tegia/docs` обновлена.
11. Проект собирается через `make` из `/build`.

## 11. Риски и ограничения

1. Priority FIFO может вызвать starvation низких priority.
2. Priority ordering может нарушить причинный порядок stateful actor, если causally связанные сообщения отправлены с разными priority.
3. Уже переданные в `pool_t` item не переупорядочиваются.
4. Для stateless actor с `max_inflight > 1` часть сообщений может уйти в `pool_t` до появления более высокого priority.
5. В v1 не решается fair scheduling между actor instances. Follow-up: [Task 007 - Case 004](../technical/roadmap/cases/task-007-case-004-actor-instance-fair-scheduling.md).
6. 64 `std::deque` на mailbox принимаются как допустимый memory overhead v1.

## 11.1 Roadmap/backlog artifacts

Отложенные возможности и ограничения v1 вынесены в отдельные roadmap cases:

1. [Task 007 - Case 001: Fairness и aging для priority mailbox](../technical/roadmap/cases/task-007-case-001-priority-fairness-aging.md).
2. [Task 007 - Case 002: Preemption и reprioritization задач, уже переданных в `pool_t`](../technical/roadmap/cases/task-007-case-002-pool-preemption-reprioritization.md).
3. [Task 007 - Case 003: Расширенная политика priority для callback chain](../technical/roadmap/cases/task-007-case-003-callback-priority-policy.md).
4. [Task 007 - Case 004: Fair scheduling между actor instances](../technical/roadmap/cases/task-007-case-004-actor-instance-fair-scheduling.md).

## 12. Actor Model Audit (2026-07-05)

Задача соответствует модели акторов и готова к реализации.

Обоснование:

1. Mailbox остается владельцем внутренней очереди actor instance.
2. Взаимодействие остается message-based.
3. Состояние очереди не выносится в общий mutable singleton.
4. `max_inflight = 1` сохраняет single-flight для `stateful` actor.
5. Изменение causal ordering явно зафиксировано: FIFO сохраняется только внутри одного priority, а разные priority могут переупорядочиваться.
6. Callback на тот же actor instance явно трактуется как обычное новое сообщение с inherited priority.

## 13. DB Schema Migration Guard

Persisted schema/data изменения в текущем объеме задачи не обнаружены.

Подтверждение:

1. Не меняются таблицы, индексы, persisted JSON, status values, durable queues или event stores.
2. Migration path для текущего объема работ не требуется.
3. Если при реализации порядок mailbox будет вынесен в persisted config, DB-backed actor metadata или durable queue/event store, задачу нужно вернуть на migration review до изменения кода.
