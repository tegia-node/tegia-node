#include <tegia/dict/dict.h>

#include <iostream>

tegia::dict_t * tegia::dict_t::_dict= nullptr;

namespace tegia {

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




