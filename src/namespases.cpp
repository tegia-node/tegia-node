
#include <tegia/namespaces.h>

#include "node/node.h"
#include "threads/data.h"

#include <tegia/actors/actor.h>
#include <tegia/actors/type.h>


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

int message::send(
	const std::string &name, 
	const std::string &action, 
	nlohmann::json data,
	int priority)
{
	auto _message = tegia::message::init(std::move(data));
	return tegia::threads::data->node()->send_message(name,action,_message,priority);
};


int message::send(
	const std::string &name, 
	const std::string &action, 
	const std::shared_ptr<message_t> &message,
	int priority)
{
	return tegia::threads::data->node()->send_message(name,action,message,priority);
};

int message::resolve(const std::string &name)
{
	return tegia::threads::data->node()->resolve(name);
};

} // namespace tegia


