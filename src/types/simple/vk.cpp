#include <tegia/types/simple/vk.h>
#include <tegia/core/cast.h>
#include <regex>

namespace tegia {
namespace types {


////////////////////////////////////////////////////////////////////////////////////////////
/**
		
*/   
////////////////////////////////////////////////////////////////////////////////////////////


vk_t::vk_t():simple_t("vk")
{
	
};


////////////////////////////////////////////////////////////////////////////////////////////
/**
		
*/   
////////////////////////////////////////////////////////////////////////////////////////////


int vk_t::parse(const std::string &value, const nlohmann::json &validate)
{	
	if(value == "")
	{
		this->_value = "";
		this->_is_valid = false;
		return 0;
	}

	//
	// Проверяем,что это прямая ссылка на ID
	//

	{
		std::regex  _regex(R"(id[0-9]{1,})");
		std::smatch _match;
		if(std::regex_search(value, _match, _regex))
		{
			this->_value = _match[0].str().substr(2);
			this->_is_valid = true;
			return 1;
		}
	}

	//
	// Проверяем,что это прямой ID
	//

	{
		std::regex  _regex(R"([0-9]{1,})");
		std::smatch _match;
		if(std::regex_search(value, _match, _regex))
		{
			this->_value = _match[0].str();
			this->_is_valid = true;
			return 1;
		}
	}

	//
	// TODO: Делать запрос к ВК
	//

	this->_value = "";
	
	std::cout << _ERR_TEXT_ <<  std::endl;
	std::cout << value << std::endl;
	std::cout << "not found pattern '" << R"(id([0-9]+)$)" << "'" << std::endl;

	exit(0);

	this->_is_valid = false;
	return 2;
};


////////////////////////////////////////////////////////////////////////////////////////////
/**
		
*/   
////////////////////////////////////////////////////////////////////////////////////////////


int vk_t::set_id(long long int value)
{
	if(value == 0)
	{
		this->_is_valid = false;
		this->_value = "";
		return 0;
	}
	else
	{
		this->_is_valid = true;
		this->_value = core::cast<std::string>(value);
		return 1;
	}
};


}	// END namespace identifier
}	// END namespace tegia
