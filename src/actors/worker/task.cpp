#include <tegia/actors/worker/task.h>


/*
    {
        "status": 0,
        "stage": 0,
        "uuid": "<string UUID>",
        "type": "<string>",
        "params": {},
        "result": null,
        "error": null        
        "hash": "<string>",
        
    }

*/

namespace tegia {
namespace worker {

void task_t::init(const nlohmann::json &jdata)
{
    std::cout << _YELLOW_ << jdata << _BASE_TEXT_ << std::endl;

    this->uuid = jdata["uuid"].get<std::string>();
    this->type = jdata["type"].get<std::string>();
    this->hash = jdata["hash"].get<std::string>();

    this->params = jdata["params"];
    this->result = jdata["result"];
    this->error = jdata["error"];

    if (jdata.contains("stage") == true)
    {
        this->stage = jdata["stage"].get<int>();
    }
    else
    {
        this->stage = 1;
    }

    if (jdata.contains("status") == true)
    {
        this->status = jdata["status"].get<int>();
    }
    else
    {
        this->status = 0;
    }    
};


}  // END namespace worker
}  // END namespace tegia
