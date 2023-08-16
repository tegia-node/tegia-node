#ifndef H_TEGIA_PLATFORM_ACTORS_ACTOR2
#define H_TEGIA_PLATFORM_ACTORS_ACTOR2

#include <tegia/node/node.h>

////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// ACTOR CLASS                                                                            //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

#define ACTOR_TYPE "Actor2"

class actor2: public tegia::actors::actor_base
{
	private:
		
	public:
		actor2(const std::string &name);
		~actor2();

		std::string func2(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params);


		//
		// 
		// 

		static void add_route(tegia::actors::router_map * _map)
		{
			auto ro = new tegia::actors::router<actor2>(ACTOR_TYPE);

			ACTION_ROUTE2("/action2", &actor2::func2);

			//ro->add("/action2", std::bind(&actor2::func2, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
			
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
	return new actor2(name);
};

extern "C" void load_actor(tegia::actors::router_map * _map)
{
	actor2::add_route(_map);
};


#endif