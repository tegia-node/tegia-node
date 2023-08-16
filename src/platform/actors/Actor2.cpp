#include "Actor2.h"


actor2::actor2(const std::string &name): tegia::actors::actor_base()
{
	this->type = ACTOR_TYPE; 
	this->name = name;	
};


actor2::~actor2()
{ };


std::string actor2::func2(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params)
{
	std::cout << type << "::func2 " << message->data["test"].get<std::string>() << std::endl;
	std::cout << type << "::func2 " << route_params["__path"].get<std::string>() << std::endl;

	return "";
};


