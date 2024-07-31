#include <tegia/dict/dict.h>
#include <tegia/core/const.h>

#include <iostream>

tegia::dicts::catalog_t * tegia::dicts::catalog_t::_catalog = nullptr;


/////////////////////////////////////////////////////////////////////////////////////////////////
/*

	tegia::dicts::index_t

*/
/////////////////////////////////////////////////////////////////////////////////////////////////


namespace tegia {
namespace dicts {


std::tuple<bool,nlohmann::json> index_t::find(const std::string &index)
{
	auto pos = this->_index.find(index);
	if(pos == this->_index.end())
	{
		return std::make_tuple(false,nullptr);
	}

	return std::make_tuple(true,*pos->second);
};


}
}


/////////////////////////////////////////////////////////////////////////////////////////////////
/*

	tegia::dicts::catalog_t

*/
/////////////////////////////////////////////////////////////////////////////////////////////////


namespace tegia {
namespace dicts {


/////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
/////////////////////////////////////////////////////////////////////////////////////////////////


catalog_t::catalog_t(nlohmann::json &data):_data(data)
{

};


/////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
/////////////////////////////////////////////////////////////////////////////////////////////////


bool catalog_t::init(const std::string &dict_name,const std::string &dict_file)
{
	auto [code,info,data] = tegia::json::_file(dict_file);

	/*
	std::cout << "code = " << code << std::endl;
	std::cout << "info = " << info << std::endl;
	std::cout << data["info"] << std::endl;
	std::cout << data["indexes"] << std::endl;
	*/

	if(code != 0)
	{
		return false;
	}

	//
	// Создаем словарь
	//

	tegia::dicts::dict_t * dict = new tegia::dicts::dict_t();
	dict->filename = dict_file;
	dict->_data = data;

	this->_dictionaries.insert({dict_name,dict});

	//
	// Создаем индексы
	//

	for(auto it = data["indexes"].begin(); it != data["indexes"].end(); ++it)
	{
		// std::cout << it.key() << " = " << it.value().get<std::string>() << std::endl;

		nlohmann::json::json_pointer ptr(it.value().get<std::string>());
		tegia::dicts::index_t * index = new tegia::dicts::index_t();
		this->_indexes.insert({dict_name + "/" + it.key(),index});

		for(auto record = dict->_data["data"].begin(); record != dict->_data["data"].end(); ++record)
		{
			// std::cout << (*record)[ptr].get<std::string>() << std::endl;
			index->_index.insert({(*record)[ptr].get<std::string>(),&(*record)});
		}
	}

	return true;
};


/////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
/////////////////////////////////////////////////////////////////////////////////////////////////


catalog_t * catalog_t::instance(nlohmann::json data)
{
	if(_catalog == nullptr)
	{
		_catalog = new catalog_t(data);
	}
	return _catalog;
};


/////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
/////////////////////////////////////////////////////////////////////////////////////////////////


index_t * catalog_t::index(const std::string &dictionary, const std::string &index)
{
	if(_catalog == nullptr)
	{
		return nullptr;
	}

	//
	// Ищем индекс
	//

	{
		auto pos = _catalog->_indexes.find(dictionary + "/" + index);
		if(pos != _catalog->_indexes.end())
		{
			return pos->second;
		}
	}

	//
	// Проверяем загружен ли сам справочник
	//

	{
		auto pos = _catalog->_dictionaries.find(dictionary);
		if(pos != _catalog->_dictionaries.end())
		{
			return nullptr;
		}
	}

	//
	// Проверяем доступен ли справочник
	//

	if(_catalog->_data.contains(dictionary) == false)
	{
		return nullptr;
	}

	//
	// Загружаем справочник
	//

	bool res = _catalog->init(dictionary,_catalog->_data[dictionary].get<std::string>());
	if(res == false)
	{
		return nullptr;
	}

	std::cout << _OK_TEXT_ << "LOAD INDEX 'ISO 3166-1/alpha-3'" << std::endl;

	return _catalog->index(dictionary,index);
};


}
}















tegia::dict_t * tegia::dict_t::_dict= nullptr;

namespace tegia {


/////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
/////////////////////////////////////////////////////////////////////////////////////////////////


dict_t::dict_t():filename("")
{

};


/////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
/////////////////////////////////////////////////////////////////////////////////////////////////

dict_t * dict_t::instance()
{
    if(_dict == nullptr)
	{
        _dict = new dict_t();
		_dict->init();
    }
    return _dict;
};


/////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
/////////////////////////////////////////////////////////////////////////////////////////////////

void dict_t::init()
{
	std::cout << "INIT DICT" << std::endl;

	auto [code,info,data] = tegia::json::_file(this->folder + "/countries.json");

	std::cout << "code = " << code << std::endl;
	std::cout << "info = " << info << std::endl;

	std::cout << data["info"] << std::endl;

	for(auto it = data["data"].begin(); it != data["data"].end(); ++it)
	{
		std::string code = (*it)["phone"]["code"].get<std::string>();
		auto pos = this->_map.find(code.substr(1));
		if(pos == this->_map.end())
		{
			nlohmann::json tmp;
			tmp["country"] = (*it)["code"];
			tmp["regexp"] = (*it)["phone"]["regexp"].get<std::string>();

			this->_map.insert({code.substr(1),tmp});
		}

		std::cout << (*it)["code"]["alpha-3"].get<std::string>() 
				<< " :: " << (*it)["code"]["number"].get<std::string>() 
				<< " :: " << code.substr(1)
				<< std::endl;
	}

	// exit(0); 
};


nlohmann::json dict_t::find(const std::string &phone)
{
	for(size_t i = 1; i < phone.size(); ++i)
	{
		auto pos = this->_map.find(phone.substr(0,i));
		if(pos != this->_map.end())
		{
			return pos->second;
		}
	}

	return nullptr;
};



}	// END namespace tegia




