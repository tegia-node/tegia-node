#include <iostream>
#include <tegia/core/const.h>

#include "catalog.h"

tegia::dictionaries::catalog_t * tegia::dictionaries::catalog_t::_catalog = nullptr;


/////////////////////////////////////////////////////////////////////////////////////////////////
/*

	tegia::dictionares::catalog_t

*/
/////////////////////////////////////////////////////////////////////////////////////////////////


namespace tegia {
namespace dictionaries {


/////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
/////////////////////////////////////////////////////////////////////////////////////////////////


catalog_t::catalog_t(){};
catalog_t * catalog_t::instance()
{
	if(_catalog == nullptr)
	{
		_catalog = new catalog_t();
	}
	return _catalog;
};


/////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
/////////////////////////////////////////////////////////////////////////////////////////////////


int catalog_t::_list(const std::string &folder)
{
	this->_folder = folder;
	std::string list_file = folder + "/list.json";

	auto [code,info,data] = tegia::json::_file(list_file);

	if(code != 0)
	{
		std::cout << _ERR_TEXT_ << "init dictionaries catalog" << std::endl;
		std::cout << "code = " << code << std::endl;
		std::cout << "info = " << info << std::endl;
		std::cout << data << std::endl;
 
		this->_status = 500;
		return 500;
	}

	this->_data = data;
	this->_status = 200;

	return 200;
};


/////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
/////////////////////////////////////////////////////////////////////////////////////////////////


int catalog_t::status()
{
	return this->_status;
};


/////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
/////////////////////////////////////////////////////////////////////////////////////////////////


int catalog_t::_dictionary(const std::string &dictionary_name)
{
	//
	//
	//

	nlohmann::json::json_pointer dictionary_file_ptr("/dictionaries/" + dictionary_name);
	if(this->_data.contains(dictionary_file_ptr) == false)
	{
		return 404;
	}

	//
	//
	//

	std::string dictionary_file = this->_folder + this->_data[dictionary_file_ptr].get<std::string>();
	auto [code,info,data] = tegia::json::_file(dictionary_file);

	/*
	std::cout << "code = " << code << std::endl;
	std::cout << "info = " << info << std::endl;
	std::cout << data["info"] << std::endl;
	std::cout << data["indexes"] << std::endl;
	*/

	if(code != 0)
	{
		return 500;
	}

	//
	// Создаем словарь
	//

	tegia::dictionaries::dictionary_t * dictionary = new tegia::dictionaries::dictionary_t();
	dictionary->filename = dictionary_file;
	dictionary->_data = data;

	this->_dictionaries.insert({dictionary_name,dictionary});

	//
	// Создаем индексы
	//

	for(auto it = data["indexes"].begin(); it != data["indexes"].end(); ++it)
	{
		// std::cout << it.key() << " = " << it.value().get<std::string>() << std::endl;

		nlohmann::json::json_pointer ptr(it.value().get<std::string>());
		tegia::dictionaries::index_t * index = new tegia::dictionaries::index_t();
		this->_indexes.insert({dictionary_name + "/" + it.key(),index});

		for(auto record = dictionary->_data["data"].begin(); record != dictionary->_data["data"].end(); ++record)
		{
			// std::cout << (*record)[ptr].get<std::string>() << std::endl;
			index->_index.insert({(*record)[ptr].get<std::string>(),&(*record)});
		}
	}

	return 200;
};


/////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
/////////////////////////////////////////////////////////////////////////////////////////////////


index_t * catalog_t::index(const std::string &dictionary, const std::string &index)
{
	//
	// Ищем индекс
	//

	{
		auto pos = this->_indexes.find(dictionary + "/" + index);
		if(pos != this->_indexes.end())
		{
			return pos->second;
		}
	}

	//
	// Проверяем загружен ли сам справочник
	//

	{
		auto pos = this->_dictionaries.find(dictionary);
		if(pos != this->_dictionaries.end())
		{
			// std::cout << _ERR_TEXT_ << "not found '" + dictionary + "/" + index + "'" << std::endl;
			return nullptr;
		}
	}

	//
	// Проверяем доступен ли справочник
	//

	if(this->_data["dictionaries"].contains(dictionary) == false)
	{
		// std::cout << _ERR_TEXT_ << "not available '" + dictionary + "'" << std::endl;
		std::cout << this->_data << std::endl;
		return nullptr;
	}

	//
	// Загружаем справочник
	//

	int res = _catalog->_dictionary(dictionary);
	if(res != 200)
	{
		// std::cout << _ERR_TEXT_ << "not init '" + dictionary + "'" << std::endl;
		return nullptr;
	}

	std::cout << _OK_TEXT_ << "LOAD INDEX '" << dictionary << "/" + index + "'" << std::endl;

	return _catalog->index(dictionary,index);
};


}
}



