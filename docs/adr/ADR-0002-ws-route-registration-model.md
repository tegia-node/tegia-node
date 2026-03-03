# ADR-0002: WS Route Registration Model (Type-Level + Backward Compatibility)

- Статус: Proposed
- Дата: 2026-03-04
- Автор: architecture review
- Область: `tegia-node` runtime, `ws_t` и API регистрации action/route

## Контекст

Сейчас для WS-актора одна и та же операция декларируется в двух местах:
1. action через `ADD_ACTION(...)` (на уровне типа актора);
2. HTTP route через `this->_router->add(...)` (на уровне экземпляра `ws_t`, обычно в конструкторе).

Проблемы:
- дублирование деклараций;
- рассинхронизация action и route;
- instance-level роутинг при том, что action-level контракт уже type-level.

Из уточнений в `docs/route.md`:
- internal-only actions допустимы (route не обязателен для каждого action);
- источником истины по правам доступа является `roles` из `ADD_ACTION`;
- несколько routes на один action запрещены;
- в базовом контракте пока только `POST`;
- валидация `mapping` должна быть fail-fast при загрузке типа;
- дубли route должны быть фатальной ошибкой загрузки;
- placeholder `{/this}` сохраняется.

## Драйверы решения

1. Не ломать существующие конфигурации и обеспечить плавную миграцию.
2. Убрать дублирование и задать единый declarative путь для новых WS-акторов.
3. Сделать ошибки конфигурации маршрутов детектируемыми на старте.
4. Сохранить текущую runtime-семантику обработки HTTP -> ws router -> callback.

## Решение

Ввести **type-level регистрацию WS-route** как целевой механизм и оставить текущий instance-level путь как deprecated на переходный период.

### 1) Новый целевой API

- Расширить `type_base_t` route-метаданными:
  - `route_t { method, pattern, actor, action, mapping }`
  - `add_route(...)` с валидацией.
- Добавить макрос (рабочее имя) `ADD_WS_ACTION_ROUTE(...)`, который в одной декларации регистрирует:
  - `add_action(...)`;
  - `add_route(...)`.

### 2) Семантические правила (нормативно)

1. `roles` проверяются только на уровне `action` (`ADD_ACTION` / `add_action`).
2. `route.role` не используется и не вводится в новый контракт.
3. Один route -> один action; один action может иметь ноль или один route.
4. Допустимый HTTP метод в v1: только `POST`.
5. Для WS actor placeholder `{/this}` в `actor` поддерживается как стандарт.
6. `mapping` валидируется при загрузке типа (fail-fast).
7. Дубликат route (одинаковые `method+pattern` внутри типа) приводит к фатальной ошибке загрузки типа.
8. Дубликат action-route (попытка повесить второй route на тот же action) приводит к фатальной ошибке загрузки типа.

### 3) Backward compatibility (обязательно)

Переходный период поддерживает оба способа:
- **Новый**: type-level (`ADD_WS_ACTION_ROUTE` / `add_route`).
- **Старый**: instance-level `this->_router->add(...)` в конструкторе WS-актора.

Старый путь:
- остается функциональным без изменения поведения;
- помечается как deprecated в документации и в runtime-warning (при первом вызове `router->add` из actor-кода).

Приоритет применения маршрутов на переходе:
1. Сначала загружаются type-level routes.
2. Затем применяются instance-level routes.
3. При конфликте (`method+pattern`):
   - по умолчанию fail-fast (фатальная ошибка), чтобы не получить неявное переопределение;
   - временный escape hatch через feature-flag `ws.route.conflict_policy=error|legacy_override` (default=`error`).

## Почему это решение

1. Дает единый правильный контракт для новых акторов без форсированного переписывания старых.
2. Убирает риск silent-break из-за дублирующих деклараций.
3. Позволяет мигрировать по конфигурациям и по типам, а не "большим взрывом".
4. Сохраняет текущую архитектуру `router_t::match` и callback-пайплайн.

## План внедрения

### Этап 1. Runtime поддержка dual mode

1. Добавить route registry в `type_base_t` и API `add_route`.
2. Добавить валидацию route schema и mapping schema.
3. В `ws_t` при инициализации `_router` сначала подгружать routes из metadata типа.
4. Оставить рабочим `this->_router->add(...)` и добавить deprecation warning.
5. Добавить конфликт-детектор между type-level и instance-level routes.

### Этап 2. Базовые WS маршруты платформы

1. Перенести системные WS routes (`/member/add`, `/member/accept`, `/member/remove`, при необходимости `/init`, `/router`) в type-level декларации.
2. Удалять дубли из конструктора `ws_t` только после проверки совместимости.

### Этап 3. Миграция прикладных конфигураций

1. Для новых actor types использовать только `ADD_WS_ACTION_ROUTE`.
2. Для существующих — постепенная миграция с сохранением старого кода.
3. После миграции каждого типа удалять instance-level `router->add(...)` из конструктора.

### Этап 4. Завершение deprecation

1. После согласованного окна миграции перевести deprecated путь в hard error.
2. Удалить legacy-ветку и feature-flag `legacy_override`.

## Deprecation policy

- Версия N: поддержка dual mode, предупреждения включены.
- Версия N+1: dual mode сохраняется, но warning повышается до high-priority (и в release notes).
- Версия N+2: legacy instance-level route registration отключается (или допускается только явным legacy-flag для аварийного отката).

(Номера версий подставляются после согласования release cadence.)

## Критерии приемки

1. Старые WS-акторы (с `this->_router->add`) работают без изменения поведения.
2. Новые WS-акторы можно объявлять через единый API без дублирования action/route.
3. Дубликаты route и invalid mapping обнаруживаются на этапе загрузки типа.
4. В логах есть явное deprecation-сообщение при использовании старого пути.
5. Есть интеграционные тесты минимум для 4 сценариев:
   - legacy-only;
   - type-level-only;
   - mixed без конфликтов;
   - mixed с конфликтом.

## Open Issues

1. Нужен ли отдельный helper для генерации `mapping` (чтобы убрать JSON literal из макроса)?
2. Нужно ли поддержать в будущем методы кроме `POST` (это потребует расширения тестового контракта)?
3. Должен ли deprecation warning быть единоразовым на тип или на каждый actor instance?
