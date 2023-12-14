#ifndef H_TEGIA_CORE_CONTACTS_BASE
#define H_TEGIA_CORE_CONTACTS_BASE


#include <iostream>


namespace tegia {
namespace contacts {


#define OTHER          3646436640      // crc32("other")
#define EMAIL          3885137012      // crc32("email")
#define PHONE          1146066909      // crc32("phone")

#define FACEBOOK_ID    2615737752      // crc32("facebook_id")

#define VK_ID          3315043922      // crc32("vk_id")
#define VK_LINK        3712801537      // crc32("vk_link")





class contact_t
{
	protected:
		// std::string _value;
		std::string _type;

	public:

		int status = 0;
		
		contact_t() = delete;

		contact_t(const std::string &type):_type(type) {};

		virtual ~contact_t() = default;

		virtual std::string value() const = 0;

		/*
		{
			return this->_value;
		};
		*/

		virtual std::string type() final
		{
			return this->_type;
		};
};


}	// END namespace contacts
}	// END namespace tegia


#endif