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

	{
		std::lock_guard<std::mutex> lock(this->_mutex);

		if(this->_queue.size() >= this->_max_queue_size)
		{
			return actor_mailbox_t::QUEUE_OVERFLOW;
		}

		this->_active_messages.fetch_add(1);
		this->_queue.push_back(std::move(item));
	}

	return this->dispatch_available();
}

bool actor_mailbox_t::idle() const
{
	std::lock_guard<std::mutex> lock(this->_mutex);

	return this->_active_messages.load() == 0 &&
		this->_queue.empty() == true &&
		this->_running_messages == 0;
}

std::size_t actor_mailbox_t::active() const
{
	return this->_active_messages.load();
}

std::size_t actor_mailbox_t::queued() const
{
	std::lock_guard<std::mutex> lock(this->_mutex);
	return this->_queue.size();
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

int actor_mailbox_t::dispatch_available()
{
	while(true)
	{
		actor_mailbox_item_t next;

		{
			std::lock_guard<std::mutex> lock(this->_mutex);

			if(this->_queue.empty() == true)
			{
				return actor_mailbox_t::OK;
			}

			if(this->_running_messages >= this->_max_inflight)
			{
				return actor_mailbox_t::OK;
			}

			next = std::move(this->_queue.front());
			this->_queue.pop_front();
			this->_running_messages++;
		}

		int code = this->dispatch(next);
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
