# WS Routes: индекс

WS route — это декларация сопоставления HTTP запроса с `actor/action` и правилами маппинга данных. На уровне runtime route резолвится в `router_t`, а права доступа проверяются на уровне action. Контракты маршрутов используются как для локальных WS‑акторов, так и для внешних actor (по имени), включая межнодовые вызовы.

Этот файл — корневая точка входа для любых изменений, связанных с WS‑route. Все детали разбиты по типам артефактов:

## Архитектура и решения (статусы)

- `docs/adr/ADR-0002-ws-route-registration-model.md` — В процессе реализации, модель регистрации WS‑route (type‑level + совместимость).
- `docs/adr/ADR-0003-ws-route-authorization.md` — Черновик, проверка прав доступа на уровне route.

## Реализованные задачи и планирование (статусы)

- `docs/tasks/task-001.md` — Выполнено, реализация ADR‑0002 (Phase 1).
- `docs/tasks/task-002.md` — Выполнено, внешний actor в WS‑route и документация.
- `docs/tasks/task-003.md` — Выполнено, новая сигнатура `ADD_WS_ACTION_ROUTE`.
- `docs/tasks/task-004.md` — Выполнено, базовая валидация JSON в `ADD_WS_ACTION_ROUTE`.

## Пользовательская документация (SDK)

- `include/tegia/docs/ws-routing-contract.md` — контракт WS‑маршрутов, правила `mapping`, примеры.
- `include/tegia/docs/quickstart-new-actor.md` — минимальные правила для новых акторов и routes.
