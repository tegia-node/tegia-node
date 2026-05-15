#ifndef H_ACTORS_WORKER_TASK
#define H_ACTORS_WORKER_TASK


#include <tegia/tegia.h>

namespace tegia {
namespace worker {

class task_t
{
    public:
        task_t() = default;
        ~task_t() = default;

        std::string uuid;
        tegia::actors::addr_t handler;
        tegia::actors::addr_t manager;
        tegia::actors::addr_t worker;

        int stage = 0;
        nlohmann::json params = nullptr;
        nlohmann::json error = nullptr;

		void init(const nlohmann::json &jdata);
        int commit(int status, nlohmann::json result, nlohmann::json error);
};

}  // END namespace worker
}  // END namespace tegia

#endif