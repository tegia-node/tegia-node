#include <tegia/types/simple/rus_snils.h>
#include <regex>

namespace tegia {
namespace types {


////////////////////////////////////////////////////////////////////////////////////////////
/**
		
*/   
////////////////////////////////////////////////////////////////////////////////////////////


rus_snils_t::rus_snils_t():simple_t("rus_snils")
{
	
};


////////////////////////////////////////////////////////////////////////////////////////////
/**
		
	0 - empty
	1 - ok
	2 - not valid

*/   
////////////////////////////////////////////////////////////////////////////////////////////


int rus_snils_t::parse(const std::string &value, const nlohmann::json &validate)
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

	std::string __snils = "";

	{
		std::regex r(R"([0-9])");

		for(std::sregex_iterator i = std::sregex_iterator(value.begin(), value.end(), r);
								i != std::sregex_iterator();
								++i)
		{
			std::smatch m = *i;
			__snils = __snils + m.str();
		}
	}

	//
	// Проверяем длину
	//

	if(__snils == "")
	{
		this->_value = "";
		this->_is_valid = false;
		return 0;		
	}

	if(__snils.length() != 11)
	{
		this->_value = "";
		this->_is_valid = false;
		return 2;
	}

	// 
	// FORMAT
	//

	/*
		ХХХ-ХХХ-ХХХ YY
	*/

	__snils = __snils.substr(0,3) + "-" + __snils.substr(3,3) + "-" + __snils.substr(6,3) + " " + __snils.substr(9,2);

	this->_value = __snils;
	this->_is_valid = true;
	return 1;
};



}	// END namespace types
}	// END namespace tegia
