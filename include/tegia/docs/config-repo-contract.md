# Контракт конфигурационного репозитория (`*-conf`)

Документ фиксирует минимальный формат конфигов, который runtime реально использует.

## 1. `configs/cluster.json`

Ключевой блок:

```json
{
  "configurations": {
    "example-conf": true,
    "other-conf": false
  }
}
```

Смысл:
- ключ - имя конфигурации;
- `true` - загрузить `./configurations/<name>.json`;
- `false` - пропустить.

Дополнительно может использоваться:
- `/nodes/data/thread/count` - thread count (если задан);
- `/nodes/data/ws/route/conflict_policy` (`error` или default override-режим) для конфликтов legacy/type routes.

## 2. `configurations/<name>.json`

Минимальная рабочая структура:

```json
{
  "path": "/abs/or/relative/path/to/libs/",
  "types": {
    "MY::TYPE": "libmy_type.so"
  },
  "db": {
    "connections": {},
    "databases": {}
  },
  "patterns": {
    "my/*": "MY::TYPE"
  },
  "init": [
    {
      "actor": "my/bootstrap",
      "action": "/init",
      "data": {}
    }
  ]
}
```

Runtime-поведение:
- `path + types[*]` используется как путь к библиотеке типа;
- `init` агрегируется в общий список стартовых сообщений;
- `patterns` агрегируются в общую таблицу паттернов;
- `db.connections` и `db.databases` агрегируются между конфигами, конфликт по имени с разными значениями - фатальная ошибка.

## 3. Контракт `types`

Каждая библиотека должна экспортировать `_init_type`, который возвращает зарегистрированный `type_base_t*`.

## 4. Контракт `patterns`

- ключ - шаблон имени актора;
- значение - имя типа (`MY::TYPE`);
- при отправке сообщения runtime пытается разрешить actor name через patterns.

## 5. Контракт `init`

Каждый элемент:
- `actor` (string);
- `action` (string);
- `data` (json object/array/value).

На старте runtime отправляет эти сообщения после регистрации patterns.

## 6. Практическая проверка перед запуском

- все `types` указывают на существующие `.so`;
- в каждой `.so` есть `_init_type`;
- нет конфликтующих `db.connections`/`db.databases`;
- `patterns` покрывают ожидаемые actor names;
- `init` использует существующие actor/action.
