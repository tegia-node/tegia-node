#include <tegia/core/cast.h>
#include <tegia/types/simple/phone.h>
#include <regex>

#include <tegia/dict/dict.h>

namespace tegia {
namespace types {


////////////////////////////////////////////////////////////////////////////////////////////
/**
		
*/   
////////////////////////////////////////////////////////////////////////////////////////////


phone_t::phone_t():simple_t("phone")
{
	
};


////////////////////////////////////////////////////////////////////////////////////////////
/**

	0 - empty
	1 - ok
	2 - not valid

*/   
////////////////////////////////////////////////////////////////////////////////////////////


int phone_t::parse(const std::string &value, const nlohmann::json &validate)
{
	// std::cout << "phone_t::parse: " << value << std::endl;

	this->_code = 0;
	std::string tmp = value;

	//
	//
	//

	nlohmann::json::json_pointer ptr("/replace/" + tmp);
	if(validate.contains(ptr) == true)
	{
		tmp = validate[ptr].get<std::string>();
	}

	//
	// Выделяем только цифры
	//

	if(tmp == "")
	{
		this->_value = tmp;
		this->_is_valid = false;
		return 0;
	}

	if(tmp == "0")
	{
		this->_value = tmp;
		this->_is_valid = false;
		return 0;
	}

	std::string _phone = "";

	{
		std::regex r(R"([0-9])");

		for(std::sregex_iterator i = std::sregex_iterator(tmp.begin(), tmp.end(), r);
								i != std::sregex_iterator();
								++i)
		{
			std::smatch m = *i;
			_phone = _phone + m.str();
		}
	}


	if(_phone.size() < 6)
	{
		this->_value = _phone;
		this->_is_valid = false;
		return 2;	
	}

	//
	// CHECK PHONE
	//

	auto index = tegia::dicts::catalog_t::index("ITU-T E.164","prefix");
	if(index == nullptr)
	{
		std::cout << _ERR_TEXT_ << "NOT LOAD INDEX 'ITU-T E.164/prefix'" << std::endl;
		exit(0);
	}

	for(size_t i = 1; i < _phone.size(); ++i)
	{
		auto [res,data] = index->find("+" + _phone.substr(0,i));

		if(res == true)
		{
			// std::cout << data << std::endl;

			this->_value = _phone;
			this->_is_valid = true;
			this->_code = 4000001 + core::cast<int>(data["country"]["number"].get<std::string>()) * 1000;
			return 1;
		}
	}


	//
	// OLD SEARCH
	//

	/*
	auto dict = tegia::dict_t::instance();
	auto info = dict->find(_phone);

	if(info != nullptr)
	{
		{
			std::regex  _regex(info["regexp"].get<std::string>());
			std::smatch _match;
			if(std::regex_match(_phone, _match, _regex))
			{
				this->_value = _phone;
				this->_is_valid = true;
				this->_code = 4000001 + core::cast<int>(info["country"]["number"].get<std::string>()) * 1000;
				return 1;
			}
		}
	}
	*/

	//
	// Особый случай для России [1] 8 XXX XXX XXXX
	//

	{
		std::regex  _regex("^8[0-9]{10}$");
		std::smatch _match;
		if(std::regex_match(_phone, _match, _regex))
		{
			this->_value = "7" + _phone.substr(1);
			this->_is_valid = true;
			this->_code = 4643001;
			return 1;
		}
	}

	//
	// Особый случай для России [2] 9XX XXX XXXX
	//

	{
		std::regex  _regex("^9[0-9]{9}$");
		std::smatch _match;
		if(std::regex_match(_phone, _match, _regex))
		{
			this->_value = "7" + _phone;
			this->_is_valid = true;
			this->_code = 4643001;
			return 1;
		}
	}

	this->_value = _phone;
	this->_is_valid = false;
	return 2;
};



}	// END namespace types
}	// END namespace tegia
