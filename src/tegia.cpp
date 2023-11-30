
#include <tegia2/tegia.h>
#include "node/node.h"

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
	int priority)
{
	auto message = std::make_shared<message_t>(std::move(data));
	return tegia::node::node::instance()->send_message(name,action,message,priority);
};


int send(
	const std::string &name, 
	const std::string &action, 
	const std::shared_ptr<message_t> &message,
	int priority)
{
	return tegia::node::node::instance()->send_message(name,action,message,priority);
};



} // namespace message
} // namespace tegia


