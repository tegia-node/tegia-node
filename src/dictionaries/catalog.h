#ifndef H_TEGIA_DICTIONARIES_CATALOG
#define H_TEGIA_DICTIONARIES_CATALOG

#include <string>
#include <unordered_map>

#include <tegia/core/json.h>
#include <tegia/dictionaries/index.h>
#include "dictionary.h"


namespace tegia {
namespace node {
	class node;
}
}


namespace tegia {
namespace dictionaries {

class catalog_t
{
	friend class tegia::node::node;
	
	protected:
		catalog_t();
		int _list(const std::string &folder);
		int _dictionary(const std::string &dictionary_name);
		

		// bool init(const std::string &dict_name,const std::string &dict_file);

		static catalog_t * _catalog;

		// 
		//
		//

		std::string  _folder;
		nlohmann::json _data;
		int _status = 0;

		/*
			0  - не инициализирован
			200 - инициализирован
			500 - ошибка
		*/

		std::unordered_map<std::string, index_t *> _indexes;
		std::unordered_map<std::string, dictionary_t *>  _dictionaries;

	public:	
		catalog_t(catalog_t &other) = delete;
		void operator=(const catalog_t &) = delete;

		static catalog_t * instance();

		index_t * index(const std::string &dictionary, const std::string &index);
		int status();
};


} // END namespace dictionares
} // END namespace tegia


#endif