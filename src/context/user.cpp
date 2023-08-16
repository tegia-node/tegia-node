#include <tegia2/context/user.h>



namespace tegia {
user::user()
{
	this->_uuid = "00000000-0000-0000-0000-000000000000";
	this->_name = "";
	this->_surname = "root";
	this->_patronymic = "";
	this->_sex = 1;
	this->_photo = "";
	this->_status = 0;
};


user::~user(){};


std::string user::uuid()
{
	return this->_uuid;
};


}	// END NAMESPACE tegia 

