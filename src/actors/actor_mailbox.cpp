#include "actor_mailbox.h"

#include <utility>

#include "../threads/pool_t.h"

namespace tegia::actors
{

actor_mailbox_t::actor_mailbox_t(
	mode_t mode,
	tegia::threads::pool_t * pool,
	dispatch_fn_t dispatch_fn,
	std::size_t max_inflight,
	std::size_t max_queue_size)
{
	this->_mode = mode;
	this->_pool = pool;
	this->_dispatch_fn = std::move(dispatch_fn);
	this->_max_inflight = actor_mailbox_t::normalize_max_inflight(mode, max_inflight);
	this->_max_queue_size = actor_mailbox_t::normalize_max_queue_size(max_queue_size);
}

int actor_mailbox_t::enqueue(actor_mailbox_item_t item)
{
	if(this->ready() == false)
	{
		return actor_mailbox_t::NOT_READY;
	}

	item.enqueued_at = std::chrono::steady_clock::now();
	item.priority = actor_mailbox_t::normalize_priority(item.priority);

	bool dispatch_now = false;

	{
		std::lock_guard<std::mutex> lock(this->_mutex);

		if(this->queue_empty_locked() == true && this->_running_messages < this->_max_inflight)
		{
			this->_active_messages.fetch_add(1);
			this->_running_messages++;
			dispatch_now = true;
		}
		else
		{
			if(this->queue_size_locked() >= this->_max_queue_size)
			{
				return actor_mailbox_t::QUEUE_OVERFLOW;
			}

			this->_active_messages.fetch_add(1);
			this->queue_push_locked(std::move(item));
		}
	}

	if(dispatch_now == true)
	{
		int code = this->dispatch(std::move(item));
		if(code != actor_mailbox_t::OK)
		{
			this->rollback_dispatch();
		}

		return code;
	}

	return this->dispatch_available();
}

bool actor_mailbox_t::idle() const
{
	std::lock_guard<std::mutex> lock(this->_mutex);

	return this->_active_messages.load() == 0 &&
		this->queue_empty_locked() == true &&
		this->_running_messages == 0;
}

std::size_t actor_mailbox_t::active() const
{
	return this->_active_messages.load();
}

std::size_t actor_mailbox_t::queued() const
{
	std::lock_guard<std::mutex> lock(this->_mutex);
	return this->queue_size_locked();
}

std::size_t actor_mailbox_t::running() const
{
	std::lock_guard<std::mutex> lock(this->_mutex);
	return this->_running_messages;
}

actor_mailbox_t::mode_t actor_mailbox_t::mode() const
{
	return this->_mode;
}

std::size_t actor_mailbox_t::max_inflight() const
{
	std::lock_guard<std::mutex> lock(this->_mutex);
	return this->_max_inflight;
}

std::size_t actor_mailbox_t::max_queue_size() const
{
	std::lock_guard<std::mutex> lock(this->_mutex);
	return this->_max_queue_size;
}

std::size_t actor_mailbox_t::normalize_max_inflight(mode_t mode, std::size_t max_inflight)
{
	if(mode == mode_t::serial)
	{
		return 1;
	}

	if(max_inflight == 0)
	{
		return 1;
	}

	return max_inflight;
}

std::size_t actor_mailbox_t::normalize_max_queue_size(std::size_t max_queue_size)
{
	if(max_queue_size == 0)
	{
		return 1;
	}

	return max_queue_size;
}

int actor_mailbox_t::normalize_priority(int priority)
{
	if(priority < 0)
	{
		return 0;
	}

	if(priority > 63)
	{
		return 63;
	}

	return priority;
}

bool actor_mailbox_t::ready() const
{
	return this->_pool != nullptr && static_cast<bool>(this->_dispatch_fn) == true;
}

bool actor_mailbox_t::queue_empty_locked() const
{
	return this->_queued_messages == 0;
}

std::size_t actor_mailbox_t::queue_size_locked() const
{
	return this->_queued_messages;
}

void actor_mailbox_t::queue_push_locked(actor_mailbox_item_t item)
{
	std::size_t priority = static_cast<std::size_t>(item.priority);
	this->_priority_queues[priority].push_back(std::move(item));
	this->_priority_mask.set(priority);
	this->_queued_messages++;
}

actor_mailbox_item_t actor_mailbox_t::queue_pop_locked()
{
	for(std::size_t priority = 0; priority < this->_priority_queues.size(); ++priority)
	{
		if(this->_priority_mask.test(priority) == false)
		{
			continue;
		}

		auto item = std::move(this->_priority_queues[priority].front());
		this->_priority_queues[priority].pop_front();
		this->_queued_messages--;

		if(this->_priority_queues[priority].empty() == true)
		{
			this->_priority_mask.reset(priority);
		}

		return item;
	}

	return {};
}

int actor_mailbox_t::dispatch_available()
{
	while(true)
	{
		actor_mailbox_item_t next;

		{
			std::lock_guard<std::mutex> lock(this->_mutex);

			if(this->queue_empty_locked() == true)
			{
				return actor_mailbox_t::OK;
			}

			if(this->_running_messages >= this->_max_inflight)
			{
				return actor_mailbox_t::OK;
			}

			next = this->queue_pop_locked();
			this->_running_messages++;
		}

		int code = this->dispatch(std::move(next));
		if(code != actor_mailbox_t::OK)
		{
			this->rollback_dispatch();
			return code;
		}
	}
}

int actor_mailbox_t::dispatch(actor_mailbox_item_t item)
{
	int priority = item.priority;

	return this->_pool->add_task(
		[this, item = std::move(item)]() mutable
		{
			this->run(std::move(item));
		},
		priority
	);
}

void actor_mailbox_t::run(actor_mailbox_item_t item)
{
	this->_dispatch_fn(item);
	this->complete();
}

void actor_mailbox_t::complete()
{
	this->_active_messages.fetch_sub(1);

	{
		std::lock_guard<std::mutex> lock(this->_mutex);

		if(this->_running_messages > 0)
		{
			this->_running_messages--;
		}
	}

	this->dispatch_available();
}

void actor_mailbox_t::rollback_dispatch()
{
	this->_active_messages.fetch_sub(1);

	std::lock_guard<std::mutex> lock(this->_mutex);

	if(this->_running_messages > 0)
	{
		this->_running_messages--;
	}
}

} // namespace tegia::actors
