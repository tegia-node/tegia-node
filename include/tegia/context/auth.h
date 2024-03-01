#ifndef H_TEGIA_AUTH
#define H_TEGIA_AUTH

#include <tegia/context/user.h>
#include <tegia/context/jwt.h>

namespace tegia {

class auth
{
	public:

		static ::tegia::user * test();
		static std::string key(const std::string &name);

		static ::tegia::jwt * generate_jwt(::tegia::user * user, int type, const std::string &priv_key);
		static ::tegia::jwt * unpack(const std::string &token, const std::string &pub_key);
};

}	// END namespace tegia



#endif