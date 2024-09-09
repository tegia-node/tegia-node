#ifndef H_TEGIA_NODE
#define H_TEGIA_NODE
//////////////////////////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <tuple>

#include <tegia/core/json.h>
#include <tegia/dict/dict.h>

#include "../actors/map.h"

#include "config.h"

//
//
//

namespace tegia {
namespace threads {

class pool;

}
}

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

		int send_message(
			const std::string &actor, 
			const std::string &action, 
			const std::shared_ptr<message_t> &message,
			int priority);

		int send_message(
			const std::shared_ptr<message_t> &message,
			std::function<int(const std::shared_ptr<message_t> &)> fn,
			int priority);

	private:
		tegia::threads::pool * _threads;
		tegia::node::config  * _config;

		tegia::actors::map_t actor_map;

		void init_thread(const nlohmann::json &config);

};	// END class node

}	// END namespace node
}	// EMD namespace tegia


//////////////////////////////////////////////////////////////////////////////////////////////////////
#endif