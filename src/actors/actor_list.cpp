#include <tegia/actors/actor_list.h>


#include <dlfcn.h>

namespace tegia {
namespace actors {


list::~list()
{
	std::scoped_lock lock{this->actor_mutex};
	_actors.clear();
}



/////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
/////////////////////////////////////////////////////////////////////////////////////////////


void list::load_configurations()
{
	//
	// Загружаем конфигурации
	//

	auto* const _config = tegia::conf::get("node");
	auto* const configurations = (*_config)["configurations"];

	for (auto const& configuration : *configurations)
	{
		try
		{
			if (configuration["isload"].get<bool>())
			{	
				load_configuration(configuration["name"].get<std::string>(), configuration["file"].get<std::string>());
			}
		}

		catch (nlohmann::json::parse_error& e)
		{
			std::cout << _ERR_TEXT_ << "parse_error" << std::endl;
			std::cout << "      configuration description in ./config.json must be:" << std::endl;
			std::cout << "      {" << std::endl;
			std::cout << "         \"isload\": <bool>," << std::endl;
			std::cout << "         \"name\": <string>," << std::endl;
			std::cout << "         \"file\": <string: path to config file>" << std::endl;
			std::cout << "      }" << std::endl;
			exit(0);
		}

		catch (nlohmann::json::out_of_range& e)
		{
			std::cout << _ERR_TEXT_ << "out_of_range" << std::endl;			
			std::cout << "      configuration description in ./config.json must be:" << std::endl;
			std::cout << "      {" << std::endl;
			std::cout << "         \"isload\": <bool>," << std::endl;
			std::cout << "         \"name\": <string>," << std::endl;
			std::cout << "         \"file\": <string: path to config file>" << std::endl;
			std::cout << "      }" << std::endl;
			exit(0);
		}

		catch (nlohmann::json::type_error& e)
		{
			std::cout << _ERR_TEXT_ << "type_error" << std::endl;	
			std::cout << "      configuration description in ./config.json must be:" << std::endl;
			std::cout << "      {" << std::endl;
			std::cout << "         \"isload\": <bool>," << std::endl;
			std::cout << "         \"name\": <string>," << std::endl;
			std::cout << "         \"file\": <string: path to config file>" << std::endl;
			std::cout << "      }" << std::endl;
			exit(0);
		}

	}
};


void list::init()
{
	//
	// Выполняем инициализирующие операции
	//

	auto* const _config = tegia::conf::get("node");
	auto* const init = (*_config)["init"];
	if (init != nullptr)
	{
		for (auto const& item : *init)
		{
			send_message(item["actor"].get<std::string>(), item["action"].get<std::string>(), item["message"]);
		}	
	}
};


/////////////////////////////////////////////////////////////////////////////////////////////
//
//  Функция обеспечиваает загрузку конкретной конфигурации 
//
/////////////////////////////////////////////////////////////////////////////////////////////


void list::load_configuration(const std::string &name, const std::string &file)
{
	std::cout << std::endl;
	std::cout << "LOAD CONFIGURATION: [\033[32;1m" << name << "\033[0m] [" << file << "]" << std::endl;

	auto conf = tegia::conf::load(name, file);

	std::string path = (*conf)["path"]->get<std::string>();

	//
	// Если есть секция "db"   
	//

	auto db = (*conf)["db"];
	int thread_count = tegia::threads::count();

	if( db->is_null() == false)
	{
		for(auto it = db->begin(); it != db->end(); it++)   
		{ 
			tegia::mysql::connect(thread_count, (*it) );                                                                
		}
	}

	// 
	// Загружаем типы акторов
	//
	if (auto loads = (*conf)["loads"]; !loads->is_null())
	{
		for (auto& item : *loads)
		{
			// std::cout << type_itr->get<std::string>() << std::endl;
			this->load_actor_type(path, item.get<std::string>());
		}
	}
	else
	{
		EOUT("No one actor type to load is specified in the configuration file!")
		// TODO: ????
	}

	// 
	// Содаем экземпляры акторов
	//
	if (auto actors = (*conf)["actors"]; !actors->is_null())
	{
		for (auto& actor : *actors)
		{
			std::string const  actor_name = actor["name"].get<std::string>();
			std::string const actor_type = actor["type"].get<std::string>();
		    bool const share = actor.contains("share") && actor["share"].get<bool>();
			auto init_data = actor.contains("data") ? actor["data"]  : nlohmann::json::object();

			create_actor(actor_type, actor_name, share, init_data); 
		}
	}
	else
	{
		EOUT("No one actor to create is specified in the configuration file!")
		// TODO: ????
	}
};


/////////////////////////////////////////////////////////////////////////////////////////////
//
//  Функция обеспечиваает загрузку библиотеки актора
//
/////////////////////////////////////////////////////////////////////////////////////////////


bool list::load_actor_type(const std::string &path, const std::string &actor_type)
{
	void * lib;
	std::string lib_name = path + "/bin/lib" + actor_type + ".so";

	// std::cout << lib_name << std::endl;

	lib = dlopen(lib_name.c_str(), RTLD_LAZY);
	if (!lib)
	{
		std::string message = "[" + std::string(dlerror()) + "]";
		std::cout << _ERR_TEXT_ "load " << actor_type << " " << message << std::endl;
		return false;
	}

	auto _fn = ( tegia::actors::router_base* (*)(void) )dlsym(lib,"_load_actor");
	this->_routers.emplace(actor_type,_fn());

	return true;
};


	/////////////////////////////////////////////////////////////////////////////////////////////
	//
	//  Создание нового актора
	//
	/////////////////////////////////////////////////////////////////////////////////////////////
	tegia::actors::actor_base* list::create_actor(const std::string &actor_type, const std::string &actor_name, bool const share, nlohmann::json &data)
	{
		std::unique_lock<std::shared_mutex> ul(this->actor_mutex);

		// 
		// Проверить, что актор уже существует
		//

		auto pos_actor = this->_actors.find(actor_name);
		if(pos_actor == this->_actors.end())
		{
			auto pos = this->_routers.find(actor_type);
			if(pos == this->_routers.end())
			{
				std::cout << _ERR_TEXT_ << "create actor " << actor_name << " \033[31m not found factory " << actor_type << "\033[0m" << std::endl;
				return nullptr;
			}

			actor_desc_t desc{};
			desc.actor = pos->second->create_actor(actor_name, data);
			desc.router = pos->second;
			desc.share = share;
			this->_actors.emplace(actor_name, std::move(desc));
			std::cout << _OK_TEXT_ << _BCYAN_ << "create actor " << _BASE_TEXT_ << actor_name << " [\033[37m" << actor_type << "\033[0m]" << std::endl;

			if (tegia::sockets::is_client_node() && share)
			{
				auto actors = nlohmann::json::array();
				auto actor = nlohmann::json::object();
				actor["actor"] = actor_name;
				actor["actions"] = std::move(pos->second->get_action_names());
				actors.push_back(std::move(actor));
				auto const json_data = nlohmann::json {{CONFIG_MESSAGE_DATA, {{NODE_UUID, tegia::sockets::node_uuid()}, {ADD_ACTOR_LIST, std::move(actors)}}}};
				tegia::sockets::send_config_message_to_server(json_data.dump());
			}

			return desc.actor;
		}
		else
		{
			std::cout << _OK_TEXT_ << _BCYAN_ << "actor is already create " << _BASE_TEXT_ << actor_name << " [\033[37m" << actor_type << "\033[0m]" << std::endl;
			return pos_actor->second.actor;		
		}

		return nullptr;
	};


/////////////////////////////////////////////////////////////////////////////////////////////
//
//  Получение указателя на объект актора
//
/////////////////////////////////////////////////////////////////////////////////////////////


tegia::actors::actor_base * list::get_actor(const std::string &actor_name)
{
	std::shared_lock<std::shared_mutex> sl(this->actor_mutex);

	auto pos = this->_actors.find(actor_name);
	if(pos == this->_actors.end())
	{
		return nullptr;	
	}

	return this->_actors[actor_name].actor;
};


	/////////////////////////////////////////////////////////////////////////////////////////////
	//
	//  Удаление актора
	//
	/////////////////////////////////////////////////////////////////////////////////////////////
	bool list::delete_actor(const std::string &actor_name)
	{
		std::unique_lock<std::shared_mutex> ul(this->actor_mutex);

		auto pos = this->_actors.find(actor_name);
		if(pos == this->_actors.end())
		{
			std::cout << _ERR_TEXT_ << "[1] delActor " << actor_name << " not found actor" << std::endl;	
			LDEBUG("delActor " << actor_name << " not found actor")
			return true;	
		}

		if(pos->second.actor->countMessage.load() == 0)
		{
			bool send_notification = tegia::sockets::is_client_node() && pos->second.is_local && pos->second.share;
			delete pos->second.actor;
			this->_actors.erase(pos);
			if (send_notification)
			{
				auto actors = nlohmann::json::array();
				actors.emplace_back(actor_name);
				auto const json_data = nlohmann::json {{CONFIG_MESSAGE_DATA, {{NODE_UUID, tegia::sockets::node_uuid()}, {REMOVE_ACTOR_LIST, std::move(actors)}}}};
				tegia::sockets::send_config_message_to_server(json_data.dump());
			}
			return true;
		}

		std::cout << _ERR_TEXT_ << "[2] delActor " << actor_name << " messages not null" << std::endl;	
		LDEBUG("delActor " << actor_name << " messages not null")

		return false;
	};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//
// REM:
//		0 - success send message
//		1 - actor not found
//		2 - action not found
//

int list::send_message(const std::string &actor_name, const std::string &action_name, const std::shared_ptr<message_t> &message, int priority)
{
	std::shared_lock<std::shared_mutex> sl(this->actor_mutex);

	auto pos = this->_actors.find(actor_name);
	if (pos == this->_actors.end())
	{
		//
		//	Resend message to the server node, maybe required actor exists somewhere else.
		//
		if (tegia::sockets::is_client_node() && message->is_local())
		{
			auto fn = [actor = actor_name, action = action_name, message, priority] () -> void
			{ 
				tegia::sockets::send_message_to_server(std::move(actor), std::move(action), message, priority);
			};
				
			return tegia::threads::add_task(fn, message->get_priority());
		}
		//
		//	Nowhere to send message any more!
		//
		else
		{
			std::cout << _ERR_TEXT_ << "[1] sendMessage " << actor_name << action_name << " not found actor" << std::endl;	
			LDEBUG("sendMessage " << actor_name << action_name << " not found actor")
			return 1;	
		}
	}

	//
	// Select whether actor is located on the local or remote node:
	//
	if (pos->second.is_local)
	{
		NOUT("Send local message '" << actor_name << action_name << "'")
		return pos->second.router->init_job(pos->second.actor,action_name,message,priority);
	}
	else
	{
		//std::cout << _OK_TEXT_ << "Actor '" << actor_name << "' is located on the node " << pos->second.node_uuid  << std::endl;
		NOUT("Send remote message '" << actor_name << action_name << "' to node [" << pos->second.node_uuid << "]")
		
		auto fn = [actor = actor_name, action = action_name, uuid = pos->second.node_uuid, message, priority] () -> void
		{ 
			tegia::sockets::send_message_to_client(std::move(actor), std::move(action), std::move(uuid), message, priority);
		};
			
		return tegia::threads::add_task(fn, priority);
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int list::send_message(const std::string &actor_name, const std::string &action_name, nlohmann::json data, int priority)
{
	auto const message = std::make_shared<message_t>(data);
	return send_message(actor_name, action_name, message, priority);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::string list::find_pattern(const std::string &actor_name, const std::string &action_name)
{
	std::shared_lock<std::shared_mutex> sl(this->actor_mutex);

	auto pos = this->_actors.find(actor_name);
	if(pos == this->_actors.end())
	{
		return "";
	}

	return pos->second.router->find_pattern(action_name);
};


	///
	/// Returns the list with the names of actually loaded actors.
	///
	nlohmann::json list::shared_local_actors() const noexcept
	{
		nlohmann::json actors = nlohmann::json::array();
		std::shared_lock lock{actor_mutex};
		for (auto const& [name, desc] : _actors)
		{
			if (desc.share && desc.is_local)
			{
				nlohmann::json actor = nlohmann::json::object();
				actor["actor"] = name;
				actor["actions"] = std::move(desc.router->get_action_names());
				actors.push_back(std::move(actor));
			}
		}
		return actors;
	}

	///
	/// Adds to the list actors located on another remote node.
	//
	void list::add_client_node_actors(std::string node_uuid, nlohmann::json node_actor_list) noexcept
	{
		std::scoped_lock lock{actor_mutex};
		for (auto const& actor : node_actor_list)
		{
			auto const actor_name = actor["actor"].get<std::string>();
			_actors[actor_name].node_uuid = node_uuid;
			auto const router = std::make_shared<tegia::actors::remote_router>(actor_name);
			_actors[actor_name].router  = router;
			_actors[actor_name].is_local = false;
			_actors[actor_name].share = true;
			NOUT("Added remote actor '" << actor_name << "' on node [" << node_uuid <<"]")
			if (actor.contains("actions") && actor["actions"].is_array())
			{
				for (auto const& action : actor["actions"])
				{
					//NOUT("\tadd action: '" << action << "'")
					router->add(action.get<std::string>(), nullptr);
				}
			}
		}
	}

	///
	/// Removes from the list actors located on another remote node.
	//
	void list::remove_client_node_actors(nlohmann::json node_actor_list) noexcept
	{
		std::scoped_lock lock{actor_mutex};
		for (auto const& actor : node_actor_list)
		{
			auto const actor_name = actor.get<std::string>();
			_actors.erase(actor_name);
		}
	}

}	// namespace actors
}  // namespace tegia

