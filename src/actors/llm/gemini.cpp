#include "gemini.h"

#include <format>
#include <tuple>

#include <tegia/core/http.h>

namespace tegia {
namespace llm {


//
//
//


nlohmann::json gemini::set_system(const std::string &content)
{
	nlohmann::json _system;
	_system["parts"].push_back({
		{ "text", content }
	});
	return std::move(_system);
};


//
//
//


nlohmann::json gemini::set_user(const std::string &content)
{
	nlohmann::json _user;
	_user["role"] = "user";
	_user["parts"].push_back({
		{ "text", content }
	});
	return std::move(_user);
};


//
//
//


std::tuple<int, std::string> gemini::request(
	const nlohmann::json &system,
	const nlohmann::json &user,
	const std::string &model_name,
	const std::string &api_key,
	bool is_json)
{
	auto http = tegia::http::client();
	http.set_header("Content-Type", "application/json");

	nlohmann::json prompt;
	prompt["system_instruction"] = system;
	prompt["contents"].push_back(user);

	if(is_json == true)
	{
		prompt["generationConfig"]["response_mime_type"] = "application/json";
	}

	//  TODO: ADD JSON SCHEMA

	const auto status = http.post(
		std::format(
			"https://generativelanguage.googleapis.com/v1beta/models/{}:generateContent?key={}",
			model_name,
			api_key
		),
		prompt.dump()
	);

	if(status == 200)
	{
		nlohmann::json body_json = nlohmann::json::parse(http.response->data);				
		return { 
			200, 
			body_json["candidates"][0]["content"]["parts"][0]["text"].get<std::string>()
		};
	}
	else
	{
		std::cout << _ERR_TEXT_ << "GEMINI ERROR" << std::endl;
		std::cout << "status = " << status << std::endl;
		exit(0);
	}
};


} // namespace llm
} // namespace tegia
