#ifndef H_TEGIA_THREAD_QUEUE
#define H_TEGIA_THREAD_QUEUE
// --------------------------------------------------------------------


#include <functional>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <bitset>
#include <strings.h>

using namespace std::chrono_literals;




namespace tegia {
namespace threads {

// Приоритет выполнения обработчика  
   // #define _PHIGHT_     0 
   // #define _PMEDIUM_    1 
   // #define _PLOW_       2 

class worker;
class worker_t;

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
	friend class tegia::threads::worker_t;

	protected:

		std::vector<std::queue<task*>> tasks 
		{
			{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},
			{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},
			{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},
			{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{}
		};

		std::bitset<64> tasks_bitset {0};

		
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
		{	};

		~queue()
		{  	};



		int add(tegia::threads::task * _task, int priority = 0)
		{  
			if(priority > 63 || priority < 0)
			{
				// TODO: GENERATE ERROR
				return 0;
			}

			std::unique_lock<std::mutex> locker(this->mutex);
			
			this->tasks[priority].push(_task);
			this->tasks_bitset.set(priority);

			this->cv.notify_one();
			return 0;
		};


		tegia::threads::task * get()
		{
			tegia::threads::task * _task = nullptr;

			//
			// TODO: Возможно, тут нужен мьютекс
			// this->mutex.lock();
			//

			long long int x = this->tasks_bitset.to_ullong();
			int s = ffsll(x);

			if(s == 0)
			{
				// std::cout << "not found tasks" << std::endl;
				// exit(0);
				return _task;
			}

			_task = this->tasks[s-1].front();
			this->tasks[s-1].pop();

			if(this->tasks[s-1].size() == 0)
			{
				this->tasks_bitset.reset(s-1);
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