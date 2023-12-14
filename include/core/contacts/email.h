#ifndef H_TEGIA_CORE_CONTACTS_EMAIL
#define H_TEGIA_CORE_CONTACTS_EMAIL


#include <iostream>
#include <tegia2/core/contacts/contact.h>


namespace tegia {
namespace contacts {

class email_t: public tegia::contacts::contact_t
{
	protected:
		bool is_format = false;
		std::string _email;

	public:
		email_t();
		email_t(const std::string &_value);
		virtual ~email_t() = default;

		std::string value() const override final;
		bool parse(const std::string &_value);
};


}	// END namespace contacts
}	// END namespace tegia


#endif