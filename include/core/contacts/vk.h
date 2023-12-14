#ifndef H_TEGIA_CORE_CONTACTS_VK
#define H_TEGIA_CORE_CONTACTS_VK


#include <iostream>

#include <tegia2/core/const.h>
#include <tegia2/core/contacts/contact.h>


namespace tegia {
namespace contacts {

class vk_t: public tegia::contacts::contact_t
{
	protected:
		bool is_format = false;
		std::string _link;
		std::string _id;

	public:
		vk_t();
		virtual ~vk_t() = default;

		std::string value() const override final;

		bool set_link(const std::string &_value);
		bool set_id(const std::string &_value);
};


}	// END namespace contacts
}	// END namespace tegia


#endif