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
	return tegia::crypt::MD5u(this->_phone);
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

nlohmann::json phone_t::graph() const
{
	nlohmann::json tmp;
	tmp["type"] = "phone";
	tmp["value"] = this->_phone;

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
	// Выделяем только цифры
	//

	if(_value == "")
	{
		this->_phone = _value;
		this->_is_valid = false;
		return 0;
	}

	if(_value == "0")
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


	if(_phone.size() < 6)
	{
		this->_phone = _phone;
		this->_is_valid = false;
		return 1;	
	}

	//
	// CHECK PHONE
	//

	auto dict = tegia::dict_t::instance();
	auto info = dict->find(_phone);

	if(info != nullptr)
	{
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
	/*
	else
	{
		std::cout << _ERR_TEXT_ << "not found INFO" << std::endl;
		std::cout << "      value  = '" << value << "'" << std::endl;
		std::cout << "      phone '" << _phone << "'" << std::endl;

		this->_phone = _phone;
		this->_is_valid = false;
		return 1;			
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
			this->_phone = "7" + _phone.substr(1);
			this->_is_valid = true;
			this->_category = 4643001;
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
			this->_phone = "7" + _phone;
			this->_is_valid = true;
			this->_category = 4643001;
			return 1;
		}
	}

	if(_phone.size() > 10)
	{
		/*
		std::cout << _ERR_TEXT_ << "not match phone" << std::endl;
		std::cout << "      value  = '" << value << "'" << std::endl;
		std::cout << "      phone '" << _phone << "'" << std::endl;
		*/
	}

	this->_phone = _phone;
	this->_is_valid = false;
	return 1;
};



}	// END namespace types
}	// END namespace tegia
