#include <tegia/types/snils.h>
#include <regex>

namespace tegia {
namespace types {


////////////////////////////////////////////////////////////////////////////////////////////
/**
		
*/   
////////////////////////////////////////////////////////////////////////////////////////////


snils_t::snils_t():base_t("snils")
{
	
};


////////////////////////////////////////////////////////////////////////////////////////////
/**
		
*/   
////////////////////////////////////////////////////////////////////////////////////////////


std::string snils_t::value() const
{
	return this->_snils;
};


////////////////////////////////////////////////////////////////////////////////////////////
/**
		
*/   
////////////////////////////////////////////////////////////////////////////////////////////


std::string snils_t::hash() const
{
	return tegia::crypt::MD5u(this->_snils);
};


////////////////////////////////////////////////////////////////////////////////////////////
/**
		
*/   
////////////////////////////////////////////////////////////////////////////////////////////


nlohmann::json snils_t::json() const
{
	nlohmann::json tmp;
	tmp["snils"] = this->_snils;

	return std::move(tmp);
};

nlohmann::json snils_t::graph() const
{
	nlohmann::json tmp;
	tmp["type"] = "snils";
	tmp["value"] = this->_snils;

	return std::move(tmp);
};


////////////////////////////////////////////////////////////////////////////////////////////
/**
		
	0 - empty
	1 - ok
	2 - not valid

*/   
////////////////////////////////////////////////////////////////////////////////////////////


int snils_t::parse(const std::string & value, const nlohmann::json &validate)
{
	if(value == "")
	{
		this->_snils = value;
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
		this->_snils = "";
		this->_is_valid = false;
		return 0;		
	}

	if(__snils.length() != 11)
	{
		this->_snils = "";
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

	this->_snils = __snils;
	this->_is_valid = true;
	return 1;
};



}	// END namespace types
}	// END namespace tegia
