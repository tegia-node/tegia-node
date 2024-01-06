#include <string>
#include <regex>

#include <tegia2/core/contacts/phone.h>



namespace tegia {
namespace contacts {


///////////////////////////////////////////////////////////////////////////////////////////
/*

*/
///////////////////////////////////////////////////////////////////////////////////////////

phone_t::phone_t():contact_t("phone")
{
	
};


phone_t::phone_t(const std::string &_value):contact_t("phone")
{
	this->parse(_value);
};


///////////////////////////////////////////////////////////////////////////////////////////
/*

*/
///////////////////////////////////////////////////////////////////////////////////////////


std::string phone_t::value() const
{
	return this->_phone;
};


///////////////////////////////////////////////////////////////////////////////////////////
/*

*/
///////////////////////////////////////////////////////////////////////////////////////////


bool phone_t::parse(const std::string &value)
{
	//
	// Выделяем только цифры
	//

	if(value == "")
	{
		this->_phone = value;
		this->is_format = false;
		return false;
	}

	std::string _phone = "";

	{
		std::regex r(R"([0-9])");

		for(std::sregex_iterator i = std::sregex_iterator(value.begin(), value.end(), r);
								i != std::sregex_iterator();
								++i)
		{
			std::smatch m = *i;
			_phone = _phone + m.str();
		}
	}

	//
	// RUS_PHONE
	//

	{
		std::regex  _regex(R"(^7[0-9]{10}$)");
		std::smatch _match;
		if(std::regex_match(_phone, _match, _regex))
		{
			// return std::move(_phone);
			this->_phone = _phone;
			this->is_format = true;
			return true;
		}
	}

	{
		std::regex  _regex(R"(^8[0-9]{10}$)");
		std::smatch _match;
		if(std::regex_match(_phone, _match, _regex))
		{
			_phone = "7" + _phone.substr(1);

			// std::cout << phone << std::endl;
			// std::cout << _phone << std::endl;
			// exit(0);

			// return std::move(_phone);
			this->_phone = _phone;
			this->is_format = true;
			return true;
		}
	}

	//
	//
	//

	// return std::move(_phone);
	this->_phone = _phone;
	this->is_format = false;
	return false;
};


///////////////////////////////////////////////////////////////////////////////////////////
/*

*/
///////////////////////////////////////////////////////////////////////////////////////////




}	// END namespace contacts
}	// END namespace tegia