# Контракт сообщений

## 1. Базовый объект сообщения

`message_t` содержит:
- `data` - payload для action;
- `http` - HTTP-контекст (если сообщение пришло из HTTP ingress);
- `callback` - цепочка адресов для последующей маршрутизации.

## 2. Создание сообщения

```cpp
auto msg = tegia::message::init();
msg->data["id"] = "123";
```

или

```cpp
auto msg = tegia::message::init({
    {"id", "123"}
});
```

## 3. Отправка сообщения

```cpp
tegia::message::send("domain/actor-name", "/action", msg);
```

API:
- `send(name, action, json data, priority=0)`;
- `send(name, action, message, priority=0)`;
- `send(message, fn, priority=0)` для callback-style обработки.

## 4. Callback chain

После выполнения action runtime может продолжить маршрут:

```cpp
message->callback.add("target/actor", "/next/action");
```

Если callback задан, runtime вызывает следующий адрес автоматически.

## 5. Валидация payload

Если action зарегистрирован через `ADD_ACTION2(...)`, runtime валидирует `message->data` по JSON schema перед постановкой в worker pool.

При невалидном payload текущая реализация завершает выполнение процесса (fail-fast).

## 6. Практические рекомендации

- Держи `message->data` плоским и предсказуемым.
- Явно заполняй коды/статусы ответа в HTTP-сценариях через `message->http["response"]`.
- Не полагайся на неявные поля в `http`: проверяй наличие нужных ключей перед чтением.
