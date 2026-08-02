# Task 007 - Case 002: Preemption и reprioritization задач, уже переданных в `pool_t`

**Статус:** backlog
**Источник:** [task-007.md](../../../tasks/task-007.md)
**Фаза:** TBD
**Приоритет:** TBD

## 1. Контекст

Task-007 фиксирует, что priority влияет только на сообщения, которые еще находятся во внутренней очереди mailbox. Item, уже переданный в `pool_t`, не переупорядочивается и не вытесняется.

## 2. Проблемный сценарий

1. Mailbox передает low-priority item в `pool_t`.
2. Позже в тот же actor instance приходит high-priority сообщение.
3. High-priority сообщение не может вытеснить item, уже переданный в `pool_t`.

## 3. Последствия

- Priority mailbox не дает полного real-time priority contract.
- Для stateless actor с `max_inflight > 1` часть low-priority work может стартовать до появления high-priority work.
- Ожидания авторов конфигураций должны быть ограничены текущим контрактом v1.

## 4. Границы ответственности

- `actor_mailbox_t` управляет только своей внутренней очередью.
- `threads::pool_t` в рамках Task-007 не поддерживает preemption или reprioritization.
- Worker threads не прерывают уже запущенную action.

## 5. Требования для следующей фазы

- Определить, нужен ли runtime-механизм reprioritization задач в `pool_t`.
- Определить, допустима ли preemption для action в текущей модели выполнения.
- Определить влияние на callback chain и single-flight для `stateful` actor.

## 6. Критерии закрытия кейса

1. Принято отдельное архитектурное решение: preemption/reprioritization требуется или явно не поддерживается.
2. Если требуется, описан контракт `pool_t` и worker lifecycle.
3. Добавлены тесты для сценариев late high-priority message.

## 7. Связанные материалы

- [Task-007](../../../tasks/task-007.md)
- [actor-execution-transition.md](../../../actor-execution-transition.md)
