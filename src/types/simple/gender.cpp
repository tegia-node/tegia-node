#include <tegia/types/simple/gender.h>

namespace tegia {
namespace types {


////////////////////////////////////////////////////////////////////////////////////////////
/**
		
*/   
////////////////////////////////////////////////////////////////////////////////////////////


gender_t::gender_t():simple_t("gender")
{
	
};


////////////////////////////////////////////////////////////////////////////////////////////
/**
		
	0 - empty
	1 - ok
	2 - not valid

*/   
////////////////////////////////////////////////////////////////////////////////////////////


int gender_t::parse(const std::string & value, const nlohmann::json &validate)
{
	long long int crc32_gender = tegia::crypt::crc32(value);
	switch(crc32_gender)
	{
		//
		// ERROR
		//

		case 4108050209: // '0'
		case 808273962:  // '-1'
		case 2837715344: // '-2'
		{
			this->_value = "0";
			this->_is_valid = false;
			return 2;
		}
		break;

		//
		// EMPTY
		//

		case 0:          // ''
		case 3916222277: // ' '
		{
			this->_value = "0";
			this->_is_valid = false;
			return 0;
		}
		break;

		//
		// MALE
		//

		case 2724537472: // 'Мужчина'
		case 3236906023: // 'Мужской'
		case 2212294583: // '1'
		case 3664761504: // 'M'
		case 962070671:  // 'М' (киррилица)
		{
			this->_value = "1";
			this->_is_valid = true;
			return 1;
		}
		break;

		//
		// FEMALE
		//

		case 287992342:  // 'Женщина'
		case 655494494:  // 'Женский'
		case 450215437:  // '2'
		case 1304234792: // 'F'
		case 3649954193: // 'Ж'
		{
			this->_value = "2";
			this->_is_valid = true;
			return 1;
		}
		break;

		//
		// ERROR
		//

		default:
		{
			std::cout << _ERR_TEXT_ << std::endl;
			std::cout << "gender = '" << value << "'" << std::endl;
			std::cout << "crc32  = " << crc32_gender << std::endl;
			exit(0);
		}
		break;
	}
};



}	// END namespace types
}	// END namespace tegia
