#include <tegia/llm/openrouter.h>

#include <format>
#include <tuple>

#include <tegia/core/http.h>

namespace tegia {
namespace llm {


//
//
//


nlohmann::json openrouter::set_system(const std::string &content)
{
	nlohmann::json _system;
	_system["role"] = "system";
	_system["content"] = content;
	return std::move(_system);
};


//
//
//


nlohmann::json openrouter::set_user(const std::string &content)
{
	nlohmann::json _system;
	_system["role"] = "user";
	_system["content"] = content;
	return std::move(_system);
};


//
//
//


std::tuple<int, std::string> openrouter::request(
	const nlohmann::json &system,
	const nlohmann::json &user,
	const std::string &model_name,
	const std::string &api_key,
	bool is_json)
{
	auto http = tegia::http::client();
	http.set_header("Content-Type", "application/json");
	http.set_header("Authorization", "Bearer " + api_key);

	nlohmann::json prompt;
	prompt["model"] = model_name;
	prompt["messages"].push_back(system);
	prompt["messages"].push_back(user);

	if(is_json == true)
	{
		prompt["response_format"]["type"] = "json_object";
	}

	const auto status = http.post(
		"https://openrouter.ai/api/v1/chat/completions",
		prompt.dump()
	);

	if(status == 200)
	{
		nlohmann::json body_json = nlohmann::json::parse(http.response->data);
		return { 
			200, 
			body_json["choices"][0]["message"]["content"].get<std::string>() 
		};
	}
	else
	{
		std::cout << _ERR_TEXT_ << "OPENROUTER ERROR" << std::endl;
		std::cout << "status = " << status << std::endl;
		exit(0);
	}
};


} // namespace llm
} // namespace tegia

