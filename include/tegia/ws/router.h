#ifndef H_TEGIA_APP_ROUTER
#define H_TEGIA_APP_ROUTER


#include <tuple>
#include <iostream>
#include <tegia/core/json.h>


namespace tegia {
namespace app {

class router_t
{
	public:
		router_t(const std::string &ws_name);
		~router_t() = default;

		bool add(const std::string &method, const std::string &pattern, nlohmann::json data);
		std::tuple<int, nlohmann::json> match(
			int role,
			const std::string &method, 
			const std::string &path, 
			nlohmann::json * post);

		void print();


	private:
		std::string ws_name;
		std::unordered_map<std::string, nlohmann::json> _route_map;

		void actor_name(nlohmann::json * _params);
		void action_name(nlohmann::json * _params);
};


}	// END namespace app
}	// END namespace tegia

#endif