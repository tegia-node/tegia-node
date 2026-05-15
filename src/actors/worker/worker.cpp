#include <tegia/actors/worker/worker.h>


namespace tegia {
namespace actors {

/*
int worker_t::init(const std::shared_ptr<message_t> &message)
{
	std::cout << _YELLOW_ << "[RUN] tegia::actors::worker::init()" << _BASE_TEXT_ << std::endl;
	std::cout << message->data << std::endl;
	
	this->manager.actor  = message->data["manager"]["actor"].get<std::string>();
	this->manager.action = message->data["manager"]["action"].get<std::string>();
	
	tegia::message::send(
		this->manager.actor, 
		this->manager.action, 
		{
			{ "worker", this->name },
			{ "task", "" },
			{ "status", 100 }
		}
	);

	std::cout << _YELLOW_ << "[END] tegia::actors::worker::init()" << _BASE_TEXT_ << std::endl;
	return 200;
};
*/

} // END namespace actors
} // END namespace tegia







