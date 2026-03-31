# ADR-0001: Actor Execution Model

- Статус: Черновик
- Дата: 2026-03-03
- Автор: architecture review
- Область: `tegia-node` runtime

## Контекст

Текущее поведение runtime допускает параллельный запуск `action` одного и того же actor instance несколькими worker threads.

Факты из текущего состояния:
- модель проектировалась как близкая к stateless-акторам, но на практике значимая доля акторов stateful;
- для защиты состояния в акторах используются `mutex`, что приводит к непредсказуемой блокировке и деградации throughput;
- отсутствует формальный thread-safety контракт для actor state;
- отсутствует backpressure и формальные SLO/SLI;
- этот риск уже зафиксирован в `docs/architecture.md` как приоритетный.

Цель ADR: выбрать единый execution contract для actor instance и определить минимальный план внедрения в runtime.

## Драйверы решения

1. Корректность: исключить гонки и неявную порчу состояния actor instance.
2. Предсказуемость: сделать поведение runtime и latency более детерминированными.
3. Производительность: сохранить параллелизм между разными акторами без принудительных `mutex` внутри каждого action.
4. Простота разработки: снизить требования к ручной синхронизации в коде конфигураций.
5. Эволюция: оставить путь к multi-node маршрутизации без ломки семантики.

## Рассматриваемые варианты

### Вариант A. Full Parallel (текущая семантика)

Описание:
- любой action любого actor instance может исполняться параллельно;
- thread-safety полностью на стороне автора актора.

Плюсы:
- минимальные изменения runtime;
- потенциально максимальный throughput для строго stateless actor.

Минусы:
- высокий риск гонок в stateful actor;
- сложный и неявный контракт для разработчиков;
- сложная отладка production-инцидентов;
- фактический уход в coarse locking и потерю преимуществ акторной модели.

### Вариант B. Single-threaded Mailbox Per Actor Instance

Описание:
- для каждого actor instance сообщения обрабатываются строго последовательно;
- в каждый момент времени выполняется не более одного action на instance;
- параллелизм достигается за счет разных actor instances.

Плюсы:
- естественная семантика акторной модели;
- убирает большую часть необходимости в `mutex` внутри actor state;
- проще reasoning, тестирование и эксплуатация;
- упрощает формулировку delivery/idempotency контрактов.

Минусы:
- требуется переработка runtime scheduler/очередей;
- возможен head-of-line blocking при долгих action;
- нужно явно вынести долгие I/O/CPU операции в отдельные механизмы.

### Вариант C. Hybrid (декларация режима на actor/action)

Описание:
- по умолчанию последовательная обработка per instance;
- отдельные actor/action могут быть помечены как parallel-safe.

Плюсы:
- компромисс между безопасностью и пиковым throughput.

Минусы:
- усложнение контракта и runtime;
- риск ошибочной маркировки parallel-safe;
- увеличенная стоимость code review и сопровождения.

## Решение

Принять **Вариант B: Single-threaded Mailbox Per Actor Instance** как целевой execution contract runtime.

Нормативное правило:
- runtime гарантирует, что два action одного actor instance не исполняются одновременно.

Дополнительно:
- параллелизм между разными actor instances сохраняется;
- долгие операции не должны монополизировать mailbox: выносятся в task-акторы/внешние очереди/async pipelines;
- любые существующие `mutex`, защищающие только внутреннее состояние одного actor instance, считаются техническим долгом и должны быть удалены после миграции.

## Последствия

Положительные:
- резкое снижение класса data race ошибок;
- упрощение контракта для команд конфигураций;
- более предсказуемые latency профили на уровне instance.

Отрицательные/цена:
- нужно изменить механизм планирования в `actors::map_t` и pool integration;
- нужен механизм очереди per instance и флаг `inflight`;
- потребуется отдельная стратегия для long-running action.

## План внедрения (минимальный)

1. Runtime contract
- Зафиксировать в документации runtime правило single-flight per actor instance.

2. Scheduler changes
- Добавить per-instance mailbox (FIFO) и признак выполнения (`inflight`).
- Логика dispatch:
  - если `inflight=false`, ставить задачу в worker и включать `inflight=true`;
  - если `inflight=true`, сообщение помещать в mailbox без немедленного запуска;
  - по завершении action запускать следующий элемент mailbox либо сбрасывать `inflight=false`.

3. Backpressure v1
- Ввести ограничение размера mailbox на instance.
- При превышении лимита возвращать контролируемую ошибку (например, `429/queue_overflow`) и писать событие в лог.

4. Long-running actions
- Зафиксировать практику: блокирующие/долгие операции переносить в отдельные task-механизмы.

5. Observability v1
- Метрики: mailbox depth per actor, dispatch wait time, action execution time, reject count.

6. Migration
- Режим совместимости на переходный период: feature-flag `actor_execution_model=legacy|serial`.
- По умолчанию в новых кластерах `serial`.

## Влияние на существующий код

Критические точки изменения:
- `actors::map_t::send_message(...)` — постановка/маршрутизация задач;
- callback завершения action — триггер следующего сообщения mailbox;
- `threads::queue`/`pool_t` — интеграция с новой семантикой планирования.

Без изменений в ADR-1:
- remote messaging semantics;
- delivery guarantees между нодами;
- policy engine безопасности.

## Риски и меры

1. Риск: падение throughput на hot actors.
- Мера: вынесение тяжелых action, масштабирование через шардирование actor identity.

2. Риск: рост очередей и latency.
- Мера: mailbox limits + backpressure + метрики и алерты.

3. Риск: регрессии в legacy конфигурациях.
- Мера: feature-flag и поэтапное включение по конфигурациям.

## Критерии приемки ADR-1

1. В runtime документирован и реализован single-flight per actor instance.
2. Для одного actor instance отсутствуют параллельные вызовы action в стресс-тесте.
3. Есть минимум 4 runtime-метрики: queue depth, wait time, execution time, rejects.
4. Есть механизм backpressure при переполнении mailbox.

## Open Issues

1. Нужен ли приоритет внутри mailbox (или только FIFO)?
2. Какой стандартный лимит mailbox per instance принимаем на старте?
3. Нужен ли fair scheduling между "горячими" и "холодными" actor instances?
