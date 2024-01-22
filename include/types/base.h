#ifndef H_TEGIA_TYPES_BASE
#define H_TEGIA_TYPES_BASE

#include <string>

#include <tegia2/core/const.h>
#include <tegia2/core/json.h>
#include <tegia2/core/crypt.h>

namespace tegia {
namespace types {

//
//
//

enum class TYPE: long long int { 
	OTHER        = 3646436640,    // crc32("other")
	EMAIL        = 3885137012,    // crc32("email")
	PHONE        = 1146066909,    // crc32("phone")

	GENDER       = 3343321666,    // crc32("gender")
	DATE         = 2862495610,    // crc32("date")

	FACEBOOK_ID  = 2615737752,    // crc32("facebook_id")
	VK_ID        = 3315043922,    // crc32("vk_id")
	VK_LINK      = 3712801537     // crc32("vk_link")
};

//
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

		virtual std::string type() final
		{
			return this->_type;
		};

		virtual bool is_valid() final
		{
			return this->_is_valid;
		};
};

}	// END namespace types
}	// END namespace tegia


#endif