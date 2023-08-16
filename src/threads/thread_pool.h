#ifndef _TEGIA_THREAD_POOL_H_
#define _TEGIA_THREAD_POOL_H_
// --------------------------------------------------------------------

#include <tegia/core/crypt.h>

#include "thread_worker.h"


typedef std::shared_ptr<tegia::threads::worker> _worker_ptr;

namespace tegia {
namespace threads {

class pool
{
	protected:
		std::unordered_map<std::string, _worker_ptr> _workers;
		tegia::threads::queue * _queue;

		int init(int threads_count);
		int threads_count = 4;

	public:

		static pool* _self;
		static pool* instance(int threads_count = 4);

		pool();
		~pool();

		int add_task(std::function<void()> fn, int priority);
		int get_count();

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
		tegia::threads::data * const get_current();
#pragma GCC diagnostic pop

}; // class pool

}  // namespace threads
}  // namespace tegia



// --------------------------------------------------------------------
#endif