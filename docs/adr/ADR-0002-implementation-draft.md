# ADR-0002 Implementation Draft (Phase 1, no breaking changes)

- Статус: Implemented (Phase 1 core)
- Дата: 2026-03-04
- Основание: `ADR-0002-ws-route-registration-model.md`
- Цель этапа: внедрить dual mode (`type-level` + `legacy instance-level`) без изменения поведения существующих конфигураций.

## 1. Границы этапа

Входит в Phase 1:
1. Добавление type-level route metadata.
2. Загрузка type-level routes в `ws_t` при создании instance.
3. Сохранение работоспособности legacy `this->_router->add(...)`.
4. Deprecation warning для legacy пути.
5. Валидация `mapping` и детектор конфликтов.
6. Конфигурируемая политика конфликтов `ws.route.conflict_policy` (обязательная часть Phase 1).

Не входит:
1. Удаление legacy API.
2. Расширение HTTP методов кроме `POST`.
3. Полная миграция всех конфигураций.

## 2. Точки изменения по файлам (реализовано)

### 2.1 `include/tegia/actors/type.h`

Добавить:
1. `struct route_t`.
2. `std::unordered_map<std::string, route_t> route_by_key` (`key = method + " " + pattern`).
3. `std::unordered_map<std::string, std::string> route_action_index` (`action -> key`) для запрета "несколько route на action".
4. API:
   - `int add_route(const std::string &method, const std::string &pattern, const nlohmann::json &data);`
   - `const std::unordered_map<std::string, route_t> & routes() const;`
5. Validation helper:
   - `bool validate_route_data(const nlohmann::json &data, std::string &error) const;`
   - `bool validate_route_mapping(const nlohmann::json &mapping, std::string &error) const;`

Правила в `add_route`:
1. Разрешить только `POST`.
2. Проверять обязательные поля `data.actor`, `data.action`, `data.mapping`.
3. Проверять, что `data.action` начинается с `/`.
4. Проверять отсутствие дубля `method+pattern`.
5. Проверять отсутствие второго route для того же `action`.
6. При ошибке: лог + `exit(0)` (fail-fast).

### 2.2 `src/actors/map.h` и `src/actors/map.cpp`

Добавить read-only accessor к типу:

```cpp
const tegia::actors::type_base_t * get_type(const std::string &name) const;
```

Поведение:
- возвращает `nullptr`, если тип не найден;
- только чтение.

Назначение:
- `ws_t` забирает metadata своего типа через `tegia::threads::thread->actor_map()->get_type(this->type)`.

### 2.3 `include/tegia/ws/router.h` и `src/ws/router.cpp`

Добавить overload для добавления route c источником:

```cpp
enum class route_source_t { type_level, legacy_instance };
bool add(const std::string &method, const std::string &pattern, nlohmann::json data, route_source_t source);
```

Текущее `add(method, pattern, data)` оставить как wrapper:
- вызывает `add(..., route_source_t::legacy_instance)`.

Новая логика `add`:
1. Проверка дубля route key.
2. При конфликте действовать по `ws.route.conflict_policy`:
   - `error`: fail-fast;
   - `legacy_override`: legacy route заменяет ранее добавленный type-level route.
3. Для legacy path писать deprecation warning.

### 2.4 `include/tegia/ws/ws.h` и `src/ws/ws.cpp`

Добавить приватный helper:

```cpp
int load_type_routes();
```

Порядок в конструкторе `ws_t`:
1. Создать `_router` как сейчас.
2. Вызвать `load_type_routes()`.
3. Сохранить текущий legacy-код `this->_router->add(...)`.

`load_type_routes()`:
1. Получить `actor_map` из thread context.
2. Получить `type_meta = actor_map->get_type(this->type)`.
3. Если `type_meta == nullptr`, писать warning `WS_TYPE_METADATA_NOT_FOUND` и возвращать 0 (legacy остается рабочим).
4. Для каждого route из `type_meta->routes()` вызвать `_router->add(..., route_source_t::type_level)`.

Важно:
- Не менять `ws_t::router(...)` и callback pipeline.

### 2.5 `src/ws/router.cpp` (чтение policy из cluster config)

Используем runtime flag:

- `ws.route.conflict_policy`: `legacy_override|error`.

Источник и порядок чтения (реализовано):
1. `cluster.json` path: `/nodes/data/ws/route/conflict_policy`.
2. Если path отсутствует или значение невалидно, использовать default.

Значение по умолчанию для Phase 1:
- `legacy_override` (чтобы не ломать существующие конфигурации при смешанном режиме).

План изменения default:
- после завершения миграции платформенных/прикладных конфигураций переключить default на `error` отдельным решением.

## 3. Контракт `mapping` (v1, fail-fast без ложных срабатываний)

`mapping` валиден, если:
1. Это JSON object.
2. Каждый ключ:
   - строка JSON Pointer (`/a/b`),
   - либо пустая строка `""` (копирование в корень `data`).
3. Каждое значение:
   - любой non-string JSON literal (число, bool, object, array, null), или
   - string-literal, или
   - string JSON Pointer (строка, начинающаяся с `/`).
4. Для string JSON Pointer проверяется синтаксис pointer, но не обязательная существуемость пути на старте (она зависит от runtime payload).

Это обеспечивает fail-fast на структурных ошибках и не ломает текущие рабочие payload-dependent mapping.

## 4. Макросы и API для новых конфигураций

В `include/tegia/actors/type.h` добавлено:

```cpp
#define ADD_WS_ROUTE(method, pattern, data_json) \
    type->add_route(method, pattern, data_json)

#define ADD_WS_ACTION_ROUTE(method, pattern, actor_name, action, func, mapping_json, ...) \
    do { \
        type->add_action(action, "", static_cast<tegia::actors::action_fn_ptr>(func), tegia::user::roles(__VA_ARGS__)); \
        nlohmann::json __r = { \
            {"actor", actor_name}, \
            {"action", action}, \
            {"mapping", mapping_json} \
        }; \
        type->add_route(method, pattern, __r); \
    } while(0)
```

`ADD_ACTION` и `ADD_ACTION2` остаются без изменений.
Примечание: по итоговому замечанию `actor` в `ADD_WS_ACTION_ROUTE` задается явно и может быть как `"{/this}"`, так и именем другого актора.

## 5. Совместимость и deprecation

### 5.1 Гарантия совместимости

1. Legacy-конструкторы с `this->_router->add(...)` продолжают работать.
2. Если type-level routes не объявлены, поведение идентично текущему.
3. Если объявлены только type-level routes, они работают без legacy кода.
4. Если `this->type` не найден в map metadata, поведение не ломается (маршрутизация работает через legacy).

### 5.2 Deprecation сигнал

При вызове legacy `router->add(...)`:
- warning код: `WS_LEGACY_ROUTE_DEPRECATED`;
- в сообщении: actor type, pattern, method;
- warning выводится один раз на actor type за процесс (не на каждый instance).

## 6. Стратегия миграции конфликтов (без аварий)

Phase 1 rollout:
1. Сначала выпуск runtime с dual mode и default `legacy_override`.
2. Перенос платформенных WS-route в type-level выполняется без падений (legacy может временно перекрывать type-level).
3. По мере миграции конфигураций включать `error` точечно в окружениях (staging/canary).
4. После завершения миграции зафиксировать default `error` отдельным ADR/update.

## 7. Тест-план (минимум)

1. `legacy_only`: actor с `ADD_ACTION + this->_router->add`.
- Ожидание: поведение не изменилось.

2. `type_only`: actor с `ADD_WS_ACTION_ROUTE`, без legacy add.
- Ожидание: route работает, права берутся из `ADD_ACTION`.

3. `mixed_no_conflict`: type-level + legacy, разные pattern.
- Ожидание: оба маршрута работают, warning по legacy есть.

4. `mixed_conflict_same_pattern_policy_override`.
- Политика: `legacy_override`.
- Ожидание: падения нет, применяется legacy route, пишется warning о конфликте с кодом `WS_ROUTE_CONFLICT_OVERRIDE`.

5. `mixed_conflict_same_pattern_policy_error`.
- Политика: `error`.
- Ожидание: fail-fast на старте.

6. `invalid_mapping_syntax`.
- Ожидание: fail-fast на старте типа.

7. `second_route_same_action`.
- Ожидание: fail-fast на старте типа.

8. `type_metadata_not_found`.
- Ожидание: warning + корректная работа через legacy.

## 8. Порядок выката

1. Влить runtime изменения dual mode + conflict policy + warnings.
2. Перевести базовые WS маршруты фреймворка на type-level.
3. Выпустить версию и release notes: legacy API deprecated.
4. Мигрировать прикладные конфигурации по одной.
5. После окна миграции перевести default policy в `error`.
6. Отдельным решением отключить legacy путь.

## 9. Риски и контрмеры

1. Риск: скрытые конфликты route.
- Контрмера: явная policy + warning code `WS_ROUTE_CONFLICT_OVERRIDE` + staged switch к `error`.

2. Риск: отсутствие metadata для некоторых типов.
- Контрмера: `WS_TYPE_METADATA_NOT_FOUND` warning + legacy fallback.

3. Риск: шум логов от deprecation.
- Контрмера: warning один раз на actor type за процесс.

## 10. Критерий завершения Phase 1

Phase 1 завершен, когда:
1. runtime поддерживает dual mode;
2. старые конфигурации запускаются без правок;
3. новые конфигурации используют `ADD_WS_ACTION_ROUTE`;
4. есть warnings и документация deprecation;
5. тесты из раздела 7 проходят в обеих policy (`legacy_override`, `error`).
