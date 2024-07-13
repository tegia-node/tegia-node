#ifndef H_TEGIA_ACTORS_MAP
#define H_TEGIA_ACTORS_MAP

#include <iostream>
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


//using actor_factory_fn = std::function<tegia::actors::actor_base_t * (const std::string &)>;


class map_t
{
	public:
		map_t();
		~map_t();

		std::unordered_map <std::string, tegia::actors::domain_t *> _domains;
		std::unordered_map <std::string, nlohmann::json> _types;
		std::unordered_map <std::string, tegia::actors::type_base_t *> _patterns;
		std::unordered_map<std::string, tegia::actors::actor_base_t * > actors;

		//
		// domain
		//

		void add_domain(const std::string &name, int type);

		//
		// types
		//

		void add_type(
			const std::string &type_name, 
			const std::string &base_path, 
			nlohmann::json * data
		);

		//
		//
		//
		
		std::tuple<int,std::function<void()>> send_message(
			const std::string &name, 
			const std::string &action, 
			const std::shared_ptr<message_t> &message);
};


//
//
//


} // namespace actors
} // namespace tegia


#endif