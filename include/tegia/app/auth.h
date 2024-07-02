#ifndef H_TEGIA_AUTH
#define H_TEGIA_AUTH

#include <tegia/context/user.h>
#include <tegia/context/jwt.h>

namespace tegia {

class auth
{
	public:

		static ::tegia::user * test(
			const std::string _uuid = "204d01a6-5922-4be1-a375-3c0342f973f7",
			const std::string _surname = "Горячев",
			const std::string _name = "Игорь",
			const std::string _patronymic = "Александрович",
			int _gender = 1,
			unsigned long long int roles = 0
		);

		static std::string key(const std::string &name);

		static ::tegia::jwt * generate_jwt(::tegia::user * user, int type, const std::string &priv_key);
		static ::tegia::jwt * unpack(const std::string &token, const std::string &pub_key);
};

}	// END namespace tegia



#endif