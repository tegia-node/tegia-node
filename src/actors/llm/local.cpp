#include "local.h"

#include <format>
#include <tuple>

#include <tegia/core/http.h>

namespace tegia {
namespace llm {


//
//
//


nlohmann::json local::set_system(const std::string &content)
{
	nlohmann::json _system;
	_system["role"] = "system";
	_system["content"] = content;
	return std::move(_system);
};


//
//
//


nlohmann::json local::set_user(const std::string &content)
{
	nlohmann::json _system;
	_system["role"] = "user";
	_system["content"] = content;
	return std::move(_system);
};


//
//
//


std::tuple<int, std::string> local::request(
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
	prompt["chat_template_kwargs"] = {
		{ "enable_thinking", false }
	};

	if(is_json == true)
	{
		prompt["response_format"]["type"] = "json_object";
	}

	std::string request_uuid = tegia::random::uuid();
	tegia::json::save("../data/llm/requests/" + request_uuid + "-request.json", prompt);

	auto start = std::chrono::steady_clock::now();

	const auto status = http.post(
		"https://llm.openyourself.ru/v1/chat/completions",
		prompt.dump()
	);

	auto end = std::chrono::steady_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

	std::ofstream logFile("timings.txt", std::ios::app);
	logFile << duration.count() << " ns" << std::endl;
	logFile.close();

	if(status == 200)
	{
		//
		// JSON ERROR
		//

		nlohmann::json body_json = nlohmann::json::parse(http.response->data, nullptr, false);
		if(body_json.is_discarded())
		{
			std::cout << _ERR_TEXT_ << "Invalid JSON in Local response" << std::endl;
			std::cout << "response = " << http.response->data << std::endl;

			tegia::log::event_t event;
			event.code = "KgBsnWBlUazxhwgpWmnq";
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
			std::cout << _ERR_TEXT_ << "LOCAL RESULT ERROR" << std::endl;
			std::cout << "finish_reason = " << body_json["choices"][0]["finish_reason"].get<std::string>() << std::endl;
			tegia::json::save("../data/llm/requests/" + request_uuid + "-response-[error].json", body_json);

			tegia::log::event_t event;
			event.code = "UGuBAYzxLeLnvVZjQHRz";
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
		std::cout << _ERR_TEXT_ << "LOCAL ERROR" << std::endl;
		std::cout << "http   = https://llm.openyourself.ru/v1/chat/completions" << std::endl;
		std::cout << "status = " << status << std::endl;
		std::cout << _YELLOW_ << "REQUEST" << _BASE_TEXT_ << std::endl;
		std::cout << prompt << std::endl;
		std::cout << _YELLOW_ << "RESPONSE" << _BASE_TEXT_ << std::endl;
		std::cout << http.response->data << std::endl;
		exit(0);
	}
};


} // namespace llm
} // namespace tegia

