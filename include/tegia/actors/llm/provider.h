#ifndef H_TEGIA_LLM_PROVIDER
#define H_TEGIA_LLM_PROVIDER


#include <tegia/tegia.h>


namespace tegia {
namespace llm {

class provider_t
{
    public:
        provider_t() = default;
        ~provider_t() = default;

        int init(nlohmann::json &config);
        int set_system(const std::string &content);
        std::tuple<int,std::string> request(const std::string &task_uuid, const std::string &user, bool is_json);

    private:
		std::string provider;
		long long int provider_crc32;
		std::string url;
		std::string model;
		std::string key;
		nlohmann::json system;            
};

}
}

#endif
