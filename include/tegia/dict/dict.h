#ifndef H_TEGIA_DICTIONARES
#define H_TEGIA_DICTIONARES

#include <string>
#include <unordered_map>

#include <tegia/core/json.h>


namespace tegia {

class dict_t
{
	protected:
		dict_t() = default;
		static dict_t* _dict;

		void init();

		std::string folder = "/home/igor/tegia_v2/tegia-node/dicts";

		std::unordered_map<std::string,nlohmann::json> _map;

	public:

		dict_t(dict_t &other) = delete;
		void operator=(const dict_t &) = delete;
		static dict_t * instance();

		nlohmann::json find(const std::string &phone);

};	// END class dict_t

}	// END namespace tegia



#endif