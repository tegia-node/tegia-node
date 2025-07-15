#ifndef H_TEGIA_NODE
#define H_TEGIA_NODE
//////////////////////////////////////////////////////////////////////////////////////////////////////

// STL
   #include <iostream>
   #include <tuple>

// PUBLIC
   #include <tegia/core/json.h>
   #include <tegia/core.h>
   #include <tegia/db/mysql/mysql.h>

// PRIVATE
   #include "../actors/map.h"
   #include "../threads/manager_t.h"
   #include "../threads/pool_t.h"
   #include "logger.h"
   #include "bucket.h"

//
//
//

namespace tegia {
namespace node {


class node
{
	public:
		
		const nlohmann::json * const config(const std::string &name);
		std::string config_path(const std::string &name);

		node();
		~node();
		bool run();
		bool action();

	
	
		int send_message(	// TODO: исключить
			const std::shared_ptr<message_t> &message,
			std::function<int(const std::shared_ptr<message_t> &)> fn,
			int priority);


	private:
		tegia::threads::pool_t    * _pool;  // TODO перенести в actor_map
		tegia::node::config       * _config;
		tegia::node::logger       * _logger;
		tegia::actors::map_t      * _actor_map;
		tegia::threads::manager_t * _manager;
		tegia::tasks::bucket_t    * _bucket;

};	// END class node


}	// END namespace node
}	// EMD namespace tegia


//////////////////////////////////////////////////////////////////////////////////////////////////////
#endif