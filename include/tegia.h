#ifndef H_TEGIA
#define H_TEGIA


#include <iostream>
#include <tegia2/core/json.h>
#include <tegia2/actors/types.h>
#include <tegia2/context/context.h>


namespace tegia {
namespace actors {




} // namespace actors
} // namespace tegia



namespace tegia {
namespace message {


int send(
	const std::string &name, 
	const std::string &action, 
	nlohmann::json data,
	int priority = 0);

int send(
	const std::string &name, 
	const std::string &action, 
	const std::shared_ptr<message_t> &message,
	int priority = 0);


bool init()
{
	return false;
};


} // namespace message
} // namespace tegia



#endif