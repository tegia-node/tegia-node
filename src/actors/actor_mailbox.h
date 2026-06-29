#ifndef H_TEGIA_ACTORS_ACTOR_MAILBOX
#define H_TEGIA_ACTORS_ACTOR_MAILBOX

#include <atomic>
#include <chrono>
#include <cstddef>
#include <functional>
#include <memory>
#include <mutex>
#include <deque>

class message_t;

namespace tegia
{
	class user;
}

namespace tegia::threads
{
	class pool_t;
}

namespace tegia::actors
{

class actor_t;
struct action_t;

struct actor_mailbox_item_t
{
	// Экземпляр актора, которому адресовано сообщение.
	// Нужен dispatch-функции, чтобы вызвать action на правильном actor instance.
	actor_t * actor = nullptr;

	// Метаданные action, найденные в actor map.
	// Содержат указатель на метод актора, роли, validator и имя action.
	action_t * action = nullptr;

	// Сообщение, которое должно быть передано в action.
	// Хранится как shared_ptr, потому что одно сообщение может проходить callback chain.
	std::shared_ptr<message_t> message;

	// Пользовательский контекст, захваченный в момент отправки сообщения.
	// Worker thread должен восстановить этот контекст перед вызовом action.
	std::shared_ptr<tegia::user> user;

	// Приоритет постановки задачи в общий pool_t.
	// FIFO-порядок mailbox сохраняется до передачи item в pool, а priority применяется уже к задаче pool_t.
	int priority = 0;

	// Время попадания сообщения в mailbox.
	// Используется для будущих метрик ожидания в очереди и диагностики задержек dispatch.
	std::chrono::steady_clock::time_point enqueued_at = std::chrono::steady_clock::now();
};

class actor_mailbox_t
{
	public:
		enum class mode_t
		{
			// Stateless-режим.
			// Mailbox может держать несколько item, одновременно переданных в pool_t.
			parallel,

			// Stateful-режим.
			// Mailbox принудительно ограничивает число одновременно переданных в pool_t item значением 1.
			serial
		};

		// Универсальная функция выполнения mailbox item.
		// Runtime передает сюда actor/action/message/user, а конкретная реализация вызывает текущую action_func.
		// Функция возвращает код выполнения и не должна использовать исключения.
		using dispatch_fn_t = std::function<int(const actor_mailbox_item_t &)>;

		// Сообщение принято mailbox или задача успешно поставлена в pool_t.
		static constexpr int OK = 0;

		// Очередь mailbox заполнена, сообщение не принято к выполнению.
		static constexpr int QUEUE_OVERFLOW = 429;

		// Mailbox не сконфигурирован: нет pool_t или dispatch-функции.
		static constexpr int NOT_READY = 500;

		// Mailbox нельзя создавать пустым: для корректной работы сразу нужны pool_t и dispatch-функция.
		actor_mailbox_t() = delete;

		// Создает сразу готовый mailbox.
		// mode задает семантику actor instance, pool принимает задачи на выполнение,
		// dispatch_fn выполняет конкретный action, max_inflight ограничивает число item в pool_t,
		// max_queue_size ограничивает число ожидающих item внутри mailbox.
		actor_mailbox_t(
			mode_t mode,
			tegia::threads::pool_t * pool,
			dispatch_fn_t dispatch_fn,
			std::size_t max_inflight = 1,
			std::size_t max_queue_size = 1024);

		// Принимает сообщение в mailbox.
		// Метод всегда сначала кладет item во внутреннюю FIFO-очередь, затем пытается передать
		// в pool_t столько item, сколько разрешено лимитом max_inflight.
		// Для stateful actor max_inflight равен 1, для stateless actor должен равняться числу worker threads pool_t.
		// priority жестко нормализуется в диапазон [0, 63].
		int enqueue(actor_mailbox_item_t item);

		// Возвращает true, если mailbox полностью свободен.
		// Это означает: нет item в pool_t, внутренняя очередь пуста и счетчик active равен нулю.
		bool idle() const;

		// Возвращает количество сообщений actor instance, которые уже приняты mailbox.
		// В счет входят item, ожидающие в очереди, и item, уже переданные в pool_t.
		std::size_t active() const;

		// Возвращает количество сообщений во внутренней FIFO-очереди mailbox.
		// Это item, которые еще не были переданы в pool_t.
		std::size_t queued() const;

		// Возвращает количество item, уже переданных в pool_t и еще не завершенных.
		// Для serial mailbox значение должно быть 0 или 1.
		std::size_t running() const;

		// Возвращает текущую семантику mailbox: parallel или serial.
		mode_t mode() const;

		// Возвращает лимит одновременно выполняемых item.
		// Для serial mailbox всегда возвращает 1, даже если при настройке было передано большее значение.
		std::size_t max_inflight() const;

		// Возвращает лимит внутренней FIFO-очереди mailbox.
		// Лимит применяется к item, которые ожидают передачи в pool_t.
		std::size_t max_queue_size() const;

	private:
		// Нормализует лимит одновременно выполняемых item.
		// Для serial режима всегда возвращает 1; для parallel режима значение 0 заменяется на 1.
		static std::size_t normalize_max_inflight(mode_t mode, std::size_t max_inflight);

		// Нормализует лимит очереди.
		// Значение 0 запрещено и приводится к минимальному рабочему значению 1.
		static std::size_t normalize_max_queue_size(std::size_t max_queue_size);

		// Нормализует priority под текущий контракт pool_t.
		// Значение меньше 0 становится 0, значение больше 63 становится 63.
		static int normalize_priority(int priority);

		// Проверяет, можно ли принимать сообщения.
		// Mailbox готов, если задан общий pool_t и dispatch-функция.
		bool ready() const;

		// Пытается передать item из FIFO-очереди в pool_t до достижения max_inflight.
		// Метод не выполняет action сам: он только создает задачи в pool_t.
		int dispatch_available();

		// Создает задачу pool_t для одного item.
		// Перед вызовом этого метода running-счетчик уже увеличен под mutex.
		int dispatch(actor_mailbox_item_t item);

		// Выполняет одно сообщение mailbox внутри worker thread.
		// После dispatch всегда вызывает complete(); ошибки передаются кодом возврата dispatch-функции.
		void run(actor_mailbox_item_t item);

		// Завершает обработку одного item.
		// Уменьшает active/running-счетчики и пытается передать в pool_t следующий item из очереди.
		void complete();

		// Откатывает учет item, который был взят из очереди, но не был поставлен в pool_t.
		// Item не возвращается в очередь: ошибка эскалируется вызывающему коду, а решение о retry/drop принимается выше.
		void rollback_dispatch();

		// Семантика текущего mailbox.
		// parallel используется для stateless actor, serial — для stateful actor.
		mode_t _mode = mode_t::serial;

		// Общий пул задач runtime.
		// Mailbox сам не создает потоки, а только передает готовые задачи в этот pool.
		tegia::threads::pool_t * _pool = nullptr;

		// Функция, которая реально выполняет mailbox item.
		// В текущей архитектуре должна оборачивать вызов map_t::action_func или его будущий аналог.
		dispatch_fn_t _dispatch_fn;

		// Количество сообщений, принятых mailbox и еще не полностью завершенных.
		// Включает item в очереди и item, уже переданные в pool_t.
		std::atomic<std::size_t> _active_messages{0};

		// Защищает состояние mailbox: очередь, running-счетчик и лимиты.
		// Также используется в const-методах чтения состояния.
		mutable std::mutex _mutex;

		// FIFO-очередь ожидающих сообщений.
		// Очередь используется и для stateful, и для stateless actor; различается только max_inflight.
		std::deque<actor_mailbox_item_t> _queue;

		// Количество item, уже переданных в pool_t и еще не завершенных.
		// Для stateful actor лимит равен 1, для stateless actor лимит равен числу worker threads pool_t.
		std::size_t _running_messages = 0;

		// Максимальное количество item этого actor instance, которые mailbox может одновременно передать в pool_t.
		// Для serial mailbox значение нормализуется до 1.
		std::size_t _max_inflight = 1;

		// Максимальный размер FIFO-очереди mailbox.
		// При превышении enqueue(...) возвращает QUEUE_OVERFLOW и не принимает сообщение.
		std::size_t _max_queue_size = 1024;
};

} // namespace tegia::actors

#endif
