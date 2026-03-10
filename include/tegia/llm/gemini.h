#ifndef H_TEGIA_LLM_GEMINI_PROVIDER
#define H_TEGIA_LLM_GEMINI_PROVIDER


#include <tegia/tegia.h>


namespace tegia {
namespace llm {


class gemini
{
	public:
		static nlohmann::json set_system(const std::string &content);
		static nlohmann::json set_user(const std::string &content);
		static std::tuple<int, std::string> request(
			const nlohmann::json &system,
			const nlohmann::json &user,
			const std::string &model_name,
			const std::string &api_key,
			bool is_json);
};


} // namespace llm
} // namespace tegia


#endif
