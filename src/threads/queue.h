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
#include <optional>
#include <strings.h>
#include <utility>
#include <vector>

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
	std::function<void()> fn;

	task() = default;
	~task() = default;

	task(const task &) = delete;
	task & operator=(const task &) = delete;
	task(task &&) = default;
	task & operator=(task &&) = default;
};


class queue
{
	friend class tegia::threads::worker;
	friend class tegia::threads::worker_t;

	protected:

		std::vector<std::queue<task>> tasks;

		std::bitset<64> tasks_bitset {0};

		// Условная переменная, которая задает ожидание задачи на исполнение
		std::condition_variable cv;

		// мьютекс
		std::mutex				mutex;

		// Проверяет пустоту очереди при уже захваченном queue::mutex.
		bool empty_locked() const
		{
			return this->tasks_bitset.none();
		};

	public:

		queue()
			: tasks(64)
		{	};

		~queue()
		{  	};



		int add(tegia::threads::task && _task, int priority = 0)
		{  
			if(priority > 63 || priority < 0)
			{
				// TODO: GENERATE ERROR
				return 400;
			}

			std::unique_lock<std::mutex> locker(this->mutex);
			
			this->tasks[priority].push(std::move(_task));
			this->tasks_bitset.set(priority);

			this->cv.notify_one();
			return 0;
		};


		std::optional<tegia::threads::task> get()
		{
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
				return std::nullopt;
			}

			tegia::threads::task _task = std::move(this->tasks[s-1].front());
			this->tasks[s-1].pop();

			if(this->tasks[s-1].size() == 0)
			{
				this->tasks_bitset.reset(s-1);
			}

			return std::optional<tegia::threads::task>(std::move(_task));
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
