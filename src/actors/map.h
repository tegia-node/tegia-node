#ifndef H_TEGIA_ACTORS_MAP
#define H_TEGIA_ACTORS_MAP

#include <iostream>
#include <shared_mutex>

#include <tegia/core/json.h>
#include <tegia/core/const.h>
#include <tegia/actors/type.h>
#include <tegia/actors/actor.h>



//
//
//

namespace tegia {

namespace domain {

	static int LOCAL = 1;
	static int REMOUTE = 2;

} // END namespace domain
} // END namespace tegia


//
//
//

namespace tegia {
namespace actors {

class domain_t
{
	public:
		std::string _name = "";
		int _type = 0;

		domain_t() = default;
		~domain_t() = default;
};

//
//
//

class map_t
{
	protected:
		std::unordered_map<std::string, tegia::actors::action_t     * >  _actions;
		std::unordered_map<std::string, tegia::actors::actor_t      * >  _actors;
		std::unordered_map<std::string, tegia::actors::type_base_t  * >  _types;
		std::unordered_map<std::string, tegia::actors::type_base_t  * >  _patterns;
		std::unordered_map<std::string, tegia::actors::domain_t     * >  _domains;

	public:

		map_t() = default;
		~map_t() = default;

		//
		// domains
		//

		int add_domain(const std::string &name, int type);

		//
		// types
		//

		int add_type(
			const std::string &name, 
			const std::string &path
		);

		int add_pattern(
			const std::string &pattern, 
			const std::string &type
		);

		//
		//
		//
			
		std::tuple<int,std::function<void()>> send_message(
			const std::string &name, 
			const std::string &action, 
			const std::shared_ptr<message_t> &message);

		//
		//
		//

		int unload(const std::string &actor);

	
	private:

		void action_func(
			tegia::actors::actor_t * _actor,
			tegia::actors::action_t * _action,
			const std::shared_ptr<message_t> &message,
			std::shared_ptr<tegia::user> user);

		std::shared_mutex shared_mtx;

		// tegia::actors::actor_t * get_actor(const std::string &name);	


};



} // namespace actors
} // namespace tegia


#endif