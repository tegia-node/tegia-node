#include <tegia/actors/worker/task.h>


namespace tegia {
namespace worker {

void task_t::init(const nlohmann::json &jdata)
{
    /*
    std::cout << _RED_TEXT_ << std::endl;
    std::cout << jdata << std::endl;
    std::cout << _BASE_TEXT_ << std::endl;
    */
   
    this->uuid = jdata["uuid"].get<std::string>();
    this->params = jdata["params"];

    this->handler.actor = jdata["bind"]["handler"]["actor"].get<std::string>();
    this->handler.action = jdata["bind"]["handler"]["action"].get<std::string>();

    this->manager.actor = jdata["bind"]["manager"]["actor"].get<std::string>();
    this->manager.action = jdata["bind"]["manager"]["action"].get<std::string>();

    this->worker.actor = jdata["bind"]["worker"]["actor"].get<std::string>();
    this->worker.action = jdata["bind"]["worker"]["action"].get<std::string>();

    if(jdata.contains("stage") == true)
    {
        this->stage = jdata["stage"].get<int>();
    }
    else
    {
        this->stage = 1;
    }
};

//
//
//

int task_t::commit(int status, nlohmann::json result, nlohmann::json error)
{
    tegia::message::send(
        this->manager.actor,
        this->manager.action,
        nlohmann::json { 
            { "uuid", this->uuid },
            { "status", status },
            { "worker", this->worker.actor },
            { "result", result },
            { "error", error } 
        }
    );

    return 0;
}


}  // END namespace worker
}  // END namespace tegia

