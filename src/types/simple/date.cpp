#include <regex>
#include <tegia/core/crypt.h>
#include <tegia/types/simple/date.h>

namespace tegia {
namespace types {


////////////////////////////////////////////////////////////////////////////////////////////
/**
		
*/   
////////////////////////////////////////////////////////////////////////////////////////////


date_t::date_t():simple_t("date")
{
	
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
	//
	//

	if(tmp == "")
	{
		this->_value = "";
		this->_is_valid = false;
		return 0;
	}

	//
	//
	//

	if(validate.contains("expected") == false)
	{
		exit(0);

		this->_value = tmp;
		this->_is_valid = true;
		return 1;
	}

	// std::cout << "expected = " << validate["expected"].get<std::string>() << std::endl;
	// std::cout << "value    = " << value << std::endl;

	std::regex  _regex(validate["expected"].get<std::string>());
	std::smatch _match;

	if(std::regex_match(tmp, _match, _regex) == false)
	{
		this->_value = tmp;
		this->_is_valid = false;
		return 2;
	}

	long long int format = tegia::crypt::crc32(validate["expected"].get<std::string>());
	switch(format)
	{
		case 1995152703: // [0-9]{4}-[0-9]{2}-[0-9]{2} [0-9]{2}:[0-9]{2}:[0-9]{2}
		case 970670383:  // [0-9]{4}-[0-9]{2}-[0-9]{2} [0-9]{2}:[0-9]{2}:[0-9]{2}.[0-9]{3}
		{
			this->_value = tmp.substr(0,10);
			this->_is_valid = true;
			return 1;
		}
		break;

		case 295637175: // [0-9]{4}-[0-9]{2}-[0-9]{2}
		{
			this->_value = tmp;
			this->_is_valid = true;
			return 1;
		}
		break;

		case 284019523: // [0-9]{2}.[0-9]{2}.[0-9]{4}
		{
			this->_value = tmp.substr(6,4) + "-" + tmp.substr(3,2) + "-" + tmp.substr(0,2);
			this->_is_valid = true;
			return 1;
		}
		break;

		default:
		{
			std::cout << _ERR_TEXT_ << std::endl;
			std::cout << "value    = [" << tmp << "]" << std::endl;
			std::cout << "expected = [" << validate["expected"].get<std::string>() << "]" << std::endl;
			std::cout << "crc32    = " << format << std::endl;
			exit(0);
		}
		break;
	}
	// END switch(format)
};



}	// END namespace identifier
}	// END namespace tegia
