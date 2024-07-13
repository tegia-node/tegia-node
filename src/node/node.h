#ifndef H_TEGIA_NODE
#define H_TEGIA_NODE
// ---------------------------------------------------------------------------------------------------

#include <iostream>
#include <tuple>

#include <tegia/core/json.h>
#include <tegia/dict/dict.h>

#include "../actors/map.h"

#include "config.h"



/*
struct actor_t
{
	std::string name;
	tegia::actors::actor_base * actor;
	tegia::actors::type_base *type;
};
*/


namespace tegia {
namespace threads {
	class pool;
}
}

namespace tegia {
namespace node {

class node
{
	public:

		// static node * _self;
		// static node * instance();
		
		const nlohmann::json * const config(const std::string &name);
		std::string config_path(const std::string &name);

		node();
		~node();
		bool run();
		bool action();

		// const nlohmann::json * const config();

		

		int send_message(
			const std::string &actor, 
			const std::string &action, 
			const std::shared_ptr<message_t> &message,
			int priority);

		int resolve(const std::string &name);

	private:
		tegia::threads::pool * _threads;
		tegia::node::config  * _config;


		tegia::actors::map_t actor_map;


		void init_thread(const nlohmann::json &config);



};	// END class node

}	// END namespace node
}	// EMD namespace TEGIA


// ---------------------------------------------------------------------------------------------------
#endif