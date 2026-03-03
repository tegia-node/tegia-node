#ifndef H_TEGIA_APP_ROUTER
#define H_TEGIA_APP_ROUTER


#include <tuple>
#include <iostream>
#include <tegia/core/json.h>


namespace tegia {
namespace app {

enum class route_source_t
{
	type_level = 0,
	legacy_instance = 1
};

class router_t
{
	public:
		router_t(const std::string &ws_name, const std::string &ws_type = "");
		~router_t() = default;

		bool add(const std::string &method, const std::string &pattern, nlohmann::json data);
		bool add(const std::string &method, const std::string &pattern, nlohmann::json data, route_source_t source);
		std::tuple<int, nlohmann::json> match(
			int role,
			const std::string &method, 
			const std::string &path, 
			nlohmann::json * post);

		void print();


	private:
		std::string ws_name;
		std::string ws_type;
		std::unordered_map<std::string, nlohmann::json> _route_map;
		std::unordered_map<std::string, route_source_t> _route_source;

		void actor_name(nlohmann::json * _params);
		void action_name(nlohmann::json * _params);
};


}	// END namespace app
}	// END namespace tegia

#endif
