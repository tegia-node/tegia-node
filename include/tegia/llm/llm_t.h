#ifndef H_TEGIA_ACTORS_LLM
#define H_TEGIA_ACTORS_LLM

#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <functional>
#include <array>

#include <tegia/tegia.h>
#include <tegia/core/http.h>
#include <tegia/db/mysql/mysql.h>


namespace tegia {
namespace actors {


class llm_t : public actor_t
{
	template <typename actor_type, typename Enable> friend class type_t;

	protected:
		llm_t(const std::string &type, const std::string &name)
			: actor_t(type, name)
		{}

	protected:
		template<typename TYPE>
		int init(const std::shared_ptr<message_t> &message)
		{
			if(message->data.contains("provider") == true)
			{
				this->provider = message->data["provider"].get<std::string>();
				this->provider_crc32 = tegia::crypt::crc32(this->provider);
				message->data.erase("provider");
			}

			if(message->data.contains("url") == true)
			{
				this->url = message->data["url"].get<std::string>();
				message->data.erase("url");
			}

			if(message->data.contains("model") == true)
			{
				this->model = message->data["model"].get<std::string>();
				message->data.erase("model");
			}

			if(message->data.contains("key") == true)
			{
				this->key = message->data["key"].get<std::string>();
				message->data.erase("key");
			}

			if(message->data.contains("system") == true)
			{
				std::string path = message->data["system"].get<std::string>();
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
				message->data.erase("system");
			}

			static_cast<TYPE *>(this)->init(message);
			return 200;
		}

		void set_system(const std::string &content);
		std::tuple<int,std::string> request(const std::string &task_uuid, const std::string &user, bool is_json);

	private:	
		std::string provider;
		long long int provider_crc32;
		std::string url;
		std::string model;
		std::string key;
		nlohmann::json system;
};


//
//
//


template<typename actor_type>
class type_t<actor_type, std::enable_if_t<std::is_base_of_v<tegia::actors::llm_t, actor_type>>> : public type_base_t 
{
	protected: 

	public:
		type_t(const std::string &type): type_base_t(type)
		{
			this->add_action(
				"/init",
				"",
				static_cast<tegia::actors::action_fn_ptr>(&tegia::actors::llm_t::template init<actor_type>),
				tegia::user::roles(ROLES::SESSION::SYSTEM)
			);
		};

		actor_t * create_actor(const std::string &name) override
		{
			return new actor_type(name);
		};
};


} // END namespace actors
} // END namespace tegia


#endif
