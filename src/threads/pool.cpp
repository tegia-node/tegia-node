
#include "pool.h"

/*
void init_context()
{
	std::cout << "init context" << std::endl;
	std::cout << "[constructor] tid = " << tegia::context::tid() << std::endl;
};
*/


namespace tegia {
namespace threads {

pool::pool()
{
	this->queue = new tegia::threads::queue();
};

pool::~pool()
{
	delete this->queue;
};


int pool::init(int threads_count, const nlohmann::json &db_config, std::function<void()> _callback)
{
	this->threads_count = threads_count;
	this->_callback = _callback;

	for(int i = 0; i < threads_count; i++)
	{
		auto pWorker = std::make_shared<tegia::threads::worker>(
			this->queue,												// очередь задач
			db_config, 													// конфиг подключения к БД
			std::bind(&pool::signal,this, std::placeholders::_1)		// фунция, сигнализирующая, то поток инициализирован
		);

		std::cout << _RED_TEXT_ << "tid = " << pWorker->tid << _BASE_TEXT_ << std::endl;

		this->_workers.emplace(pWorker->tid, pWorker);
	}

	return 0;
};


void pool::signal(const std::string &tid)
{
	this->signal_mutex.lock();
	this->_workers_status.insert({tid,true});
	if(this->_workers_status.size() == this->threads_count)
	{
		this->_callback();
	}
	this->signal_mutex.unlock();
	// return 0;
};


int pool::add_task(std::function<void()> _fn, int priority)
{
	auto _task = new tegia::threads::task(tegia::random::uuid());
	_task->fn = _fn;
	this->queue->add(_task, priority);
	return 0;
};


}  // namespace threads
}  // namespace tegia


