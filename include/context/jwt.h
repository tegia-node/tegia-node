#ifndef _H_TEGIA_JWT_
#define _H_TEGIA_JWT_


#include <bitset>

#include <tegia/const.h>
#include <tegia/core/core.h>
#include <tegia/core/json.h>

class A2JWT;

namespace tegia {
namespace threads {

class data;
class context;

}
}

class JWT
{
	friend class tegia::threads::context;
	friend class tegia::threads::data;
	friend class A2JWT;

	private:
		std::string uuid;
		
		std::string name;
		std::string surname;
		std::string patronymic;
		int sex;
		std::string photo;
		nlohmann::json roles;

		std::bitset<64> _roles{0};

		std::string _t_create;
		int status;	

		int type;	// тип токена 
					//   1 - клиенский токен
					//   2 - API токен
		std::string ip;				

		std::string token;

	public:

		JWT()
		{	
			this->uuid = "00000000-0000-0000-0000-000000000000";
			this->name = "";
			this->surname = "root";
			this->patronymic = "";
			this->sex = 1;
			this->photo = "";
			this->roles = nlohmann::json::array();
			this->_t_create = "";
			this->status = 0;
			this->type = 0;
			this->ip = "";
			this->token = "";
		};
		~JWT(){};

		void print()
		{
			std::cout << _OK_TEXT_ << "JWT" << std::endl;
			std::cout << "UUID:       " << this->uuid << std::endl;
			std::cout << "status:     " << this->status << std::endl;
			std::cout << "name:       " << this->name << std::endl;
			std::cout << "surname:    " << this->surname << std::endl;
			std::cout << "patronymic: " << this->patronymic << std::endl;
			std::cout << "sex:        " << this->sex << std::endl;
			std::cout << "photo:      " << this->photo << std::endl;
			std::cout << "roles:      " << this->_roles.to_ullong() << std::endl;
		};

		int get_type()
		{
			return this->type;
		};

		std::string get_token()
		{
			return this->token;
		};

		int get_status()
		{
			return this->status;
		};

		bool check_ip(const std::string &ip)
		{
			if(this->ip == ip)
			{
				return true;
			}
			return false;
		};


		nlohmann::json json();
		int pack(const std::string &priv_key);
		int unpack(const std::string &tegia_token, const std::string &pub_key);
};

typedef std::shared_ptr<JWT> jwt_ptr;


#endif