#ifndef H_TEGIA_ACTORS_ACTOR_ENTRY
#define H_TEGIA_ACTORS_ACTOR_ENTRY

#include <cstddef>
#include <memory>
#include <utility>

#include "actor_mailbox.h"

namespace tegia::actors
{

class actor_t;
class type_base_t;

class actor_entry_t
{
	public:
		// Создает runtime-запись actor instance без mailbox.
		// type хранит метаданные actor type для быстрого поиска action.
		explicit actor_entry_t(actor_t * actor, type_base_t * type = nullptr)
			: _actor(actor),
			  _type(type)
		{ }

		// Создает runtime-запись actor instance с mailbox.
		// Используется map_t для dispatch сообщений через mailbox actor instance.
		actor_entry_t(
			actor_t * actor,
			type_base_t * type,
			actor_mailbox_t::mode_t mode,
				tegia::threads::pool_t * pool,
				actor_mailbox_t::dispatch_fn_t dispatch_fn,
				std::size_t max_inflight = 1,
				std::size_t max_queue_size = 50000)
			: _actor(actor),
			  _type(type),
			  _mailbox(std::make_unique<actor_mailbox_t>(
				  mode,
				  pool,
				  std::move(dispatch_fn),
				  max_inflight,
				  max_queue_size))
		{ }

		actor_entry_t() = delete;
		actor_entry_t(const actor_entry_t &) = delete;
		actor_entry_t & operator=(const actor_entry_t &) = delete;
		actor_entry_t(actor_entry_t &&) noexcept = default;
		actor_entry_t & operator=(actor_entry_t &&) noexcept = default;

		~actor_entry_t() = default;

		// Возвращает actor instance, связанный с этой runtime-записью.
		actor_t * actor() const
		{
			return this->_actor;
		}

		// Возвращает mailbox actor instance.
		actor_mailbox_t * mailbox()
		{
			return this->_mailbox.get();
		}

		// Возвращает mailbox actor instance для read-only проверок.
		const actor_mailbox_t * mailbox() const
		{
			return this->_mailbox.get();
		}

		// Проверяет, что actor pointer задан.
		bool valid() const
		{
			return this->_actor != nullptr;
		}

		// Проверяет, можно ли безопасно рассматривать actor entry как свободную от работы.
		// Для будущего unload этого недостаточно само по себе: map_t должен также синхронизировать доступ к actor registry.
		bool idle() const
		{
			return this->_mailbox == nullptr || this->_mailbox->idle();
		}

		// Экземпляр актора.
		// map_t продолжает работать с актором напрямую через _actor.
		actor_t * _actor = nullptr;

		// Метаданные actor type.
		// Нужны для быстрого поиска action без сборки строкового ключа type + action на каждое сообщение.
		type_base_t * _type = nullptr;

		// Mailbox этого actor instance.
		// Может быть nullptr только для промежуточных или тестовых runtime-записей без mailbox.
		std::unique_ptr<actor_mailbox_t> _mailbox;
};

} // namespace tegia::actors

#endif
