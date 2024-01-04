#ifndef H_TEGIA_CORE_CONTACTS_BASE
#define H_TEGIA_CORE_CONTACTS_BASE


#include <iostream>


namespace tegia {


enum class TYPES: long long int { 
	OTHER = 3646436640,         // crc32("other")
	PHONE = 1146066909,         // crc32("phone")
	VK_LINK =  3712801537       // crc32("vk_link")
};


namespace contacts {



#define _OTHER_          3646436640      // crc32("other")
#define _EMAIL_          3885137012      // crc32("email")
#define _PHONE_          1146066909      // crc32("phone")

#define _FACEBOOK_ID_    2615737752      // crc32("facebook_id")

#define _VK_ID_          3315043922      // crc32("vk_id")
#define _VK_LINK_        3712801537      // crc32("vk_link")





class contact_t
{
	protected:
		// std::string _value;
		std::string _type;
		bool _is_valid = false;

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

		virtual bool is_valid() final
		{
			return this->_is_valid;
		};

};


}	// END namespace contacts
}	// END namespace tegia


#endif