#ifndef H_TEGIA_USER
#define H_TEGIA_USER
// --------------------------------------------------------------------

#include <string>
#include <iostream>
#include <bitset>
#include <cstdarg>

#include <tegia/core/const.h>
#include <tegia/core/json.h>


namespace tegia 
{
	class auth;
}

namespace tegia::threads
{
	class _data;
}

namespace tegia::actors
{
	class ws_t;
	class map_t;
}



namespace ROLES {

namespace WS // [20 - 63]
{
	const int OWNER  = 63;
	const int ADMIN  = 62;
	const int MEMBER = 20;
}

namespace GLOBAL // [10 - 19]
{
	const int ADMIN  = 12;
	const int BOT    = 11;
	const int USER   = 10;
}

namespace SESSION // [0 - 9]
{
	const int USER   = 2;
	const int PUBLIC = 1;
	const int SYSTEM = 0;
}

} // END namespace ROLES


namespace ID {
	class EMAIL;
}


namespace tegia {
class user
{
	friend class ::tegia::threads::_data;
	friend class ::tegia::auth;
	friend class ::tegia::actors::ws_t;
	friend class ::tegia::actors::map_t;

	friend class ::ID::EMAIL;


	private:
		int _status;

		std::string _uuid;
		std::string _name;
		std::string _surname;
		std::string _patronymic;
		std::string _photo;
		int _gender;

		std::string _ws;
		std::bitset<64> _roles{0};

		void role(int role, bool set);
		unsigned long long int role();

	public:
		user();
		~user();

		std::string uuid();
		std::string ws();
		int status();

		bool check(int role);
		unsigned long long int match(unsigned long long int roles);

		void print();
		nlohmann::json json();

		// static unsigned long long int roles(int count, ...);

		template<typename... Args>
		static unsigned long long int roles(Args... args) 
		{
			std::bitset<64> _roles{0};

			// Lambda to set bits and print values
			auto set_bit = [&](int bit) {
				// std::cout << "[" << _roles.count() << "] [" << bit << "]" << std::endl;
				_roles.set(bit);
			};

			// Fold expression to apply set_bit to each argument
			(set_bit(args), ...);

			return _roles.to_ullong();
		}

};	// END CLASS user
}	// END NAMESPACE tegia 



// --------------------------------------------------------------------
#endif