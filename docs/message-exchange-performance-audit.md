# Аудит производительности обмена сообщениями

- Статус: черновик
- Область: `tegia-node` core runtime
- Дата: 2026-06-29

## 1. Область аудита

Аудит покрывает горячий путь обмена сообщениями:

1. `tegia::message::send(...)`.
2. `actors::map_t::send_message(...)`.
3. `actor_mailbox_t`.
4. `threads::pool_t`, `threads::queue`, `worker_t`.
5. Callback chain после выполнения action.
6. HTTP/WS route path как внешний источник actor messages.

Аудит статический: выводы основаны на чтении кода, без профилирования. Перед крупными изменениями нужно добавить микробенчмарки для `send_message`, `pool_t::add_task`, mailbox enqueue/complete и HTTP route dispatch.

## 2. Краткий вывод

В текущем коде есть несколько узких мест, которые критичны именно для core runtime:

- общий mutex на единственной очереди задач всего worker pool;
- ожидание `condition_variable` без predicate в worker loop;
- динамическое выделение памяти и UUID на каждую задачу pool;
- глобальный `shared_mutex` actor map на каждый message;
- строковые конкатенации и lookup action через `_actions[type + action]` на горячем пути;
- несколько уровней синхронизации на одно сообщение: actor map lock, mailbox mutex, global queue mutex, atomic counters;
- использование `nlohmann::json`, `std::function`, `std::bind`, `shared_ptr` и callback mutex в критичном пути;
- отладочный вывод в `ws_t::router(...)` на каждый HTTP route.

## 3. Критичные проблемы

### 3.1 Очередь pool: `wait` без predicate

Файлы:

- `src/threads/worker_t.cpp:40-45`
- `src/threads/queue.h:95-100`

`worker_t::thread_fn()` вызывает `cv.wait(locker)` без predicate. Комментарий говорит, что поток должен проснуться, если очередь не пустая, но это условие в коде не проверяется.

Риски:

- потерянное уведомление: задача добавлена и `notify_one()` уже вызван, но worker еще не вошел в `wait`;
- ложное пробуждение: worker проснулся, но очереди пусты;
- непредсказуемая задержка доставки сообщений под нагрузкой.

Рекомендация:

- заменить на `cv.wait(locker, [&]{ return !empty_locked() || !is_work; })`;
- сделать `queue::empty_locked()` быстрым тестом по `tasks_bitset`;
- добавить тест на сценарий "задача добавлена до wait".

### 3.2 Одна глобальная очередь и один mutex на весь worker pool

Файлы:

- `src/threads/queue.h:55-75`
- `src/threads/queue.h:87-127`
- `src/threads/worker_t.cpp:40-64`

Все producers и все workers конкурируют за один `queue::mutex`. Даже если задач много и workers много, `add()` и `get()` сериализуются через один lock.

Риски:

- contention растет вместе с числом worker threads и числом senders;
- latency одного сообщения зависит от общей активности всего pool;
- stateless actors с большим потоком сообщений будут давить на тот же mutex, что и stateful actors.

Рекомендация:

- краткосрочно: минимизировать время под mutex, объединить `get()` с lock-инвариантом и использовать predicate;
- среднесрочно: перейти на MPMC queue без 64 отдельных `std::queue`;
- долгосрочно: per-worker queues + work stealing или sharded global queues по priority/class.

### 3.3 Динамический `task` и UUID на каждую задачу

Файлы:

- `src/threads/pool_t.cpp:73-78`
- `src/threads/queue.h:31-45`
- `src/core/crypt.cpp:87-104`

`pool_t::add_task(...)` на каждую задачу делает:

- `new task`;
- `tegia::random::uuid()`;
- присваивание `std::function`;
- последующий `delete task` в worker.

Поле `task::uuid` в текущем worker path не используется. UUID создается через `uuid_generate`, затем строка создается через heap-буфер `new char[37]`.

Риски:

- лишние heap allocations на каждый message;
- высокая стоимость генерации UUID;
- allocator contention под нагрузкой;
- увеличение tail latency.

Рекомендация:

- убрать UUID из `threads::task` или сделать его опциональным только для debug/tracing;
- хранить task by value в очереди или использовать pool allocator;
- заменить `new/delete` на preallocated slab/ring или `std::unique_ptr` как промежуточный шаг;
- добавить счетчики: tasks/sec, queue wait time, executor overhead.

### 3.4 `pool_t::add_task()` всегда возвращает `0`

Файл:

- `src/threads/pool_t.cpp:73-78`

`queue::add(...)` имеет return code, но `pool_t::add_task(...)` его игнорирует и всегда возвращает успех.

Риски:

- mailbox считает item успешно переданным в pool даже при ошибке;
- будущий backpressure не сможет корректно работать;
- на ошибке priority вне диапазона возможна потеря задачи без корректной эскалации.

Рекомендация:

- возвращать реальный код `queue::add(...)`;
- не создавать task до нормализации priority;
- при ошибке освобождать task и возвращать код наверх.

## 4. Горячий путь `map_t::send_message`

### 4.1 `shared_mutex` actor map на каждый message

Файлы:

- `src/actors/map.cpp:342-348`
- `src/actors/map.cpp:389-605`

Каждый send берет `shared_lock` для поиска actor instance. При miss берется `unique_lock`, под которым выполняется:

- разбор actor name;
- поиск domain/pattern;
- создание actor instance;
- создание mailbox;
- поиск action;
- schema validation.

Риски:

- actor creation блокирует все остальные sends;
- медленный конструктор актора или schema validation под unique lock ухудшает tail latency всей платформы;
- при высокой churn-нагрузке actor map становится центральной точкой contention.

Рекомендация:

- fast path для уже существующего actor сделать без глобального shared mutex через immutable registry snapshot, RCU-подобный подход или striped locks;
- при miss делать double-check под unique lock, а тяжелое создание выносить за пределы глобального lock;
- разделить locks для `_actors`, `_actions`, `_patterns`, `_domains`;
- заранее строить route/pattern индекс, чтобы создание actor не запускало строковый state machine.

### 4.2 Lookup action через конкатенацию строк и `operator[]`

Файл:

- `src/actors/map.cpp:348`

На горячем пути существующего актора выполняется:

```cpp
auto _action = this->_actions[pos->second._actor->type + action];
```

Проблемы:

- создается временная строка `type + action` на каждый message;
- `operator[]` у `unordered_map` может вставить новый элемент при miss;
- операция выполняется под `shared_lock`, но потенциальная вставка является записью;
- при неизвестном action можно получить `nullptr` и последующий неконтролируемый путь.

Рекомендация:

- заменить на `find()` без мутации;
- хранить action index внутри `actor_entry_t` или `type_base_t` как `unordered_map<std::string, action_t*>` по action без префикса type;
- для часто вызываемых action добавить interned id или `action_id`;
- при создании `actor_entry_t` сохранить указатель на `type_base_t`, чтобы не собирать ключ `type + action`.

### 4.3 Повторный разбор actor name при miss

Файл:

- `src/actors/map.cpp:408-520`

При первом сообщении actor name разбирается вручную посимвольно, с `substr(...)` и lookup в `_domains`/`_patterns`.

Риски:

- высокая цена создания большого числа actor instances;
- множество временных строк;
- блокировка actor map на время разбора.

Рекомендация:

- компилировать patterns в trie/radix tree при загрузке конфигурации;
- использовать `std::string_view` при разборе;
- кешировать результат pattern resolution для actor name prefix;
- после mailbox-интеграции рассмотреть отдельный actor factory cache.

### 4.4 Возможная гонка и leak при одновременном создании actor

Файл:

- `src/actors/map.cpp:389-600`

После miss под `shared_lock` код берет `unique_lock`, но не делает повторный lookup `_actors.find(name)`. Если несколько потоков одновременно не нашли actor под shared lock, первый создаст actor, а второй под unique lock тоже создаст actor и вызовет `try_emplace`. При `inserted == false` новый `_actor` не удаляется.

Это влияет и на производительность:

- лишние actor constructors;
- лишние allocations mailbox;
- потенциальные утечки.

Рекомендация:

- после входа под `unique_lock` повторить `_actors.find(name)`;
- если actor уже появился, перейти в обычный enqueue path;
- тяжелое создание actor делать через двухфазный алгоритм или per-key creation lock.

## 5. Mailbox

### 5.1 Несколько синхронизаций на одно сообщение

Файлы:

- `src/actors/actor_mailbox.cpp:24-47`
- `src/actors/actor_mailbox.cpp:137-180`
- `src/actors/map.cpp:245-280`

При отправке одного сообщения сейчас выполняются:

- `actor_t::messages.fetch_add`;
- lock mailbox mutex;
- push в `std::deque`;
- unlock;
- повторный lock в `dispatch_available`;
- pop из `std::deque`;
- `pool_t::add_task`;
- lock global queue mutex;
- при завершении: atomic decrement, mailbox lock, затем возможно новый dispatch.

Риски:

- mailbox добавил семантику stateful, но для stateless actor это стало дополнительной ценой относительно старого direct `pool_t::add_task`;
- один stateful actor естественно сериализован, но stateless actor тоже платит за mailbox queue/mutex;
- `std::deque` может выделять память блоками.

Рекомендация:

- объединить enqueue и dispatch под одним проходом, без повторного lock/unlock там, где это возможно;
- для stateless fast path: если `running < max_inflight` и очередь пуста, сразу dispatch без помещения в очередь;
- заменить `std::deque` на bounded ring buffer для mailbox;
- убрать один из счетчиков `actor_t::messages` / `mailbox.active`;
- использовать `memory_order_relaxed` для счетчиков, если они только учетные.

### 5.2 `steady_clock::now()` на каждый enqueue

Файл:

- `src/actors/actor_mailbox.cpp:31`

`enqueued_at` нужен для будущих метрик, но сейчас timestamp берется всегда.

Рекомендация:

- включать timestamp только при включенной observability;
- либо собирать coarse timestamp батчами/сэмплированием.

### 5.3 Копирование `actor_mailbox_item_t`

Файлы:

- `src/actors/actor_mailbox.cpp:141-180`
- `src/actors/actor_mailbox.h:25-45`

`actor_mailbox_item_t` содержит несколько `shared_ptr` и передается/захватывается через value. Каждый copy/move shared_ptr увеличивает/уменьшает atomic refcount.

Рекомендация:

- сделать task move-only и исключить лишние копии;
- `dispatch_fn_t` заменить на легковесный function pointer/context или template callback;
- рассмотреть хранение `user` как raw/borrowed pointer с ясным lifetime contract, если user immutable в пределах message.

## 6. Message object и callback

### 6.1 UUID на каждый `message_t`

Файлы:

- `src/actors/message_t.cpp:72-82`
- `src/core/crypt.cpp:87-104`

Каждый `message_t` получает UUID, даже если он не используется в dispatch.

Рекомендация:

- сделать message uuid lazy;
- генерировать uuid только при логировании/tracing/external correlation;
- хранить UUID как 16-byte binary, строку строить только при выводе.

### 6.2 Копирование JSON в конструкторе `message_t`

Файл:

- `src/actors/message_t.cpp:72-75`

Конструктор принимает `nlohmann::json _data` by value, но поле инициализируется как `data(_data)`, то есть происходит copy вместо move.

Рекомендация:

- заменить на `data(std::move(_data))`;
- `tegia::message::init(...)` использовать `std::make_shared<message_t>(std::move(data))`;
- для системных сообщений без payload избегать создания пустого JSON object, если action его не читает.

### 6.3 Callback stack с mutex на каждый action completion

Файлы:

- `src/actors/message_t.cpp:29-61`
- `src/actors/map.cpp:235-240`

`callback_t::get()` берет mutex после каждого action. Обычно callback принадлежит одному `message_t` и читается строго в completion path.

Рекомендация:

- если callback не должен изменяться конкурентно, убрать mutex;
- заменить `std::stack` на `std::vector<addr_t>` с pop_back;
- вернуть `std::optional<addr_t>` вместо `addr_t{false,"",""}`.

## 7. Permissions и action execution

### 7.1 Проверка прав выполняется внутри worker

Файлы:

- `src/actors/map.cpp:163-240`
- `src/actors/map.cpp:370-372`

В коде есть TODO: проверку прав делать до добавления задачи в pool. Сейчас unauthorized message занимает mailbox slot и worker task, а ошибка 403 формируется уже в worker.

Рекомендация:

- вынести легкую проверку session roles до enqueue;
- для WS roles оставить actor-specific часть в action path или кешировать membership;
- не ставить очевидно запрещенные сообщения в pool.

### 7.2 `actor->roles(uuid)` вызывается для каждого action

Файл:

- `src/actors/map.cpp:173-198`

Для обычных actor это дешево, но virtual call выполняется всегда. Для `ws_t` внутри может быть lookup membership.

Рекомендация:

- пометить action/type как требующие WS roles только если roles mask содержит WS-биты;
- для action без WS-битов пропускать `actor->roles(...)`;
- кешировать `user_roles_u64` в thread context на время action.

## 8. HTTP/WS route path

### 8.1 Отладочный вывод на каждый route

Файл:

- `src/ws/ws.cpp:296-325`

`ws_t::router(...)` пишет в `std::cout`, вызывает `tegia::threads::tid()` и `user()->print()` на каждый запрос.

Риск:

- синхронизированный вывод в stdout способен доминировать над всей стоимостью request dispatch;
- `tid()` строит строку из `std::thread::id`;
- `user()->print()` печатает много строк.

Рекомендация:

- убрать вывод из hot path или закрыть compile-time/runtime log level check без построения строк;
- оставить structured trace только по sampling или debug flag.

### 8.2 Route matching строит JSON и строки на каждый запрос

Файлы:

- `src/ws/router.cpp:400-493`
- `src/ws/router.cpp:459-488`

`router_t::match(...)`:

- конкатенирует строки pattern fragments;
- копирует route JSON: `nlohmann::json _params = params_name`;
- копирует весь post: `_params["post"] = *post`;
- каждый раз создает `json_pointer`;
- строит `data` через JSON mapping.

Рекомендация:

- route registration должен компилировать mapping в структуру: actor template, action template, список pre-parsed json pointers;
- match должен возвращать легковесный result: actor, action, data;
- для статических actor/action без `{...}` не вызывать шаблонизатор `actor_name/action_name`;
- избегать копирования всего `post`, копировать только нужные поля.

## 9. Прочие наблюдения

### 9.1 Detached worker threads и отсутствие shutdown

Файлы:

- `src/threads/manager_t.cpp:43-90`
- `src/threads/worker_t.h:31-35`

Это не прямой hot-path overhead, но мешает корректным нагрузочным тестам и graceful shutdown. Без остановки workers трудно собирать стабильные метрики и проверять отсутствие потерь.

### 9.2 `node::send_message(message, fn, priority)` не ставит задачу в pool

Файл:

- `src/node/node.cpp:48-74`

Метод формирует lambda, но не передает ее в executor и возвращает `200`. Если этот путь используется или будет использоваться для runtime callbacks, он не соответствует ожиданию API.

### 9.3 Ошибки логируются через `std::cout` в runtime path

Во многих ошибочных ветках `map_t::send_message` и `action_func` строятся большие строки и пишутся в stdout. Это нормально для редких ошибок, но при массовых 403/404 такая обработка станет DoS-фактором.

Рекомендация:

- rate limit для повторяющихся ошибок;
- structured logger с lazy field formatting.

## 10. Рекомендуемый порядок оптимизации

### Этап 1. Быстрые исправления с большим эффектом

1. Исправить `worker_t::thread_fn()` на `cv.wait(lock, predicate)`.
2. Убрать UUID из `threads::task`.
3. Вернуть реальный код из `pool_t::add_task()`.
4. Заменить `_actions[type + action]` на `find()` и убрать мутацию под `shared_lock`.
5. В `message_t(nlohmann::json)` использовать `std::move`.
6. Убрать `std::cout` и `user()->print()` из `ws_t::router(...)`.
7. Добавить double-check actor existence под `unique_lock` при создании actor.

### Этап 2. Снижение contention

1. Перепроектировать `threads::queue` в MPMC queue или sharded queues.
2. Ввести fast path mailbox для stateless actor.
3. Убрать дублирование `actor_t::messages` и `mailbox.active`.
4. Разделить actor registry locks по структурам или перейти к immutable snapshots.
5. Скомпилировать actor patterns и HTTP routes в trie/precompiled descriptors.

### Этап 3. Архитектурные оптимизации

1. Ввести typed runtime envelope вместо обязательного `nlohmann::json` для routing metadata.
2. Использовать action ids вместо строковых action lookup.
3. Сделать message uuid lazy/binary.
4. Заменить `std::function`/`std::bind` в executor на move-only task с small-buffer optimization или template enqueue.
5. Добавить observability: queue length, enqueue rate, dispatch latency, actor mailbox depth, worker utilization, lock wait time.

## 11. Метрики для подтверждения

Минимальные бенчмарки:

1. `tegia::message::send` existing actor, empty action, без HTTP.
2. `tegia::message::send` stateful actor, 1 actor, N producers.
3. `tegia::message::send` stateless actor, 1 actor, N producers.
4. Actor creation throughput по pattern.
5. `pool_t::add_task` throughput без actor map.
6. HTTP route dispatch `/ws/router -> actor callback`.

Минимальные метрики:

- p50/p95/p99 enqueue latency;
- p50/p95/p99 queue wait;
- tasks/sec;
- lock contention time для actor map, mailbox, pool queue;
- allocations/message;
- callback chain latency;
- actor mailbox queued/running/active.

