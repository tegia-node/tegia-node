#include <iostream>

#include <tegia/context/jwt.h>

namespace tegia {

jwt::jwt(){};
jwt::~jwt(){};

void jwt::print()
{
	std::cout << _OK_TEXT_ << "tegai::jwt" << std::endl;
	std::cout << "status:    " << this->_status << std::endl;
	std::cout << "t_create:  " << this->_t_create << std::endl;
	std::cout << "type:      " << this->_type << std::endl;
	std::cout << "ip:        " << this->_ip << std::endl;
	std::cout << "token:     " << this->_token << std::endl;
};

int jwt::type()
{
	return this->_type;
};

std::string jwt::token()
{
	return this->_token;
};

int jwt::status()
{
	return this->_status;
};

int jwt::pack(const std::string &priv_key)
{
	return 0;
};

int unpack(const std::string &tegia_token, const std::string &pub_key)
{
	return 0;
};


}	// END namespace tegia

