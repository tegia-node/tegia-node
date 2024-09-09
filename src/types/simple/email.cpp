#include <tegia/types/simple/email.h>

namespace tegia {
namespace types {


////////////////////////////////////////////////////////////////////////////////////////////
/**
		
*/   
////////////////////////////////////////////////////////////////////////////////////////////


email_t::email_t():simple_t("email")
{
	
};


////////////////////////////////////////////////////////////////////////////////////////////
/**
		
	0 - empty
	1 - ok
	2 - not valid

*/   
////////////////////////////////////////////////////////////////////////////////////////////


int email_t::parse(const std::string & value, const nlohmann::json &validate)
{

	if(value == "")
	{
		this->_value = value;
		this->_is_valid = false;
		return 0;
	}

	// TODO: Хотя бы минимальную валидацию

	this->_value = value;

	//
	// (at) -> @
	//

	auto pos = value.find("(at)");
	if(pos != std::string::npos)
	{
		this->_value = value.substr(0,pos) + "@" + value.substr(pos+4);
	}
	
	std::transform(
		this->_value.begin(), 
		this->_value.end(), 
		this->_value.begin(),
		[](unsigned char c){ return std::tolower(c); }
	);
	
	this->_is_valid = true;
	return 1;
};



}	// END namespace types
}	// END namespace tegia
