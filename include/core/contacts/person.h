#ifndef H_TEGIA_CORE_CONTACTS_PERSON
#define H_TEGIA_CORE_CONTACTS_PERSON


#include <iostream>

#include <tegia2/core/const.h>
#include <tegia2/core/json.h>
#include <tegia2/core/contacts/contact.h>


namespace tegia {
namespace contacts {

class person_t: public tegia::contacts::contact_t
{
	protected:
		std::string surname;
		std::string name;
		std::string patronymic;
		std::string t_birth;
		int gender;

	public:
		person_t();
		virtual ~person_t() = default;
		
		std::string value() const override final;
		void parse(nlohmann::json * data);

};


}	// END namespace contacts
}	// END namespace tegia


#endif