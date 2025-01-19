#ifndef H_TEGIA_THREADS_WORKERS_POOL
#define H_TEGIA_THREADS_WORKERS_POOL

#include <mutex>
#include <thread>

#include <tegia/core.h>

#include "queue.h" 
#include "worker_t.h" 


//////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
//////////////////////////////////////////////////////////////////////////////////////////////////


namespace tegia {
namespace threads {

class pool_t
{
	public:
		pool_t() = default;
		~pool_t() = default;

		int add_task(std::function<void(void)> _fn, int priority);
		void signal(const std::string &tid);
		void run(int count);

		std::function<void()> _callback;

	private:	
		std::vector<std::shared_ptr<tegia::threads::worker_t>> _workers;
		int init_workers = 0;
		int order_workers = 0;

		tegia::threads::queue * _queue;
		std::mutex signal_mutex;
		int threads_count = 0;
};


} // END namespace threads
} // END namespace tegia

#endif