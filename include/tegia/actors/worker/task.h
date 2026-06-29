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
        std::string type;
        std::string hash;

        int stage = 0;
        int status = 0;

        nlohmann::json params = nullptr;
        nlohmann::json result = nullptr;
        nlohmann::json error = nullptr;

        void init(const nlohmann::json &jdata);
};

}  // END namespace worker
}  // END namespace tegia

#endif
