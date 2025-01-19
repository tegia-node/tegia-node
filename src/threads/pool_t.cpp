
#include "pool_t.h" 

namespace tegia {
namespace threads {


//////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
//////////////////////////////////////////////////////////////////////////////////////////////////


void pool_t::run(int count)
{
	this->init_workers = 0;
	this->order_workers = count;
	this->_workers.reserve(count);
	this->_queue = new tegia::threads::queue();
	auto _fn = std::bind(&tegia::threads::pool_t::signal,this,"tid");

	for(int i = 0; i < count; ++i)
	{
		auto worker = std::make_shared<tegia::threads::worker_t>(this->_queue);
		worker->_signal_fn = _fn;
		_workers.push_back(worker);
		tegia::threads::run(std::bind(&tegia::threads::worker_t::thread_fn,worker));
	}
};


//////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
//////////////////////////////////////////////////////////////////////////////////////////////////


void pool_t::signal(const std::string &tid)
{
	this->signal_mutex.lock();
	this->init_workers++;
	if(this->init_workers == this->order_workers)
	{
		this->_callback();
	}
	this->signal_mutex.unlock();
};


//////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
//////////////////////////////////////////////////////////////////////////////////////////////////


int pool_t::add_task(std::function<void(void)> _fn, int priority)
{
	auto _task = new tegia::threads::task(tegia::random::uuid());
	_task->fn = _fn;
	this->_queue->add(_task, priority);
	return 0;
};


} // END namespace threads
} // END namespace tegia

