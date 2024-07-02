#ifndef H_TEGIA_USER
#define H_TEGIA_USER
// --------------------------------------------------------------------

#include <string>
#include <iostream>
#include <bitset>

#include <tegia/core/const.h>
#include <tegia/core/json.h>


namespace tegia {

class auth;

namespace threads
{
	class _data;
}
}


namespace ROLES {

const int ADMIN = 63;
const int USER = 62;

} // END namespace ROLES


namespace tegia {
class user
{
	friend class ::tegia::threads::_data;
	friend class ::tegia::auth;


	private:
		std::string _uuid;
		int _status;

		std::string _name;
		std::string _surname;
		std::string _patronymic;
		std::string _photo;
		int _gender;

		std::bitset<64> _roles{0};

		void role(int role, bool set);
		unsigned long long int role();

	public:
		user();
		~user();
		std::string uuid();
		int status();

		bool check(int role);

		void print();
		nlohmann::json json();

};	// END CLASS user
}	// END NAMESPACE tegia 



// --------------------------------------------------------------------
#endif