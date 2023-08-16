#include <tegia/core/documents.h>


namespace tegia {
namespace documents {


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// ПАСПОРТ ГРАЖДАНИНА РОССИЙСКОЙ ФЕДЕРАЦИИ
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::string s_rus_passport::format(const int &format)
{
	switch(format)
	{
		case XX_XX_YYYYYY:	return region + " " + series + " " + number;	break;
		case XXXX_YYYYYY:	return region + series + " " + number;			break;
	}
	return "";
}; 

bool s_rus_passport::_parse(const std::u32string &number)	
{
	std::u32string _number = number;
	_number.erase(std::remove(_number.begin(), _number.end(), ' '),  _number.end());
	_number.erase(std::remove(_number.begin(), _number.end(), '.'),  _number.end());
	_number.erase(std::remove(_number.begin(), _number.end(), ','),  _number.end());
	_number.erase(std::remove(_number.begin(), _number.end(), '-'),  _number.end());
	_number.erase(std::remove(_number.begin(), _number.end(), '\n'), _number.end());
	_number.erase(std::remove(_number.begin(), _number.end(), '\r'), _number.end());
	_number.erase(std::remove(_number.begin(), _number.end(), u'№'), _number.end());

	_number.erase(std::remove(_number.begin(), _number.end(), u'Н'), _number.end());
	_number.erase(std::remove(_number.begin(), _number.end(), u'О'), _number.end());
	_number.erase(std::remove(_number.begin(), _number.end(), u'М'), _number.end());
	_number.erase(std::remove(_number.begin(), _number.end(), u'Е'), _number.end());
	_number.erase(std::remove(_number.begin(), _number.end(), u'Р'), _number.end());

	const std::regex txt_regex("[0-9]{4,10}");
	std::smatch reg_match;

	std::string tmp = core::cast<std::string>(_number);
	if(std::regex_search( tmp, reg_match, txt_regex) == true)
	{
		std::string _tmp = reg_match[0];

		switch(_tmp.length())
		{
			case 9: _tmp = "0" + _tmp; break;
			case 8: _tmp = "00" + _tmp; break;
			case 7: _tmp = "000" + _tmp; break;
			case 6: _tmp = "0000" + _tmp; break;
			case 5: _tmp = "00000" + _tmp; break;
			case 4: _tmp = "000000" + _tmp; break;
		}

		this->region = _tmp.substr(0,2);
		this->series = _tmp.substr(2,2);
		this->number = _tmp.substr(4,6);
		
		return true;
	}
	else
	{
		return false;
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// СНИЛС
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool snils_t::parse(const std::string &_number)
{
	if(_number.length() == 11)
	{
		number = _number;
		is_valid = true;
	}
	return is_valid;
};

std::string snils_t::format()
{
	if(is_valid == true)
	{
		std::string ret = "";

		// std::cout << number.substr(0,3) << std::endl;
		// std::cout << number.substr(3,3) << std::endl;
		// std::cout << number.substr(6,3) << std::endl;
		// std::cout << number.substr(9,2) << std::endl;

		ret = number.substr(0,3) + "-" + number.substr(3,3) + "-" + number.substr(6,3) + " " + number.substr(9,2);
		
		return ret;
	}

	return "";
};



}  // END namespace documents
}  // END namespace tegia


