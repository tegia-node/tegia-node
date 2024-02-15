#include <tegia/types/rus_passport.h>
#include <regex>

namespace tegia {
namespace types {


////////////////////////////////////////////////////////////////////////////////////////////
/**
		
*/   
////////////////////////////////////////////////////////////////////////////////////////////


rus_passport_t::rus_passport_t():base_t("rus_passport")
{
	
};


////////////////////////////////////////////////////////////////////////////////////////////
/**
		
*/   
////////////////////////////////////////////////////////////////////////////////////////////


std::string rus_passport_t::value() const
{
	return this->_rus_passport;
};


////////////////////////////////////////////////////////////////////////////////////////////
/**
		
*/   
////////////////////////////////////////////////////////////////////////////////////////////


std::string rus_passport_t::hash() const
{
	return tegia::crypt::MD5(this->_rus_passport);
};


////////////////////////////////////////////////////////////////////////////////////////////
/**
		
*/   
////////////////////////////////////////////////////////////////////////////////////////////


nlohmann::json rus_passport_t::json() const
{
	nlohmann::json tmp;
	tmp["rus_passport"] = this->_rus_passport;

	return std::move(tmp);
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
		this->_rus_passport = value;
		this->_is_valid = false;
		return 0;
	}

	//
	// Выделяем только цифры
	//

	std::string __rus_passport = "";

	{
		std::regex r(R"([0-9])");

		for(std::sregex_iterator i = std::sregex_iterator(value.begin(), value.end(), r);
								i != std::sregex_iterator();
								++i)
		{
			std::smatch m = *i;
			__rus_passport = __rus_passport + m.str();
		}
	}

	//
	// Проверяем длину
	//

	if(__rus_passport.length() != 10)
	{
		this->_rus_passport = "";
		this->_is_valid = false;
		return 2;
	}

	// 
	// FORMAT
	//

	/*
		ХХХХ YYYYYY
	*/

	this->_rus_passport = __rus_passport.substr(0,4) + " " + __rus_passport.substr(4,6);;
	this->_is_valid = true;
	return 1;
};



}	// END namespace types
}	// END namespace tegia
