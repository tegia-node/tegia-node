#ifndef H_TEGIA_THREADS_WORKERS
#define H_TEGIA_THREADS_WORKERS

#include <mutex>
#include <thread>
#include <iostream>

#include "queue.h" 

//////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
//////////////////////////////////////////////////////////////////////////////////////////////////


namespace tegia {
namespace threads {

class worker_t
{
	public:
		worker_t(tegia::threads::queue * _queue);
		~worker_t() = default;

		void thread_fn();

		std::function<void()> _signal_fn;

	private:
		bool is_work = true;
		tegia::threads::queue * _queue;
};


} // END namespace threads
} // END namespace tegia

#endif