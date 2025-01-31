#include <tegia/types/simple/rus_passport.h>
#include <regex>

namespace tegia {
namespace types {


////////////////////////////////////////////////////////////////////////////////////////////
/**
		
*/   
////////////////////////////////////////////////////////////////////////////////////////////


rus_passport_t::rus_passport_t():simple_t("rus_passport")
{
	
};


////////////////////////////////////////////////////////////////////////////////////////////
/**
		
	0 - empty
	1 - ok
	2 - not valid

*/   
////////////////////////////////////////////////////////////////////////////////////////////


int rus_passport_t::parse(const std::string & value, const nlohmann::json &validate)
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
	// FORMAT
	//

	/*
		ХХХХ YYYYYY
	*/

	if(tmp.length() == 10)
	{
		this->region = tmp.substr(0,2);
		this->series = tmp.substr(2,2);
		this->number = tmp.substr(4,6);

		this->_value = this->region + this->series + " " + this->number;
		this->_is_valid = true;
		return 1;
	}

	if(tmp.length() == 9)
	{
		this->region = "0" + tmp.substr(0,1);
		this->series = tmp.substr(1,2);
		this->number = tmp.substr(3,6);

		this->_value = this->region + this->series + " " + this->number;
		this->_is_valid = true;
		return 1;
	}

	//
	// ERROR
	//

	this->_value = "";
	this->_is_valid = false;
	return 2;
};


////////////////////////////////////////////////////////////////////////////////////////////
/**
		
*/   
////////////////////////////////////////////////////////////////////////////////////////////


std::string rus_passport_t::format(const int &format)
{
	switch(format)
	{
		case tegia::types::format::XX_XX_YYYYYY: return this->region + " " + this->series + " " + this->number; break;
		case tegia::types::format::XXXX_YYYYYY: return this->region + this->series + " " + this->number; break;
	}
	return "";
};


}	// END namespace types
}	// END namespace tegia
