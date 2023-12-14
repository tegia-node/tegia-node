#include <string>
#include <regex>

#include <tegia2/core/contacts/vk.h>



namespace tegia {
namespace contacts {


///////////////////////////////////////////////////////////////////////////////////////////
/*

*/
///////////////////////////////////////////////////////////////////////////////////////////


vk_t::vk_t():contact_t("vk")
{
	
};



///////////////////////////////////////////////////////////////////////////////////////////
/*

*/
///////////////////////////////////////////////////////////////////////////////////////////

std::string vk_t::value() const
{
	return this->_id;
};


///////////////////////////////////////////////////////////////////////////////////////////
/*

*/
///////////////////////////////////////////////////////////////////////////////////////////


bool vk_t::set_link(const std::string &link)
{
	this->_link = link;

	//
	// Проверяем,что это прямая ссылка на ID
	//

	std::regex  _regex(R"(id[0-9]{1,})");
	std::smatch _match;
	if(std::regex_search(this->_link, _match, _regex))
	{
		this->_id = _match[0].str().substr(2);
		return true;
	}
	else
	{
		std::cout << _ERR_TEXT_ <<  std::endl;
		std::cout << this->_link << std::endl;
		std::cout << "not found pattern '" << R"(id([0-9]+)$)" << "'" << std::endl;
		return false;
	}
};


///////////////////////////////////////////////////////////////////////////////////////////
/*

*/
///////////////////////////////////////////////////////////////////////////////////////////


bool vk_t::set_id(const std::string &id)
{
	this->_id = id;
	return true;
};

///////////////////////////////////////////////////////////////////////////////////////////
/*

*/
///////////////////////////////////////////////////////////////////////////////////////////




}	// END namespace contacts
}	// END namespace tegia