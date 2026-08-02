# Task 007 - Case 001: Fairness и aging для priority mailbox

**Статус:** backlog
**Источник:** [task-007.md](../../../tasks/task-007.md)
**Фаза:** TBD
**Приоритет:** TBD

## 1. Контекст

Task-007 вводит `priority_fifo` как strict priority без aging или weighted fairness. Это принято как ограничение v1, чтобы не расширять текущую задачу за пределы mailbox ordering.

## 2. Проблемный сценарий

1. Actor instance получает постоянный поток сообщений с высоким priority.
2. Сообщения с низким priority остаются во внутренней очереди mailbox.
3. Strict priority не дает низким priority гарантии продвижения.

## 3. Последствия

- Возможен starvation низких priority.
- Tail latency низких priority может стать неограниченной.
- Без отдельной политики fairness поведение сложно описывать через SLO.

## 4. Границы ответственности

- `actor_mailbox_t` отвечает за порядок сообщений внутри одного actor instance.
- `threads::pool_t` не решает starvation сообщений, которые еще не были переданы из mailbox.
- Автор actor type отвечает за решение включить `priority_fifo` в v1 с учетом известного ограничения.

## 5. Требования для следующей фазы

- Определить, нужен ли aging, weighted scheduling или другая fairness-политика.
- Определить совместимость новой политики с `stateful` actor.
- Определить метрики ожидания по priority, если fairness будет вводиться.

## 6. Критерии закрытия кейса

1. Принята отдельная политика fairness для priority mailbox или явно подтверждено, что strict priority остается постоянным контрактом.
2. Описаны тесты для starvation/fairness сценариев.
3. Обновлена runtime и SDK-документация.

## 7. Связанные материалы

- [Task-007](../../../tasks/task-007.md)
- [ADR-0001](../../../adr/ADR-0001-actor-execution-model.md)
