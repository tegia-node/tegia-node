#include "Actor1.h"


actor1::actor1(const std::string &name): tegia::actors::actor_base()
{
	this->type = ACTOR_TYPE; 
	this->name = name;	
};


actor1::~actor1()
{ };

std::string actor1::func1(const std::shared_ptr<message_t> &message, const nlohmann::json &route_params)
{
	std::cout << type << "::func1 " << message->data["test"].get<std::string>() << std::endl;
	std::cout << type << "::func1 " << route_params["__path"].get<std::string>() << std::endl;

	return "";
};


