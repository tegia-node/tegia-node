#include <tegia/core/cast.h>
#include <tegia/types/phone.h>
#include <regex>

#include <tegia/dict/dict.h>

namespace tegia {
namespace types {


////////////////////////////////////////////////////////////////////////////////////////////
/**
		
*/   
////////////////////////////////////////////////////////////////////////////////////////////


phone_t::phone_t():base_t("phone")
{
	
};


////////////////////////////////////////////////////////////////////////////////////////////
/**
		
*/   
////////////////////////////////////////////////////////////////////////////////////////////


std::string phone_t::value() const
{
	return this->_phone;
};


////////////////////////////////////////////////////////////////////////////////////////////
/**
		
*/   
////////////////////////////////////////////////////////////////////////////////////////////


std::string phone_t::hash() const
{
	return tegia::crypt::MD5(this->_phone);
};


////////////////////////////////////////////////////////////////////////////////////////////
/**
		
*/   
////////////////////////////////////////////////////////////////////////////////////////////


nlohmann::json phone_t::json() const
{
	nlohmann::json tmp;
	tmp["phone"] = this->_phone;

	return std::move(tmp);
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

	this->_category = 0;
	std::string _value = value;

	//
	//
	//

	/*
	nlohmann::json::json_pointer ptr("/replace/" + _value);
	if(validate.contains(ptr) == true)
	{
		_value = validate[ptr].get<std::string>();
	}
	*/

	//
	// Выделяем только цифры
	//

	if(_value == "")
	{
		this->_phone = _value;
		this->_is_valid = false;
		return 0;
	}

	std::string _phone = "";

	{
		std::regex r(R"([0-9])");

		for(std::sregex_iterator i = std::sregex_iterator(_value.begin(), _value.end(), r);
								i != std::sregex_iterator();
								++i)
		{
			std::smatch m = *i;
			_phone = _phone + m.str();
		}
	}


	//
	// CHECK PHONE
	//

	auto dict = tegia::dict_t::instance();
	auto info = dict->find(_phone);

	if(info != nullptr)
	{
		// std::cout << _OK_TEXT_ << "found country" << std::endl;
		// std::cout << info << std::endl;

		{
			std::regex  _regex(info["regexp"].get<std::string>());
			std::smatch _match;
			if(std::regex_match(_phone, _match, _regex))
			{
				this->_phone = _phone;
				this->_is_valid = true;
				this->_category = 4000001 + core::cast<int>(info["country"]["number"].get<std::string>()) * 1000;
				return 1;
			}		
		}
	}

	if(_phone.size() > 10)
	{
		std::cout << "value  = " << value << std::endl;
		std::cout << "_phone = " << _phone << std::endl;
	}

	// exit(0);

	this->_phone = _phone;
	this->_is_valid = false;
	return 1;



	/*
	{
		std::regex  _regex(R"(^(\+{0,1}7|8)[0-9]{10}$)");
		std::smatch _match;
		if(std::regex_match(_phone, _match, _regex))
		{
			this->_phone = _phone;
			this->_is_valid = true;
			this->_category = 4643001;
			return 1;
		}
	}
	*/

};



}	// END namespace types
}	// END namespace tegia
