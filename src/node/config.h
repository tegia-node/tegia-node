#ifndef H_TEGIA_NODE_CONFIG
#define H_TEGIA_NODE_CONFIG
// ---------------------------------------------------------------------------------------------------

#include <iostream>
#include <tegia/core/json.h>


struct _conf
{
	std::string name;
	std::string file;
	nlohmann::json data;
};


namespace tegia {
namespace node {

class node;

class config
{
	friend class tegia::node::node;

	public:
		config();
		~config();

		bool load();
		const nlohmann::json * const get(const std::string &name);

	private:
		std::unordered_map<std::string, _conf *> _map;
		std::vector<std::string> _names;

		int thread_count = 5;

};	// END class config

}	// END namespace node
}	// EMD namespace tegia


// ---------------------------------------------------------------------------------------------------
#endif