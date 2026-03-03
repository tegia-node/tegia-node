# Проблема

Сейчас при указания action для актора используется макрос `ADD_ACTION`.

```cpp
ADD_ACTION(
    "/clients/count",
    &PDP::WS::CLIENTS::clients_count,
    ROLES::WS::MEMBER
);
```

Если при этом такой актор является workspace, то дополнительно нужно задать route для action.

```cpp
this->_router->add("POST", "/clients/count",
    nlohmann::json::parse(R"({
        "actor": "{/this}",
        "action":"/clients/count",
        "mapping":
        {
            "": "/post/data"
        }
    })"));
```

Это делается в конструкторе актора.

Это приводит к двум проблемам:
1. Нужно дважды писать код: отдельно `ADD_ACTION`, отдельно `router->add(...)`.
2. `_router` сейчас живет на уровне экземпляра `ws_t`, а `ADD_ACTION` регистрирует action на уровне типа актора.

---

# Анализ по текущему коду

1. `ADD_ACTION` добавляет action в `type_base_t::fmap` (уровень типа, единоразово при `_init_type`).
2. Роутер (`tegia::app::router_t`) сейчас создается в `ws_t` конструкторе (`new router_t(this->name)`) и заполняется per-instance.
3. В `ws_t::router(...)` match делается по `_router->match(...)`, далее в callback кладется `actor/action` из route.
4. В `router_t` единственная instance-специфика для маршрута — подстановка `{/this}` через `ws_name`.

Вывод:
- Декларация маршрутов и декларация action разведены по разным уровням (type vs instance).
- Для устранения дублирования нужно поднимать декларацию route на уровень типа актора.

---

# Предложение по решению

## Целевой принцип

Для `ws`-акторов action и HTTP-route должны объявляться в одном месте на уровне типа актора, а не в конструкторе экземпляра.

## Рекомендуемая реализация (вариант v1)

### 1) Добавить route registry в `type_base_t`

Расширить `type_base_t` структурой маршрута, например:

```cpp
struct route_t {
    std::string method;
    std::string pattern;
    nlohmann::json data; // actor/action/mapping/role
};

std::vector<route_t> routes;
int add_route(const std::string &method, const std::string &pattern, nlohmann::json data);
```

### 2) Ввести единый макрос для ws-action + route

```cpp
#define ADD_WS_ACTION_ROUTE(method, pattern, action, func, mapping_json, ...) \
    do { \
        type->add_action(action, "", static_cast<tegia::actors::action_fn_ptr>(func), tegia::user::roles(__VA_ARGS__)); \
        nlohmann::json __r = { \
            {"actor", "{/this}"}, \
            {"action", action}, \
            {"mapping", mapping_json} \
        }; \
        type->add_route(method, pattern, __r); \
    } while(0)
```

Идея: одна декларация регистрирует и action, и route.

### 3) Инициализация роутера из type metadata

В `ws_t` конструкторе не описывать маршруты вручную.

Вместо этого:
- получить описание routes текущего `type`;
- заполнить `_router` из этого списка.

Это сохраняет текущий `router_t` и минимизирует объем изменений.

### 4) Поддержать базовые системные ws routes отдельно

`/member/add`, `/member/accept`, `/member/remove`, `/init`, `/router` остаются в базовом `type_t<ws_t>` (как сейчас actions), и туда же добавляются соответствующие routes через новый API.

### 5) Совместимость

На переходный период оставить возможность ручного `this->_router->add(...)` в конструкторе, но пометить как deprecated путь.

---

# Почему этот вариант

1. Убирает дублирование: action + route задаются в одном месте.
2. Приводит уровни абстракции в соответствие: и action, и route живут на уровне типа.
3. Минимально ломает текущий runtime: `router_t::match` и callback-пайплайн не нужно переписывать.
4. Позволяет постепенно перейти к общему (shared/immutable) route table для всех экземпляров типа.

---

# Следующий шаг (технический)

1. Добавить `route_t` и `add_route` в `type_base_t`.
2. Добавить макрос `ADD_WS_ACTION_ROUTE`.
3. Подключить сборку routes в `ws_t` из metadata типа.
4. Перенести существующие ws маршруты (`member/add` и т.д.) в декларативный путь.
5. Добавить runtime-проверку на конфликт route/action (дубликаты).

---

# Вопросы для уточнения перед реализацией

1. Нужно ли жесткое правило: "для ws action обязательно должен быть route"? Или допускаются internal-only actions без HTTP route?

Ответ: допускаются и такие, и такие action

2. Какой источник истины для roles в route-доступе:
   - только `ADD_ACTION` roles;
   - или отдельное поле `role` в route тоже должно работать?

Ответ: сейчас поле role в route никак не используется. Источником истины должен быть roles в ADD_ACTION

3. Разрешаем ли несколько routes на один action (alias paths / versioning)?

Ответ: нет

4. Должны ли route-и поддерживать методы кроме `POST` (GET/PUT/DELETE) в базовом контракте?

Ответ: сейчас только POST

5. Нужна ли валидация `mapping` schema при старте типа (fail-fast)?

Ответ: Да, нужна

6. Должны ли дубли route считаться фатальной ошибкой загрузки типа?

Ответ: Да, должнв

7. Оставляем ли `{/this}` как обязательный паттерн для ws, или вводим более явный placeholder (например `${ws}`)?

Ответ: А чем плох текущий?

8. Нужна ли возможность объявлять route в JSON-файле конфигурации типа (не в C++ коде)?

Ответ: Это не имеет смысла, потому что все равно в коде нужна жесткая привязка к конкретным функциям класса. Или это можно унверсализировать?

---

# Пример нового описания через `ADD_WS_ACTION_ROUTE`

Старый вариант (два места):

```cpp
ADD_ACTION(
    "/clients/count",
    &PDP::WS::CLIENTS::clients_count,
    ROLES::WS::MEMBER
);

this->_router->add("POST", "/clients/count",
    nlohmann::json::parse(R"({
        "actor": "{/this}",
        "action":"/clients/count",
        "mapping":
        {
            "": "/post/data"
        }
    })"));
```

Новый вариант (одно место):

```cpp
ADD_WS_ACTION_ROUTE(
    "POST",
    "/clients/count",
    "{/this}",
    "/clients/count",
    &PDP::WS::CLIENTS::clients_count,
    nlohmann::json::parse(R"({
        "": "/post/data"
    })"),
    ROLES::WS::MEMBER
);
```

Пример, когда целевой actor не `{/this}`:

```cpp
ADD_WS_ACTION_ROUTE(
    "POST",
    "/clients/count",
    "stats/global",
    "/clients/count",
    &PDP::WS::CLIENTS::clients_count,
    nlohmann::json::parse(R"({
        "": "/post/data"
    })"),
    ROLES::WS::MEMBER
);
```
