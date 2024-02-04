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
		router_t() = default;
		~router_t() = default;

		bool add(const std::string &method, const std::string &pattern, nlohmann::json data);
		std::tuple<bool, std::string, nlohmann::json> match(const std::string &method, const std::string &path);


		void print();


	private:
		std::unordered_map<std::string, nlohmann::json> _route_map;
};


}	// END namespace app
}	// END namespace tegia

#endif