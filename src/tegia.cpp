
#include <tegia/tegia.h>
#include "node/node.h"
#include "threads/data.h"

namespace tegia {
namespace actors {




} // namespace actors
} // namespace tegia



namespace tegia {
namespace conf {

std::string path(const std::string &name)
{
	// return tegia::node::node::instance()->config_path(name);
	return tegia::threads::data->node()->config_path(name);
};


const nlohmann::json * const get(const std::string &name)
{
	return tegia::threads::data->node()->config(name);
};


} // namespace conf
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
	// return tegia::node::node::instance()->send_message(name,action,message,priority);
	return tegia::threads::data->node()->send_message(name,action,message,priority);
};


int send(
	const std::string &name, 
	const std::string &action, 
	const std::shared_ptr<message_t> &message,
	int priority)
{
	// return tegia::node::node::instance()->send_message(name,action,message,priority);
	return tegia::threads::data->node()->send_message(name,action,message,priority);
};



} // namespace message
} // namespace tegia


