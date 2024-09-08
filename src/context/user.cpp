#include <tegia/context/user.h>


namespace tegia {

/////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
/////////////////////////////////////////////////////////////////////////////////////////////


user::user():
	_status(0),
	_uuid("00000000-0000-0000-0000-000000000000"),
	_name(""),
	_surname(""),
	_patronymic(""),
	_gender(0),
	_photo(""),
	_ws("")
{
	this->_roles.set(ROLES::SESSION::SYSTEM);
	// std::cout << "create user" << std::endl; 
};


/////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
/////////////////////////////////////////////////////////////////////////////////////////////


user::~user()
{

};


/////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
/////////////////////////////////////////////////////////////////////////////////////////////


void user::role(int role, bool set)
{
	if(set == true)
	{
		this->_roles.set(role);
	}
	else
	{
		this->_roles.reset(role);
	}
};


/////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
/////////////////////////////////////////////////////////////////////////////////////////////


unsigned long long int user::role()
{
	return this->_roles.to_ullong();
};


/////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
/////////////////////////////////////////////////////////////////////////////////////////////


bool user::check(int role)
{
	return this->_roles[role];
};


/////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
/////////////////////////////////////////////////////////////////////////////////////////////


unsigned long long int user::match(unsigned long long int roles)
{
	return this->_roles.to_ullong() & roles;
};


/////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
/////////////////////////////////////////////////////////////////////////////////////////////


void user::print()
{
	std::cout << _YELLOW_ << "[tegia::user]" << _BASE_TEXT_ << std::endl;
	std::cout << "    uuid       = " << this->_uuid << std::endl;
	std::cout << "    status     = " << this->_status << std::endl;
	std::cout << "    surname    = " << this->_surname << std::endl;
	std::cout << "    name       = " << this->_name << std::endl;
	std::cout << "    patronymic = " << this->_patronymic << std::endl;
	std::cout << "    gender     = " << this->_gender << std::endl;
	std::cout << "    ws         = " << this->_ws << std::endl;
	std::cout << "    roles      = " << this->_roles.to_string() << std::endl;


	if(this->_roles.test(ROLES::SESSION::SYSTEM) == true)
	{
		std::cout << "                 ROLES::SESSION::SYSTEM" << std::endl;
	}

	if(this->_roles.test(ROLES::SESSION::PUBLIC) == true)
	{
		std::cout << "                 ROLES::SESSION::PUBLIC" << std::endl;
	}

	if(this->_roles.test(ROLES::SESSION::USER) == true)
	{
		std::cout << "                 ROLES::SESSION::USER" << std::endl;
	}

	if(this->_roles.test(ROLES::GLOBAL::USER) == true)
	{
		std::cout << "                 ROLES::GLOBAL::USER" << std::endl;
	}

	if(this->_roles.test(ROLES::GLOBAL::BOT) == true)
	{
		std::cout << "                 ROLES::GLOBAL::BOT" << std::endl;
	}

	if(this->_roles.test(ROLES::GLOBAL::ADMIN) == true)
	{
		std::cout << "                 ROLES::GLOBAL::ADMIN" << std::endl;
	}

	if(this->_roles.test(ROLES::WS::MEMBER) == true)
	{
		std::cout << "                 ROLES::WS::MEMBER" << std::endl;
	}

	if(this->_roles.test(ROLES::WS::ADMIN) == true)
	{
		std::cout << "                 ROLES::WS::ADMIN" << std::endl;
	}

	if(this->_roles.test(ROLES::WS::OWNER) == true)
	{
		std::cout << "                 ROLES::WS::OWNER" << std::endl;
	}

};


/////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
/////////////////////////////////////////////////////////////////////////////////////////////


nlohmann::json user::json()
{
	nlohmann::json tmp;

	tmp["uuid"] = this->_uuid;
	tmp["status"] = this->_status;
	tmp["surname"] = this->_surname;
	tmp["name"] = this->_name;
	tmp["patronymic"] = this->_patronymic;
	tmp["gender"] = this->_gender;
	tmp["ws"] = this->_ws;
	tmp["roles"] = this->_roles.to_ullong();

	return std::move(tmp);
};


/////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
/////////////////////////////////////////////////////////////////////////////////////////////


std::string user::uuid()
{
	return this->_uuid;
};


/////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
/////////////////////////////////////////////////////////////////////////////////////////////


std::string user::ws()
{
	return this->_ws;
};


/////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
/////////////////////////////////////////////////////////////////////////////////////////////


int user::status()
{
	return this->_status;
};


/////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
/////////////////////////////////////////////////////////////////////////////////////////////

/*
unsigned long long int user::roles(int count, ...)
{
    va_list args;
    va_start(args, count);
    
	std::bitset<64> _roles{0};

    for (int i = 0; i < count; ++i) 
	{
		auto bit = va_arg(args, int);

		std::cout << "[" << i << "] [" << bit << "]" << std::endl;

		_roles.set(bit);
    }
    
    va_end(args);
    return _roles.to_ullong();	
};
*/

}	// END NAMESPACE tegia 

