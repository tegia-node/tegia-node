#include <tegia/actors/llm/provider.h>

#include "openrouter.h"
#include "gemini.h"
#include "local.h"


namespace tegia {
namespace llm {


///////////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
///////////////////////////////////////////////////////////////////////////////////////////////////////


int provider_t::init(nlohmann::json &config)
{
    if(config.contains("provider") == true)
    {
        this->provider = config["provider"].get<std::string>();
        this->provider_crc32 = tegia::crypt::crc32(this->provider);
        config.erase("provider");
    }

    if(config.contains("url") == true)
    {
        this->url = config["url"].get<std::string>();
        config.erase("url");
    }

    if(config.contains("model") == true)
    {
        this->model = config["model"].get<std::string>();
        config.erase("model");
    }

    if(config.contains("key") == true)
    {
        this->key = config["key"].get<std::string>();
        config.erase("key");
    }

    if(config.contains("system") == true)
    {
        std::string path = config["system"].get<std::string>();
        auto file = std::ifstream(path);
        if(!file.is_open())
        {
            std::cout << _ERR_TEXT_ << "system file is not found" << std::endl;
            std::cout << "path = " << path << std::endl;
            exit(0);
        }

        std::string data((std::istreambuf_iterator<char>(file)),
                            std::istreambuf_iterator<char>());
        this->set_system(data);
        config.erase("system");
    }

    return 200;
};


///////////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
///////////////////////////////////////////////////////////////////////////////////////////////////////


int provider_t::set_system(const std::string &content)
{
	switch(this->provider_crc32)
	{
        //
		// local
        //

		case 2346092776:
		{
			this->system = tegia::llm::local::set_system(content);
		}
		break;

		//
        // gemini
        //

		case 3032164056:
		{
			this->system = tegia::llm::gemini::set_system(content);
		}
		break;

		//
        // openrouter
        //

		case 2220310207:
		{
			this->system = tegia::llm::openrouter::set_system(content);
		}
		break;

        //
        //
        //

		default:
		{
			std::cout << _ERR_TEXT_ << "not fount provider" << std::endl;
			std::cout << "provider = " << this->provider << std::endl;
			std::cout << "crc32    = " << this->provider_crc32 << std::endl;
			exit(0);
		}
		break;
	}

    return 200;
};


///////////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
///////////////////////////////////////////////////////////////////////////////////////////////////////


std::tuple<int,std::string> provider_t::request(const std::string &task_uuid, const std::string &user, bool is_json)
{
	switch(this->provider_crc32)
	{
		// local
		case 2346092776:
		{
			return tegia::llm::local::request(
				this->system,
				tegia::llm::local::set_user(user),
				this->model,
				this->key,
				is_json
			);
		}
		break;

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


///////////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
///////////////////////////////////////////////////////////////////////////////////////////////////////


}  // END namespace llm
}  // END namespace tegia

