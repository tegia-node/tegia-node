#ifndef H_TEGIA_NODE_CONFIG
#define H_TEGIA_NODE_CONFIG
// ---------------------------------------------------------------------------------------------------

#include <iostream>
#include <tegia/core/json.h>


struct conf_t
{
	std::string name;
	std::string file;
	std::string path;
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
		const nlohmann::json * cluster();
		const nlohmann::json * configuration(const std::string &name);
		const nlohmann::json * const get(const std::string &name);

	private:
		std::unordered_map<std::string, conf_t *> _map;
		nlohmann::json_schema::json_validator validator_db;

		nlohmann::json dbc;
		nlohmann::json messages;

		int thread_count = 5;

		conf_t * load(const std::string &name, const std::string &file);


};	// END class config

}	// END namespace node
}	// EMD namespace tegia


// ---------------------------------------------------------------------------------------------------
#endif