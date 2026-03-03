# Архитектура Tegia Node (актуализация после повторного анализа)

## 1. Назначение и границы

`tegia-node` — C++ runtime фреймворк для акторной модели и маршрутизации сообщений.

Что входит в этот репозиторий:
- runtime ядро (инициализация node, registry акторов, worker pool, message dispatch);
- загрузка actor-type плагинов через `dlopen` и `_init_type`;
- thread-local контекст выполнения (`thread_t`), user context, роли;
- инфраструктурные компоненты (`ws_t`, dictionaries, MySQL provider, JWT utility).

Что не входит:
- прикладная бизнес-логика;
- production HTTP ingress как часть ядра;
- оркестрация multi-node кластера (на март 2026 не реализована в runtime).

## 2. Архитектурные уровни и ownership

### 2.1 Иерархия поставки

Операционная модель (по ответам и коду):
1. Репозиторий кластера.
2. Репозиторий `tegia-node` (этот runtime).
3. Репозитории конфигураций акторов (`*-conf`).

Кластерный репозиторий является точкой сборки окружения:
- хранит `cluster`-конфиги и инсталляционные скрипты;
- подтягивает `tegia-node` и нужные конфигурации;
- задает конкретный состав акторных конфигураций для инстанса.

### 2.2 Конфигурации акторов

Конфигурация акторов — отдельный репозиторий с:
- описанием actor types;
- конфигами/параметрами;
- миграциями и прикладными ресурсами.

Подтвержденные системные конфигурации:
- `tegia-common/http-conf` (HTTP обработка);
- `tegia-common/tasks-conf` (пулы задач);
- `tegia-common/user-conf` (акторы пользователей);
- `tegia-id/id-conf` (регистрация/аутентификация).

## 3. Runtime поток выполнения (as-is)

Точка входа: `src/main.cpp`.

Шаги старта:
1. Проверка CLI (`--local` или `--config`).
2. Проверка наличия `./configs/cluster.json`.
3. Проверка наличия `./configs/configurations.json`.
4. Создание `tegia::node::node` и `node::run()`.

Шаги `node::run()` (`src/node/node.cpp`):
1. Инициализация logger/config/thread manager/actor map.
2. Загрузка включенных конфигураций из `cluster["configurations"]`.
3. Загрузка actor types (`path + *.so`) через `_init_type`.
4. Запуск `searchd` shell-командой.
5. Инициализация MySQL client (`tegia::mysql::init`).
6. Подъем worker pool (`pool->run(8)`) и callback на `node::action()`.

Шаги `node::action()`:
1. Инициализация dictionaries.
2. Регистрация доменов (жестко в коде, список статический).
3. Регистрация `patterns`.
4. Отправка init-сообщений из `messages`.

## 4. Модель сообщений и акторов

- `actors::map_t` хранит registry типов, инстансов и actions.
- `send_message` ищет/создает actor, валидирует payload (если есть schema), ставит action в очередь pool.
- Проверка доступа выполняется на action-уровне по объединению ролей:
  - user roles из `thread->_user`/message user;
  - WS-specific roles участника (`ws_t`).
- Callback chain в `message_t` поддерживает последовательную маршрутизацию после action.

## 5. Конфигурационная модель

Фактический источник включения конфигураций: `cluster.json` (`cluster["configurations"]`).

Для каждой активной конфигурации runtime читает `./configurations/<name>.json` и агрегирует:
- `db.connections`;
- `db.databases`;
- `patterns`;
- `init`-сообщения в общий `messages`.

Отдельно проверяемый в `main` файл `./configs/configurations.json` в текущей реализации не участвует в runtime-пайплайне загрузки (проверяется только факт наличия).

## 6. Текущее состояние кластера и целевое направление

Текущее состояние (март 2026):
- один процесс `tegia-node` на кластер;
- только local domains в реальном dispatch;
- remote domain задекларирован концептуально, но не реализован как рабочая маршрутизация.

Целевое направление:
- кластер из нескольких node;
- прозрачная для акторов межнодовая доставка по строковым именам;
- сохранение акторной модели без привязки к физическому размещению actor instance.

## 7. Архитектурные риски (подтверждено ответами и кодом)

1. Конкурентность актора не сериализована по instance.
2. В кодовой базе много `exit(0)` в runtime-критичных местах.
3. Нет graceful shutdown для pool/actors/mysql (кроме попытки остановки `searchd`).
4. Отсутствует backpressure стратегия (очереди растут неконтролируемо).
5. Модель доступа и эскалации ролей требует перепроектирования.
6. Отсутствуют SLO/SLI, observability метрики и интеграционные runtime-тесты.
7. Нет формального контракта `patterns`, init `messages` и ABI/lifecycle плагинов.

## 8. Статус ранее открытых вопросов

### 8.1 Закрыто

1. Ownership HTTP ingress и системных конфигураций определен (через `*-conf` репозитории).
2. Поставка и инсталляция привязаны к репозиториям кластеров.
3. Миграции БД принадлежат конфигурациям, не `tegia-node`.
4. Текущий формат логов и хранение (`/cluster/logs`) подтверждены.
5. Причина фиксированного `pool->run(8)` подтверждена как дефект реализации.

### 8.2 Частично закрыто

1. Версионирование actor libraries: концепт «канонического набора» не принят, но ownership через кластерные декларации есть.
2. Multi-node стратегия подтверждена целевой, но без runtime-контракта маршрутизации.
3. Domain list: желаемо вынести из `tegia-node`, но сейчас он хардкодится в `node::action()`.

### 8.3 Остается открытым

1. Контракты `patterns` и init `messages`.
2. Политика безопасности: role model, anti-escalation, key rotation, аудит.
3. Надежность: delivery semantics, error handling, shutdown lifecycle.
4. Конкурентность: actor isolation, queue synchronization, pool segmentation.
5. Контракты качества: тестирование, совместимость action/schema, ABI.

## 9. Новые вопросы для следующей итерации

1. Какой execution-contract для actor instance выбираем официально:
   - single-threaded mailbox per actor;
   - или parallel execution + обязательная thread-safe state model?

2. Что делаем с `configs/configurations.json`:
   - удаляем проверку из `main`;
   - или возвращаем его как источник truth (и синхронизируем с `cluster.json`)?

3. Какой минимальный shutdown protocol вводим в runtime v1:
   - stop intake;
   - drain queues с timeout;
   - закрытие actor lifecycle hooks;
   - корректный teardown DB/providers?

4. Где проходит граница безопасности:
   - централизованный policy engine в runtime;
   - или policy-as-code в конфигурациях с единым enforcement API?

5. Какой контракт remote messaging для multi-node:
   - discovery/addressing;
   - retries/timeouts;
   - delivery guarantee (at-most-once / at-least-once);
   - idempotency requirements к action?

6. Какие базовые SLI/SLO фиксируем для runtime:
   - queue depth;
   - dispatch latency p95/p99;
   - error rate;
   - saturation worker pool?

## 10. Рекомендуемый следующий пакет артефактов

1. ADR: `Actor Execution Model`.
2. ADR: `Message Delivery & Remote Routing`.
3. ADR: `Security Model v2`.
4. Runtime spec: `patterns/messages contract`.
5. Технический backlog на устранение hardcoded/unsafe участков (`run(8)`, `exit(0)`, shutdown).
