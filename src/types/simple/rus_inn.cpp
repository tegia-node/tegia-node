#include <tegia/types/simple/rus_inn.h>
#include <regex>

namespace tegia {
namespace types {


////////////////////////////////////////////////////////////////////////////////////////////
/**
		
*/   
////////////////////////////////////////////////////////////////////////////////////////////


rus_inn_t::rus_inn_t():simple_t("rus_inn")
{
	this->_code = 3643001;
};


////////////////////////////////////////////////////////////////////////////////////////////
/**
		
	0 - empty
	1 - ok
	2 - not valid

*/   
////////////////////////////////////////////////////////////////////////////////////////////


int rus_inn_t::parse(const std::string &value, const nlohmann::json &validate)
{
	if(value == "")
	{
		this->_value = value;
		this->_is_valid = false;
		return 0;
	}

	//
	// Выделяем только цифры
	//

	std::string tmp = "";

	{
		std::regex r(R"([0-9])");

		for(std::sregex_iterator i = std::sregex_iterator(value.begin(), value.end(), r);
								i != std::sregex_iterator();
								++i)
		{
			std::smatch m = *i;
			tmp = tmp + m.str();
		}
	}

	//
	// Проверяем длину
	//

	if(tmp == "")
	{
		this->_value = "";
		this->_is_valid = false;
		return 0;		
	}

	if(tmp.length() != 12)
	{
		this->_value = "";
		this->_is_valid = false;
		return 2;
	}

	// 
	// FORMAT
	//

	this->_value = tmp;
	this->_is_valid = true;
	return 1;
};


}	// END namespace types
}	// END namespace tegia
