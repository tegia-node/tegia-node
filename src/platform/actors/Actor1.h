#ifndef H_TEGIA_PLATFORM_ACTORS_ACTOR1
#define H_TEGIA_PLATFORM_ACTORS_ACTOR1

#include <tegia/node/node.h>


#define ACTOR_TYPE "Actor1"

class actor1: public tegia::actors::actor_base
{
	private:
		
	public:
		actor1(const std::string &name);
		~actor1();

		std::string func1(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params);


		//
		// 
		//	

		static void add_route(tegia::actors::router_map * _map)
		{
			auto ro = new tegia::actors::router<actor1>(ACTOR_TYPE);

			// ro->add("/action1/{test}", std::bind(&actor1::func1, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
			ACTION_ROUTE2("/action1/{test}", &actor1::func1);

			_map->_map.insert({ACTOR_TYPE, ro});
		};	

		//
		// 
		//	

		int init_message(
			tegia::actors::router_map * _map, 
			const std::string &action_name, 
			const std::shared_ptr<message_t> &message)
		{
			return _map->init_job(this, action_name, message);
		};

};

////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// IMPORT                                                                                 //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

extern "C" tegia::actors::actor_base * new_actor(const std::string &name, nlohmann::json data = nlohmann::json())
{
	return new actor1(name);
};

extern "C" void load_actor(tegia::actors::router_map * _map)
{
	actor1::add_route(_map);
};


#endif