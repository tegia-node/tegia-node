#include "openrouter.h"

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

	std::string request_uuid = tegia::random::uuid();
	tegia::json::save("../data/llm/requests/" + request_uuid + "-request.json", prompt);

	const auto status = http.post(
		"https://openrouter.ai/api/v1/chat/completions",
		prompt.dump()
	);

	if(status == 200)
	{
		//
		// JSON ERROR
		//

		nlohmann::json body_json = nlohmann::json::parse(http.response->data, nullptr, false);
		if(body_json.is_discarded())
		{
			std::cout << _ERR_TEXT_ << "Invalid JSON in OpenRouter response" << std::endl;
			std::cout << "response = " << http.response->data << std::endl;

			tegia::log::event_t event;
			event.code = "crdQGvRtdaFZUDIDdNyv";
			event._data = {
				{ "llm", http.response->data }
			};
			L3ERROR(event);
			return { 500, "" };
		}

		//
		// RESULT ERROR
		//

		if(body_json["choices"][0]["finish_reason"].get<std::string>() != "stop")
		{
			std::cout << _ERR_TEXT_ << "OPENROUTER RESULT ERROR" << std::endl;
			std::cout << "finish_reason = " << body_json["choices"][0]["finish_reason"].get<std::string>() << std::endl;
			tegia::json::save("../data/llm/requests/" + request_uuid + "-response-[error].json", body_json);

			tegia::log::event_t event;
			event.code = "crdQGvRtdaFZUDIDdNyv";
			event._data = {
				{ "llm", http.response->data }
			};
			L3ERROR(event);
			return { 500, "" };
		}

		tegia::json::save("../data/llm/requests/" + request_uuid + "-response.json", body_json);

		return { 
			200, 
			body_json["choices"][0]["message"]["content"].get<std::string>() 
		};
	}
	else
	{
		std::cout << _ERR_TEXT_ << "OPENROUTER ERROR" << std::endl;
		std::cout << "status = " << status << std::endl;
		std::cout << http.response->data << std::endl;
		exit(0);
	}
};


} // namespace llm
} // namespace tegia

