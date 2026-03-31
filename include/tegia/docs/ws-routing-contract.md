# Контракт WS-маршрутизации

Документ для акторов, наследующихся от `tegia::actors::ws_t`.

## 1. Как объявлять route

Используй type-level декларацию:

```cpp
ADD_WS_ACTION_ROUTE(
    "POST",
    "/clients/count",
    nlohmann::json::parse(R"({
        "actor": "{/this}",
        "action": "/clients/count",
        "mapping": {
            "": "/post/data"
        }
    })"),
    &MyWsActor::clients_count,
    ROLES::WS::MEMBER
);
```

Что делает макрос:
- регистрирует action;
- регистрирует HTTP route;
- связывает route -> actor/action.

Обязательные поля `route_json` для `ADD_WS_ACTION_ROUTE`:
- `actor` (string);
- `action` (string, начинается с `/`);
- `mapping` (object).

Внешний actor (без локального action) объявляется через `ADD_WS_ROUTE`. Для внешнего actor **нельзя** использовать `ADD_WS_ACTION_ROUTE`, так как он регистрирует локальный action.

Пример внешнего маршрута:

```cpp
ADD_WS_ROUTE(
    "POST",
    "/sync/client",
    nlohmann::json::parse(R"({
        "actor": "crm/client",
        "action": "/sync",
        "mapping": {
            "": "/post/data"
        }
    })")
);
```

Пример с использованием параметров из `pattern` в имени actor:

```cpp
ADD_WS_ROUTE(
    "POST",
    "/events/001/{uuid}/status",
    nlohmann::json::parse(R"({
        "actor": "data/pdp/events/0001/{/uuid}",
        "action": "/status",
        "mapping": {}
    })")
);
```

## 2. Ограничения текущего runtime

- поддерживается только метод `POST` (другой метод - фатальная ошибка загрузки типа);
- `mapping` обязателен и должен быть JSON object;
- route key (`method + pattern`) должен быть уникален внутри типа;
- один action не может иметь несколько routes (alias не поддержан);
- `route.data.action` должен начинаться с `/`.

## 3. Правила `mapping`

`mapping` переносит данные в `message->data`:
- ключ mapping - JSON pointer в `data` (например `"/user/id"` или `""`);
- значение:
  - string c `/...` - читать из контекста (`/post/...`, `/params/...`);
  - string без `/` - literal string;
  - non-string - literal JSON value.

Пример:

```json
{
  "/user/id": "/post/user_id",
  "/source": "http",
  "/flags/is_new": true
}
```

## 4. Placeholder'ы

- `actor: "{/this}"` заменяется на текущее имя workspace;
- в `actor` и `action` поддерживается подстановка параметров из route через `{...}`.

## 5. Legacy-путь

`this->_router->add(...)` в конструкторе WS-актора остается рабочим для совместимости, но считается устаревшим (deprecated).

Для новых акторов используй только `ADD_WS_ACTION_ROUTE`.

## 6. Безопасность доступа

- доступ проверяется по roles action (из `ADD_WS_ACTION_ROUTE`/`ADD_ACTION`);
- route-level `role` в текущей реализации не участвует в security-check.
