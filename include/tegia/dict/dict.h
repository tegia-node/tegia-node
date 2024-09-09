#ifndef H_TEGIA_DICTIONARES
#define H_TEGIA_DICTIONARES

#include <string>
#include <unordered_map>

#include <tegia/core/json.h>


namespace tegia {
namespace dicts {

class catalog_t;

class dict_t
{
	friend class catalog_t;

	protected:
		nlohmann::json _data;
		std::string filename;

	public:
		dict_t() = default;
		~dict_t() = default;
};


class index_t
{
	friend class catalog_t;

	protected:
		std::unordered_map<std::string, nlohmann::json *> _index;

	public:
		index_t() = default;
		~index_t() = default;

		std::tuple<bool,nlohmann::json> find(const std::string &index);
};


class catalog_t
{
	protected:
		catalog_t(nlohmann::json &data);
		bool init(const std::string &dict_name,const std::string &dict_file);

		static catalog_t * _catalog;

		nlohmann::json _data;
		std::unordered_map<std::string, index_t *> _indexes;
		std::unordered_map<std::string, dict_t *>  _dictionaries;

	public:	
		catalog_t(dict_t &other) = delete;
		void operator=(const catalog_t &) = delete;

		static catalog_t * instance(nlohmann::json data);
		static index_t * index(const std::string &dictionary, const std::string &index);

};


}
}


namespace tegia {

class dict_t
{
	protected:
		dict_t();
		static dict_t* _dict;

		void init();

		std::string folder = "/home/igor/tegia_v2/tegia-node/dicts";
		std::string filename = "";

		std::unordered_map<std::string,nlohmann::json> _map;

	public:

		dict_t(dict_t &other) = delete;
		void operator=(const dict_t &) = delete;
		static dict_t * instance();

		nlohmann::json find(const std::string &phone);

};	// END class dict_t

}	// END namespace tegia



#endif