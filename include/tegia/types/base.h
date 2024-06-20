#ifndef H_TEGIA_TYPES_BASE
#define H_TEGIA_TYPES_BASE

#include <string>

#include <tegia/core/const.h>
#include <tegia/core/json.h>
#include <tegia/core/crypt.h>

namespace tegia {
namespace types {

//
//
//

const long long int OTHER        = 3646436640;  // crc32("other")
const long long int EMAIL        = 3885137012;  // crc32("email")
const long long int PHONE        = 1146066909;  // crc32("phone")

const long long int PERSON       = 886886774;   // crc32("person")
const long long int GENDER       = 3343321666;  // crc32("gender")
const long long int DATE         = 2862495610;  // crc32("date")

const long long int FACEBOOK_ID  = 2615737752;  // crc32("facebook_id")
const long long int VK_ID        = 3315043922;  // crc32("vk_id")
const long long int VK_LINK      = 3712801537;  // crc32("vk_link")

const long long int RUS_PASSPORT = 1556358888;  // crc32("rus_passport")
const long long int RUS_INN      = 2249915708;  // crc32("rus_inn")
const long long int SNILS        = 3840612784;  // crc32("snils")


enum class TYPE: long long int { 
	OTHER        = 3646436640,    // crc32("other")

	STRING       = 2663297705,    // crc32("string")

	EMAIL        = 3885137012,    // crc32("email")
	PHONE        = 1146066909,    // crc32("phone")

	PERSON       = 886886774,     // crc32("person")
	GENDER       = 3343321666,    // crc32("gender")
	DATE         = 2862495610,    // crc32("date")

	FACEBOOK_ID  = 2615737752,    // crc32("facebook_id")
	VK_ID        = 3315043922,    // crc32("vk_id")
	VK_LINK      = 3712801537,    // crc32("vk_link")

	RUS_PASSPORT = 1556358888,    // crc32("rus_passport")
	RUS_INN      = 2249915708,    // crc32("rus_inn")
	SNILS        = 3840612784     // crc32("snils")
};

//;
//
//

class base_t
{
	protected:
		std::string _type;
		bool _is_valid = false;

	public:	
		base_t() = delete;
		base_t(std::string type):_type(type) {};
		virtual ~base_t() = default;

		virtual std::string value() const = 0;
		virtual std::string hash() const = 0;
		virtual nlohmann::json json() const = 0;

		virtual std::string type()
		{
			return this->_type;
		};

		virtual bool is_valid()
		{
			return this->_is_valid;
		};
};

}	// END namespace types
}	// END namespace tegia


#endif