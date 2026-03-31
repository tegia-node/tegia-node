# Quickstart: новый актор в `*-conf`

## 1. Реализуй actor class

- унаследуйся от `tegia::actors::actor_t` (или `tegia::actors::ws_t` для workspace);
- добавь action-методы с сигнатурой `int(const std::shared_ptr<message_t> &)`.

## 2. Зарегистрируй type в `_init_type`

- создай `type_t<Actor>("NAMESPACE::TYPE")`;
- зарегистрируй actions:
  - `ADD_ACTION` / `ADD_ACTION2`;
  - для `ws_t` используй `ADD_WS_ACTION_ROUTE`;
  - для внешнего `actor` используй `ADD_WS_ROUTE` (без локальной регистрации action).
  - для `ADD_WS_ACTION_ROUTE` обязательны ключи `actor`, `action`, `mapping` в `route_json`.

## 3. Собери shared library

- результат должен быть `*.so`;
- runtime должен иметь доступ к файлу по `path + filename` из конфигурации.

## 4. Подключи тип в `configurations/<name>.json`

- добавь `types["NAMESPACE::TYPE"] = "libname.so"`;
- при необходимости добавь `patterns`.

## 5. Добавь init-сообщение (опционально)

Если нужен bootstrap:

```json
{
  "actor": "my/bootstrap",
  "action": "/init",
  "data": {}
}
```

## 6. Включи конфигурацию в `cluster.json`

```json
{
  "configurations": {
    "<name>": true
  }
}
```

## 7. Минимальный smoke-check

- runtime загружает тип без ошибок `dlopen`/`_init_type`;
- action вызывается через `tegia::message::send(...)`;
- для WS: route резолвится и кладет ожидаемые поля в `message->data`;
- права доступа дают ожидаемый `200`/`403`.
