# Task-003: Изменение сигнатуры `ADD_WS_ACTION_ROUTE`

- Статус: Выполнено
- Дата: 2026-03-26
- Основание: `docs/adr/ADR-0002-ws-route-registration-model.md`
- Цель: упростить декларацию `ADD_WS_ACTION_ROUTE`, передавая `actor/action/mapping` одним JSON‑объектом.

## 1. Контекст

Текущая сигнатура требует передавать `actor` и `action` отдельными аргументами, а `mapping` — отдельным JSON. Предлагается объединить их в единый JSON‑объект, чтобы запись была компактнее и симметрична `ADD_WS_ROUTE`.

## 2. Текущее и целевое API

Сейчас:

```cpp
ADD_WS_ACTION_ROUTE(
    "POST", "/clients/count",
    "{/this}", "/clients/count",
    &PDP::WS::CLIENTS::clients_count,
    R"({
        "": "/post/data"
    })"_json,
    ROLES::WS::MEMBER
);
```

Должно быть:

```cpp
ADD_WS_ACTION_ROUTE(
    "POST", "/clients/count",
    R"({
        "actor": "{/this}",
        "action": "/clients/count",
        "mapping": {
            "": "/post/data"
        }
    })"_json,
    &PDP::WS::CLIENTS::clients_count,
    ROLES::WS::MEMBER
);
```

Итоговая сигнатура:

```cpp
ADD_WS_ACTION_ROUTE(
    const std::string &method,
    const std::string &pattern,
    const nlohmann::json &route_json,
    tegia::actors::action_fn_ptr func,
    ...
)
```

## 3. Объем работ

Входит:
1. Обновить макрос `ADD_WS_ACTION_ROUTE` в `include/tegia/actors/type.h`.
2. Обновить примеры и контракт в SDK‑документации.

Не входит:
1. Изменение поведения `ADD_WS_ROUTE`.
2. Изменения runtime‑валидации маршрутов (контракт остается тем же).

## 4. Системная аналитика

### 4.1 Изменения интерфейса

- Сигнатура макроса меняется: `actor/action/mapping` передаются единым JSON‑объектом.
- Семантика не меняется, меняется только форма декларации.

### 4.2 Влияние на код

- Миграция использования `ADD_WS_ACTION_ROUTE` в `*-conf` выполняется вручную и не входит в эту задачу.
- Документация должна быть синхронизирована, чтобы не осталось старых примеров.

### 4.3 Риски

- Частичная миграция приведет к ошибкам компиляции.
- Нужна синхронная правка всех мест использования, иначе сборка упадет.

## 5. Точки изменения по файлам

1. `include/tegia/actors/type.h` — новая сигнатура макроса и сборка JSON‑контейнера.
2. `include/tegia/docs/ws-routing-contract.md` — обновить пример и описание `ADD_WS_ACTION_ROUTE`.
3. `include/tegia/docs/quickstart-new-actor.md` — обновить рекомендации/пример.
4. Все использования `ADD_WS_ACTION_ROUTE` внутри репозитория `tegia-node` (включая документацию и тесты).

## 6. Критерии приемки

1. `tegia-node` компилируется без использования старой сигнатуры (конфигурации `*-conf` в отдельных репозиториях не входят в критерий).
2. В документации нет старых примеров `ADD_WS_ACTION_ROUTE`.
3. Новый формат корректно регистрирует `actor/action/mapping` в `route.data`.

## 7. Итоговые условия

1. Обратная совместимость со старой сигнатурой не требуется, переход без deprecation.
2. Новый `ADD_WS_ACTION_ROUTE` принимает `nlohmann::json` и не имеет отдельных параметров `actor/action`.

## 8. Итоговые условия (дополнительно)

1. Отдельный task на миграцию `*-conf` не требуется; правки выполняются по месту в ходе разработки/рефакторинга.

## 9. Открытые вопросы и замечания

1. Новых вопросов нет.
