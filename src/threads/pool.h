#ifndef H_TEGIA_THREAD_POOL
#define H_TEGIA_THREAD_POOL
// --------------------------------------------------------------------

#include <tegia/core/crypt.h>
#include <tegia2/context/context.h>

#include "../node/config.h"

#include "queue.h" 
#include "data.h"
#include "worker.h"


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

		int init(int threads_count, const nlohmann::json &db_config, std::function<void()> _callback);
		void signal(const std::string &tid);
		int add_task(std::function<void()> _fn, int priority);

}; // class pool

}  // namespace threads
}  // namespace tegia



// --------------------------------------------------------------------
#endif