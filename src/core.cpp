
#include <tegia/core.h>
#include <tegia/actors/actor.h>
#include <tegia/actors/type.h>

#include "node/node.h"




namespace tegia {
namespace conf {

std::string path(const std::string &name)
{
	return tegia::threads::thread->node()->config_path(name);
};


const nlohmann::json * const get(const std::string &name)
{
	return tegia::threads::thread->node()->config(name);
};


} // namespace conf
} // namespace tegia


//
//
//

namespace tegia {
namespace configuration {

const nlohmann::json * const get(const std::string &name)
{
	return tegia::threads::thread->config(name);
};


} // namespace configuration
} // namespace tegia

//
//
//

namespace tegia {

int message::send(
	const std::string &name, 
	const std::string &action, 
	nlohmann::json data,
	int priority)
{
	auto _message = tegia::message::init(std::move(data));
	return tegia::threads::thread->_actor_map->send_message(name,action,_message,priority);
};


int message::send(
	const std::string &name, 
	const std::string &action, 
	const std::shared_ptr<message_t> &message,
	int priority)
{
	return tegia::threads::thread->_actor_map->send_message(name,action,message,priority);
};


int message::send(
	const std::shared_ptr<message_t> &message,
	std::function<int(const std::shared_ptr<message_t> &)> fn,
	int priority)
{
	return tegia::threads::thread->node()->send_message(message,fn,priority);
};


} // namespace tegia



namespace tegia {
namespace actors {

int unload(const std::string &actor)
{
	return tegia::threads::thread->actor_map()->unload(actor);
}

}  // namespace actors
}  // namespace tegia



namespace tegia {
namespace threads {

int run(std::function<void()> _fn)
{
	return tegia::threads::thread->thread(_fn);
};


const std::shared_ptr<tegia::user> user()
{
    return tegia::threads::thread->user();
};



}
}
