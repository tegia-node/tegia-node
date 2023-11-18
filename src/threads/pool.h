#ifndef H_TEGIA_THREAD_POOL
#define H_TEGIA_THREAD_POOL
// --------------------------------------------------------------------

#include <tegia/core/crypt.h>
#include <tegia/const.h>
#include <tegia/core/cast.h>
#include <tegia2/context/context.h>


#include "queue.h" 
#include "worker.h"

namespace tegia {
class context2;
}	// END namespace tegia

namespace tegia {
namespace threads {

class pool
{
	protected:
		std::unordered_map<std::string, std::shared_ptr<tegia::threads::worker>> _workers;
		std::unordered_map<std::string, bool> _workers_status;

		tegia::threads::queue * queue;

		int threads_count = 4;

		std::mutex signal_mutex;
		std::function<void()> _callback;

	public:
		pool();
		~pool();

		int init(int threads_count, std::function<::tegia::context2 const * ()> _thread_init, std::function<void()> _callback);
		::tegia::context2 const * thread_init(const std::string &tid, std::function<::tegia::context2 const * ()> _thread_init);
		void signal(const std::string &tid);
		int add_task(std::function<void(::tegia::context2 const *)> _fn, int priority);

}; // class pool

}  // namespace threads
}  // namespace tegia



// --------------------------------------------------------------------
#endif