#ifndef H_TEGIA_THREAD_QUEUE
#define H_TEGIA_THREAD_QUEUE
// --------------------------------------------------------------------


#include <functional>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>


namespace tegia {
namespace threads {

// Приоритет выполнения обработчика  
   #define _PHIGHT_     0 
   #define _PMEDIUM_    1 
   #define _PLOW_       2 

class worker;


struct task
{
	std::string uuid;
	std::function<void()> fn;

	task(const std::string &_uuid): uuid(_uuid)
	{
		//std::cout << "create task " << this->uuid << std::endl;
	};

	~task()
	{
		//std::cout << "delete task " << this->uuid << std::endl;
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


		int add(tegia::threads::task * _task, int priority = _PHIGHT_)
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


		tegia::threads::task * get()
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