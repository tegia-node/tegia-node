
#include "thread_pool.h"

namespace tegia {
namespace threads {

pool* pool::_self = nullptr;
thread_local tegia::threads::data * const thread_data = new tegia::threads::data();

pool* pool::instance(int threads_count)
{
	if(!_self)
	{
		_self = new pool();
		_self->init(threads_count);
	}
	return _self;
};

pool::pool()
{
	this->_queue = new tegia::threads::queue();
};

pool::~pool()
{
	delete this->_queue;
};


int pool::init(int threads_count)
{
	this->threads_count = threads_count;
	for(int i = 0; i < threads_count; i++)
	{
		_worker_ptr pWorker(new tegia::threads::worker(this->_queue));
		this->_workers.emplace( pWorker->getTID(), pWorker);		
	}

	return 0;
};

int pool::add_task(std::function<void()> fn, int priority)
{
	thread_data->count++;
	// std::cout << "[" << core::cast<std::string>(std::this_thread::get_id()) << "] [" << thread_data->tid << "] " << thread_data->count << std::endl;

	auto _task = new tegia::threads::task();
	_task->_fn = fn;
	_task->uuid = tegia::random::uuid();
	_task->_context = thread_data->context;
	this->_queue->add_task(_task, priority);
	return 0;
};

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
tegia::threads::data * const pool::get_current()
{
	return thread_data;
};
#pragma GCC diagnostic pop


int pool::get_count()
{
	return this->threads_count;
};


}  // namespace threads
}  // namespace tegia


