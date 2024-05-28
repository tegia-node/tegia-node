#include <regex>
#include <tegia/core/crypt.h>
#include <tegia/types/date.h>

namespace tegia {
namespace types {


////////////////////////////////////////////////////////////////////////////////////////////
/**
		
*/   
////////////////////////////////////////////////////////////////////////////////////////////


date_t::date_t():base_t("date")
{
	
};


////////////////////////////////////////////////////////////////////////////////////////////
/**
		
*/   
////////////////////////////////////////////////////////////////////////////////////////////


std::string date_t::value() const
{
	return this->_date;
};


////////////////////////////////////////////////////////////////////////////////////////////
/**
		
*/   
////////////////////////////////////////////////////////////////////////////////////////////


std::string date_t::hash() const
{
	return tegia::crypt::MD5u(this->_date);
};


////////////////////////////////////////////////////////////////////////////////////////////
/**
		
*/   
////////////////////////////////////////////////////////////////////////////////////////////


nlohmann::json date_t::json() const
{
	nlohmann::json tmp;
	tmp["date"] = this->_date;

	return std::move(tmp);
};

nlohmann::json date_t::graph() const
{
	nlohmann::json tmp;
	tmp["type"] = "date";
	tmp["value"] = this->_date;

	return std::move(tmp);
};


////////////////////////////////////////////////////////////////////////////////////////////
/**
	validate

	{
		"expected": "[0-9]{4}-[0-9]{2}-[0-9]{2}",
		"return": "YYYY-MM-DD",
		"strict": true 
	}
*/   
////////////////////////////////////////////////////////////////////////////////////////////


int date_t::parse(const std::string &value, const nlohmann::json &validate)
{
	std::string _value = value;

	//
	//
	//

	nlohmann::json::json_pointer ptr("/replace/" + _value);
	if(validate.contains(ptr) == true)
	{
		_value = validate[ptr].get<std::string>();
	}

	//
	//
	//

	if(_value == "")
	{
		this->_date = "";
		this->_is_valid = false;
		return 0;
	}

	//
	//
	//

	if(validate.contains("expected") == false)
	{
		this->_date = _value;
		this->_is_valid = true;
		return 1;
	}

	std::regex  _regex(validate["expected"].get<std::string>());
	std::smatch _match;

	if(std::regex_match(_value, _match, _regex) == false)
	{
		this->_date = _value;
		this->_is_valid = false;
		return 2;
	}

	long long int format = tegia::crypt::crc32(validate["expected"].get<std::string>());
	switch(format)
	{
		case 1995152703: // [0-9]{4}-[0-9]{2}-[0-9]{2} [0-9]{2}:[0-9]{2}:[0-9]{2}
		case 970670383:  // [0-9]{4}-[0-9]{2}-[0-9]{2} [0-9]{2}:[0-9]{2}:[0-9]{2}.[0-9]{3}
		{
			this->_date = value.substr(0,10);
			this->_is_valid = true;
			return 1;
		}
		break;

		case 295637175: // [0-9]{4}-[0-9]{2}-[0-9]{2}
		{
			this->_date = _value;
			this->_is_valid = true;
			return 1;
		}
		break;

		case 284019523: // [0-9]{2}.[0-9]{2}.[0-9]{4}
		{
			this->_date = _value.substr(6,4) + "-" + _value.substr(3,2) + "-" + _value.substr(0,2);
			this->_is_valid = true;
			return 1;
		}
		break;

		default:
		{
			std::cout << _ERR_TEXT_ << std::endl;
			std::cout << "value    = [" << _value << "]" << std::endl;
			std::cout << "expected = [" << validate["expected"].get<std::string>() << "]" << std::endl;
			std::cout << "crc32    = " << format << std::endl;
			exit(0);
		}
		break;
	}
	// END switch(format)

	/*
	{
		this->_date = value;
		this->_is_valid = true;
		return 1;
	}
	*/
};



}	// END namespace identifier
}	// END namespace tegia
