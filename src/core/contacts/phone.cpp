#include <string>
#include <regex>

#include <tegia2/core/contacts/phone.h>



namespace tegia {
namespace contacts {


///////////////////////////////////////////////////////////////////////////////////////////
/*

*/
///////////////////////////////////////////////////////////////////////////////////////////


phone_t::phone_t(const std::string &_value)
{
	this->parse(_value);
};


///////////////////////////////////////////////////////////////////////////////////////////
/*

*/
///////////////////////////////////////////////////////////////////////////////////////////


bool phone_t::parse(const std::string &_value)
{
	//
	// Выделяем только цифры
	//

	std::string _phone = "";

	{
		std::regex r(R"([0-9])");

		for(std::sregex_iterator i = std::sregex_iterator(_value.begin(), _value.end(), r);
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
			this->value = _phone;
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
			this->value = _phone;
			this->is_format = true;
			return true;
		}
	}

	//
	//
	//

	// return std::move(_phone);
	this->value = _phone;
	this->is_format = false;
	return false;
};


///////////////////////////////////////////////////////////////////////////////////////////
/*

*/
///////////////////////////////////////////////////////////////////////////////////////////


std::string phone_t::get()
{
	return this->value;
};


}	// END namespace contacts
}	// END namespace tegia