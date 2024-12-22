#ifndef H_TEGIA_TYPES_SIMPLE
#define H_TEGIA_TYPES_SIMPLE

#include <string>

#include <tegia/core/const.h>
#include <tegia/core/json.h>
#include <tegia/core/crypt.h>

namespace tegia {
namespace types {

namespace format {
	const int XX_XX_YYYYYY	= 1;
	const int XXXX_YYYYYY	= 2;
}

enum class SIMPLE_TYPE: long long int 
{
	STRING              = 2663297705,    // crc32("string")

	EMAIL               = 3885137012,    // crc32("email")
	PHONE               = 1146066909,    // crc32("phone")

	GENDER              = 3343321666,    // crc32("gender")
	DATE                = 2862495610,    // crc32("date")

	FACEBOOK_ID         = 2615737752,    // crc32("facebook_id")
	VK                  = 3806098527,    // crc32("vk")
	VK_ID               = 3315043922,    // crc32("vk_id")
	VK_LINK             = 3712801537,    // crc32("vk_link")

	RUS_PASSPORT        = 1556358888,    // crc32("rus_passport")
	RUS_INN             = 2249915708,    // crc32("rus_inn")
	RUS_SNILS           = 3840612784,    // crc32("rus_snils")
};


//
//
//


class simple_t
{
	protected:
		std::string _type;
		std::string _value;
		bool _is_valid = false;
		long long int _code = 0;

	public:	
		simple_t() = delete;
		simple_t(std::string type):_type(type) {};
		virtual ~simple_t() = default;


		std::string value() const
		{
			return this->_value;
		};

		std::string hash() const
		{
			return tegia::crypt::MD5u(this->_value);
		};

		std::string type() const
		{
			return this->_type;
		};

		bool is_valid() const
		{
			return this->_is_valid;
		};

		long long int code() const
		{
			return this->_code;
		};

		virtual int parse(
			const std::string &value, 
			const nlohmann::json &validate = nlohmann::json::object()
		) = 0;

};

//
//
//


/*
tegia::types::simple_t * parse(
	const std::string &type,
	const std::string &value)
{

};
*/



}	// END namespace types
}	// END namespace tegia


#endif