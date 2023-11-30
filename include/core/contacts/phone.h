#ifndef H_TEGIA_CORE_CONTACTS_PHONE
#define H_TEGIA_CORE_CONTACTS_PHONE


#include <iostream>

namespace tegia {
namespace contacts {

class phone_t
{
	private:
		std::string value;
		bool is_format = false;

	public:
		phone_t() = default;
		phone_t(const std::string &_value);
		~phone_t() = default;

		bool parse(const std::string &_value);
		std::string get();
};


}	// END namespace contacts
}	// END namespace tegia


#endif