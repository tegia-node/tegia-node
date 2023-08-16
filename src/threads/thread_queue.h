#ifndef _TEGIA_THREAD_QUEUE_H_
#define _TEGIA_THREAD_QUEUE_H_
// --------------------------------------------------------------------


#include <functional>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>

#include <tegia/core/core.h>
#include <tegia/const.h>
#include <tegia/context/thread_context.h>

namespace tegia {
namespace threads {

extern thread_local tegia::threads::data * const thread_data;
class worker;


struct task
{
	std::string uuid;
	std::function<void()>  _fn;
	context_ptr _context;

	~task()
	{
		// std::cout << "delete task" << std::endl;
	};
};


class queue
{
	friend class tegia::threads::worker;

	protected:
		// Очереди задач на исполнение
		std::queue<task*>		hight_fqueue;
		std::queue<task*>		medium_fqueue;
		std::queue<task*>		low_fqueue;

		// Условная переменная, которая задает ожидание задачи на исполнение
		std::condition_variable cv;

		// мьютекс
		std::mutex				mutex;

	public:

		queue()
		{  	};

		~queue()
		{  	};


		int add_task(tegia::threads::task * _task, int priority = _PHIGHT_)
		{  
			std::unique_lock<std::mutex> locker(this->mutex);
			switch(priority)
			{
				case _PHIGHT_:	this->hight_fqueue.push(_task); break;
				case _PMEDIUM_: this->medium_fqueue.push(_task); break;
				case _PLOW_: this->low_fqueue.push(_task); break;
			}
			this->cv.notify_one();
			return 0;
		};


		tegia::threads::task * get_task()
		{
			tegia::threads::task * _task = nullptr;
				
			if(!this->hight_fqueue.empty())
			{
				_task = this->hight_fqueue.front();
				this->hight_fqueue.pop();
				return _task;
			}

			if(!this->medium_fqueue.empty())
			{
				_task = this->medium_fqueue.front();
				this->medium_fqueue.pop();
				return _task;
			}

			if(!this->low_fqueue.empty())
			{
				_task = this->low_fqueue.front();
				this->low_fqueue.pop();
				return _task;
			}

			return _task;
		};

		void notify_all()
		{
			this->cv.notify_all();
		};

}; // class queue

}  // namespace threads
}  // namespace tegia



// --------------------------------------------------------------------
#endif