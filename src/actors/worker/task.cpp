#include <tegia/actors/worker/task.h>


namespace tegia {
namespace worker {

void task_t::init(const nlohmann::json &jdata)
{
    const nlohmann::json* task_data = &jdata;
    if (jdata.contains("payload") && jdata["payload"].is_object()) {
        task_data = &jdata["payload"];
    }

    const auto& data = *task_data;

    this->uuid = data.value("uuid", std::string{});
    this->assignment_id = data.value("assignment_id", std::string{});
    this->params = data.value("params", nlohmann::json::object());

    if (data.contains("bind") && data["bind"].is_object()) {
        if (data["bind"].contains("handler") && data["bind"]["handler"].is_object()) {
            if (data["bind"]["handler"].contains("actor") && data["bind"]["handler"]["actor"].is_string()) {
                this->handler.actor = data["bind"]["handler"]["actor"].get<std::string>();
            }
            if (data["bind"]["handler"].contains("action") && data["bind"]["handler"]["action"].is_string()) {
                this->handler.action = data["bind"]["handler"]["action"].get<std::string>();
            }
        }

        if (data["bind"].contains("manager")) {
            const auto& m = data["bind"]["manager"];

            if (m.is_string()) {
                this->manager.actor = m.get<std::string>();
            } else if (m.is_object() && m.contains("actor") && m["actor"].is_string()) {
                this->manager.actor = m["actor"].get<std::string>();
            }

            if (data["bind"].contains("commit") && data["bind"]["commit"].is_string()) {
                this->manager.action = data["bind"]["commit"].get<std::string>();
            } else if (m.is_object() && m.contains("commit") && m["commit"].is_string()) {
                this->manager.action = m["commit"].get<std::string>();
            } else if (m.is_object() && m.contains("commit_action") && m["commit_action"].is_string()) {
                this->manager.action = m["commit_action"].get<std::string>();
            } else if (m.is_object() && m.contains("action") && m["action"].is_string()) {
                this->manager.action = m["action"].get<std::string>();
            }
        }

        if (data["bind"].contains("worker") && data["bind"]["worker"].is_object()) {
            if (data["bind"]["worker"].contains("actor") && data["bind"]["worker"]["actor"].is_string()) {
                this->worker.actor = data["bind"]["worker"]["actor"].get<std::string>();
            }
            if (data["bind"]["worker"].contains("action") && data["bind"]["worker"]["action"].is_string()) {
                this->worker.action = data["bind"]["worker"]["action"].get<std::string>();
            }
        }
    }

    if (jdata.contains("worker") && jdata["worker"].is_string()) {
        this->worker.actor = jdata["worker"].get<std::string>();
    } else if (data.contains("worker") && data["worker"].is_string()) {
        this->worker.actor = data["worker"].get<std::string>();
    }

    if (data.contains("stage") == true)
    {
        this->stage = data["stage"].get<int>();
    }
    else
    {
        this->stage = 1;
    }
};


int task_t::commit(int status, nlohmann::json result, nlohmann::json error)
{
    tegia::message::send(
        this->manager.actor,
        this->manager.action,
        nlohmann::json {
            { "uuid", this->uuid },
            { "assignment_id", this->assignment_id },
            { "assignment_id", this->assignment_id },
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
