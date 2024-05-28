#include <tegia/types/rus_inn.h>
#include <regex>

namespace tegia {
namespace types {


////////////////////////////////////////////////////////////////////////////////////////////
/**
		
*/   
////////////////////////////////////////////////////////////////////////////////////////////


rus_inn_t::rus_inn_t():base_t("rus_inn")
{
	
};


////////////////////////////////////////////////////////////////////////////////////////////
/**
		
*/   
////////////////////////////////////////////////////////////////////////////////////////////


std::string rus_inn_t::value() const
{
	return this->_rus_inn;
};


////////////////////////////////////////////////////////////////////////////////////////////
/**
		
*/   
////////////////////////////////////////////////////////////////////////////////////////////


std::string rus_inn_t::hash() const
{
	return tegia::crypt::MD5u(this->_rus_inn);
};


////////////////////////////////////////////////////////////////////////////////////////////
/**
		
*/   
////////////////////////////////////////////////////////////////////////////////////////////


nlohmann::json rus_inn_t::json() const
{
	nlohmann::json tmp;
	tmp["inn"] = this->_rus_inn;

	return std::move(tmp);
};

nlohmann::json rus_inn_t::graph() const
{
	nlohmann::json tmp;
	tmp["type"] = "rus_inn";
	tmp["value"] = this->_rus_inn;

	return std::move(tmp);
};


////////////////////////////////////////////////////////////////////////////////////////////
/**
		
	0 - empty
	1 - ok
	2 - not valid

*/   
////////////////////////////////////////////////////////////////////////////////////////////


int rus_inn_t::parse(const std::string & value, const nlohmann::json &validate)
{
	if(value == "")
	{
		this->_rus_inn = value;
		this->_is_valid = false;
		return 0;
	}

	//
	// Выделяем только цифры
	//

	std::string __rus_inn = "";

	{
		std::regex r(R"([0-9])");

		for(std::sregex_iterator i = std::sregex_iterator(value.begin(), value.end(), r);
								i != std::sregex_iterator();
								++i)
		{
			std::smatch m = *i;
			__rus_inn = __rus_inn + m.str();
		}
	}

	//
	// Проверяем длину
	//

	if(__rus_inn == "")
	{
		this->_rus_inn = "";
		this->_is_valid = false;
		return 0;		
	}

	if(__rus_inn.length() != 12)
	{
		this->_rus_inn = "";
		this->_is_valid = false;
		return 2;
	}

	// 
	// FORMAT
	//

	this->_rus_inn = __rus_inn;
	this->_is_valid = true;
	return 1;
};



int rus_inn_t::code()
{
	return 3643001;
};


}	// END namespace types
}	// END namespace tegia
