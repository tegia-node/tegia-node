# Task 007 - Case 004: Fair scheduling между actor instances

**Статус:** backlog
**Источник:** [task-007.md](../../../tasks/task-007.md)
**Фаза:** TBD
**Приоритет:** TBD

## 1. Контекст

Task-007 ограничен порядком сообщений внутри одного `actor_mailbox_t`. Fair scheduling между разными actor instances в v1 не решается.

## 2. Проблемный сценарий

1. Один actor instance генерирует большой поток сообщений и часто продвигает свой mailbox в `pool_t`.
2. Другие actor instances конкурируют за общий `threads::pool_t`.
3. Внутренний priority mailbox не задает fairness между разными actor instances.

## 3. Последствия

- Hot actor может ухудшать latency других actor instances.
- Priority внутри mailbox не заменяет общую scheduling policy runtime.
- SLO между actor instances требует отдельной политики.

## 4. Границы ответственности

- `actor_mailbox_t` отвечает за локальную очередь одного actor instance.
- `threads::pool_t` и будущий scheduler отвечают за глобальное распределение worker capacity.
- Task-007 не меняет глобальную политику `pool_t`.

## 5. Требования для следующей фазы

- Определить, нужна ли per-actor fairness или квотирование.
- Определить метрики hot actor и mailbox contribution в общий pool.
- Согласовать решение с существующей priority-моделью `threads::queue`.

## 6. Критерии закрытия кейса

1. Принято отдельное решение по fairness между actor instances.
2. Определены метрики и тесты hot/cold actor scenarios.
3. Обновлена runtime-документация.

## 7. Связанные материалы

- [Task-007](../../../tasks/task-007.md)
- [ADR-0001](../../../adr/ADR-0001-actor-execution-model.md)
