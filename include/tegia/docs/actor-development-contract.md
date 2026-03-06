# Контракт разработки акторов

Документ фиксирует минимальный рабочий контракт для разработки новых actor types в `*-conf` репозиториях.

## 1. Базовая модель

- Actor type регистрируется из shared library (`*.so`).
- Runtime загружает библиотеку и ищет символ `_init_type`.
- `_init_type` возвращает `tegia::actors::type_base_t*` с зарегистрированными actions (и для `ws_t` также routes).

## 2. Обязательная точка входа библиотеки

```cpp
extern "C" tegia::actors::type_base_t * _init_type()
{
    auto type = new tegia::actors::type_t<MyActor>("MY::TYPE");

    ADD_ACTION("/ping", &MyActor::ping, ROLES::SESSION::PUBLIC);
    // или ADD_ACTION2(action, json_schema_file, func, roles...)

    return type;
}
```

Требования:
- имя символа строго `_init_type`;
- возвращаемый тип строго `tegia::actors::type_base_t *`;
- action-функции имеют сигнатуру `int(const std::shared_ptr<message_t> &)`.

## 3. Action API

Макросы:
- `ADD_ACTION(action, func, roles...)` - регистрация action без schema-файла;
- `ADD_ACTION2(action, filename, func, roles...)` - регистрация action с JSON validator.

Ограничения:
- `action` начинается с `/` (например `/create`, `/stats/get`);
- источником прав доступа является `roles` из `ADD_ACTION`/`ADD_ACTION2`.

## 4. Модель прав

При выполнении action runtime проверяет:
- роли пользователя из контекста (`user->_roles`);
- WS-роли участника (через `actor->roles(uuid)` для workspace-акторов);
- mask action (`action.roles`).

Практическое правило:
- доступ определяется только roles, заданными при регистрации action;
- route-level поле `role` в роутере сейчас не используется.

## 5. Рекомендованный минимальный шаблон актора

```cpp
class MyActor : public tegia::actors::actor_t
{
public:
    explicit MyActor(const std::string &name)
        : actor_t("MY::TYPE", name) {}

    int ping(const std::shared_ptr<message_t> &message)
    {
        message->data["ok"] = true;
        return 200;
    }
};
```

## 6. Важные runtime-особенности

- Многие ошибки в runtime fail-fast (`exit(0)`), поэтому ошибки контракта лучше ловить на этапе ревью и локального запуска.
- Подпись action и структура регистрации должны быть строгими: runtime не выполняет "мягкую" адаптацию.
