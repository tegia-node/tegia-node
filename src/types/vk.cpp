#include <tegia/types/vk.h>
#include <tegia/core/cast.h>
#include <regex>

namespace tegia {
namespace types {


////////////////////////////////////////////////////////////////////////////////////////////
/**
		
*/   
////////////////////////////////////////////////////////////////////////////////////////////


vk_t::vk_t():base_t("vk_id")
{
	
};


////////////////////////////////////////////////////////////////////////////////////////////
/**
		
*/   
////////////////////////////////////////////////////////////////////////////////////////////


std::string vk_t::value() const
{
	return core::cast<std::string>(this->_id);
};


////////////////////////////////////////////////////////////////////////////////////////////
/**
		
*/   
////////////////////////////////////////////////////////////////////////////////////////////


std::string vk_t::hash() const
{
	return tegia::crypt::MD5u(core::cast<std::string>(this->_id));
};


////////////////////////////////////////////////////////////////////////////////////////////
/**
		
*/   
////////////////////////////////////////////////////////////////////////////////////////////


nlohmann::json vk_t::json() const
{
	nlohmann::json tmp;
	tmp["vk"] = this->_id;

	return std::move(tmp);
};


////////////////////////////////////////////////////////////////////////////////////////////
/**
		
*/   
////////////////////////////////////////////////////////////////////////////////////////////


int vk_t::parse(const std::string &value, const nlohmann::json &validate)
{
	// std::cout << value << std::endl;
	
	if(value == "")
	{
		this->_id = 0;
		this->_link = "";
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
			this->_link = value;
			this->_id = core::cast<long long int>(_match[0].str().substr(2));
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
			this->_link = value;
			this->_id = core::cast<long long int>(_match[0].str());
			this->_is_valid = true;
			return 1;
		}
	}


	// TODO: Делать запрос к ВК

	this->_link = "";
	this->_id = 0;
	
	std::cout << _ERR_TEXT_ <<  std::endl;
	std::cout << this->_link << std::endl;
	std::cout << "not found pattern '" << R"(id([0-9]+)$)" << "'" << std::endl;

	this->_is_valid = false;
	return 2;
};





bool vk_t::set_id(long long int _id)
{
	if(_id > 0)
	{
		this->_id = _id;
		this->_is_valid = true;
		return true;
	}
	else
	{
		this->_id = 0;
		this->_is_valid = false;
		return false;
	}
};



}	// END namespace identifier
}	// END namespace tegia
