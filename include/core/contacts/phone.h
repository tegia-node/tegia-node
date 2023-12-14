#ifndef H_TEGIA_CORE_CONTACTS_PHONE
#define H_TEGIA_CORE_CONTACTS_PHONE


#include <iostream>
#include <tegia2/core/contacts/contact.h>


namespace tegia {
namespace contacts {

class phone_t: public tegia::contacts::contact_t
{
	protected:
		bool is_format = false;
		std::string _phone;

	public:
		phone_t();
		phone_t(const std::string &_value);
		virtual ~phone_t() = default;

		std::string value() const override final;
		bool parse(const std::string &_value);
};


}	// END namespace contacts
}	// END namespace tegia


#endif