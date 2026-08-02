# Task 007 - Case 003: Расширенная политика priority для callback chain

**Статус:** backlog
**Источник:** [task-007.md](../../../tasks/task-007.md)
**Фаза:** TBD
**Приоритет:** TBD

## 1. Контекст

Task-007 ограничивает v1 правилом: callback наследует priority текущего mailbox item. Расширение `message_t::callback` отдельным priority и отдельная callback priority policy не входят в текущую задачу.

## 2. Проблемный сценарий

1. Action создает callback chain.
2. Бизнес-сценарий требует, чтобы следующий callback имел priority, отличный от исходного сообщения.
3. В v1 у callback нет отдельного persisted или runtime priority metadata.

## 3. Последствия

- Callback chain не может выразить повышение или понижение priority без изменения API.
- Все callback-сообщения наследуют priority исходного mailbox item.
- Для сложных pipelines может потребоваться отдельная policy.

## 4. Границы ответственности

- `map_t::action_func(...)` или mailbox dispatch layer отвечает за передачу inherited priority в v1.
- `message_t::callback` в v1 не хранит собственный priority.
- Автор action не получает нового API для callback priority в Task-007.

## 5. Требования для следующей фазы

- Определить, нужен ли priority в `message_t::callback`.
- Определить backward-compatible формат callback metadata.
- Определить правила для callback на тот же actor instance и на другой actor instance.

## 6. Критерии закрытия кейса

1. Принято решение о необходимости отдельного callback priority.
2. Если изменение требуется, описан API и миграционный путь для существующих callback сценариев.
3. Обновлены тесты callback chain.

## 7. Связанные материалы

- [Task-007](../../../tasks/task-007.md)
- [message-contract.md](../../../../include/tegia/docs/message-contract.md)
