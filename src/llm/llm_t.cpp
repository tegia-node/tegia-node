#include <tegia/llm/llm_t.h>

#include <tegia/llm/openrouter.h>
#include <tegia/llm/gemini.h>


namespace tegia {
namespace actors {


void llm_t::set_system(const std::string &content)
{
	switch(this->provider_crc32)
	{
		// gemini
		case 3032164056:
		{
			this->system = tegia::llm::gemini::set_system(content);
		}
		break;

		// openrouter
		case 2220310207:
		{
			this->system = tegia::llm::openrouter::set_system(content);
		}
		break;

		default:
		{
			std::cout << _ERR_TEXT_ << "not fount provider" << std::endl;
			std::cout << "provider = " << this->provider << std::endl;
			std::cout << "crc32    = " << this->provider_crc32 << std::endl;
			exit(0);
		}
		break;
	}
};


std::tuple<int,std::string> llm_t::request(const std::string &user, bool is_json)
{
	/*
	std::cout << _YELLOW_ << "system" << _BASE_TEXT_ << std::endl;
	std::cout << this->system << std::endl;
	std::cout << _YELLOW_ << "user" << _BASE_TEXT_ << std::endl;
	std::cout << user << std::endl;
	*/

	switch(this->provider_crc32)
	{
		// gemini
		case 3032164056:
		{
			return tegia::llm::gemini::request(
				this->system,
				tegia::llm::gemini::set_user(user),
				this->model,
				this->key,
				is_json
			);
		}
		break;

		// openrouter
		case 2220310207:
		{
			return tegia::llm::openrouter::request(
				this->system,
				tegia::llm::openrouter::set_user(user),
				this->model,
				this->key,
				is_json
			);
		}
		break;

		default:
		{
			std::cout << _ERR_TEXT_ << "not fount provider" << std::endl;
			std::cout << "provider = " << this->provider << std::endl;
			std::cout << "crc32    = " << this->provider_crc32 << std::endl;
			exit(0);
		}
		break;
	}
};


} // END namespace actors
} // END namespace tegia







