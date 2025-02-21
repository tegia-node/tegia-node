#ifndef H_TEGIA_THREADS_MANAGER
#define H_TEGIA_THREADS_MANAGER

#include <mutex>
#include <thread>

#include "../db/mysql/provider.h"
#include "../node/config.h"
#include "../node/logger.h"

#include "thread_t.h"


//////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
//////////////////////////////////////////////////////////////////////////////////////////////////


namespace tegia {
namespace threads {

class manager_t
{
	public:
		manager_t() = default;
		~manager_t() = default;

		void init(
			::tegia::node::node     * _node, 
			::tegia::node::config   * _config,
			::tegia::node::logger   * _logger,
			::tegia::actors::map_t  * _actor_map);
		int thread(std::function<void()> _fn);

	private:
		::tegia::node::node     * _node;
		::tegia::node::config   * _config;
		::tegia::node::logger   * _logger;
		::tegia::actors::map_t  * _actor_map;

};


} // END namespace threads
} // END namespace tegia

#endif