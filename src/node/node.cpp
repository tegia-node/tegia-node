#include <tegia/tegia.h>
#include <tegia/db/mysql/mysql.h>

#include <tegia/core/json.h> 

#include "node.h"
#include "../threads/pool.h"
#include "../threads/data.h"
#include "../fn.h"

// LOGGER
#include "logger.h"
#undef _LOG_LEVEL_
#define _LOG_LEVEL_ _LOG_NOTICE_
#include "log.h"




namespace tegia {
namespace node {


node::node()
{

};

node::~node()
{
	delete this->_threads;
	delete this->_config;
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


void node::init_thread(const nlohmann::json &config)
{
	::tegia::threads::data->init(config);
	::tegia::threads::data->_node = this;
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


int node::send_message(const std::string &actor, const std::string &action, const std::shared_ptr<message_t> &message, int priority)
{
	auto start_time = std::chrono::high_resolution_clock::now();

	auto [result,_fn] = this->actor_map.send_message(
		actor,
		action,
		message
	);

	auto end_time = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);
	std::cout << "[CORE] Время упаковки: " << duration.count() << " наносекунд" << std::endl;

	if(result == 200)
	{
		this->_threads->add_task(_fn,priority);
	}


	return result;
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


int node::send_message(
	const std::shared_ptr<message_t> &message,
	std::function<int(const std::shared_ptr<message_t> &)> fn,
	int priority)
{
	auto _fn = [message,fn]()
	{
		try
		{
			int result = fn(message);
			
			auto callback = message->callback.get();
			if(callback.is_addr == true)
			{
				tegia::message::send(callback.actor, callback.action, message);
			}
		}

		catch(const std::exception& e)
		{
			std::cout << _ERR_TEXT_ << "[lambda] " << e.what() << std::endl;
			exit(0);
		}
	};

	this->_threads->add_task(_fn,priority);
	return 200;
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


bool node::action()
{
	std::cout << _YELLOW_ << "\n--------------------------------------------" << _BASE_TEXT_ << std::endl;
	std::cout << _YELLOW_ << "ACTION NODE" << _BASE_TEXT_ << std::endl;
	std::cout << _YELLOW_ << "--------------------------------------------\n" << _BASE_TEXT_ << std::endl;

	LNOTICE("\n--------------------------------------------\nACTION NODE\n--------------------------------------------\n")

	//
	// INIT ACTOR TYPES
	//

	this->actor_map.add_domain("base",tegia::domain::LOCAL);
	this->actor_map.add_domain("parsers",tegia::domain::LOCAL);
	this->actor_map.add_domain("http",tegia::domain::LOCAL);
	this->actor_map.add_domain("test",tegia::domain::LOCAL);
	this->actor_map.add_domain("app",tegia::domain::LOCAL);

	this->actor_map.add_domain("ws",tegia::domain::LOCAL);
	this->actor_map.add_domain("task",tegia::domain::LOCAL);
	
	this->actor_map.add_domain("enrichment",tegia::domain::LOCAL);
	this->actor_map.add_domain("models",tegia::domain::LOCAL);
	this->actor_map.add_domain("growth",tegia::domain::LOCAL);
	this->actor_map.add_domain("leaks",tegia::domain::LOCAL);
	this->actor_map.add_domain("example",tegia::domain::LOCAL);

	this->actor_map.add_domain("auth",tegia::domain::LOCAL);
	this->actor_map.add_domain("user",tegia::domain::LOCAL);


	std::vector<nlohmann::json> messages;

	for(auto conf = this->_config->_map.begin(); conf != this->_config->_map.end(); ++conf)
	{
		std::cout << conf->second->data << std::endl;
		
		if(conf->first == "_db") continue;

		if(conf->first == "node")
		{
			//
			// INIT
			//

			if(conf->second->data.contains("init") == false) continue;
			for(auto message = conf->second->data["init"].begin(); message != conf->second->data["init"].end(); ++message)
			{
				messages.push_back( (*message) );
			}
			
			continue;
		}

		//
		//
		//

		//
		// TYPES
		//

		for(auto type = conf->second->data["types"].begin(); type != conf->second->data["types"].end(); ++type)
		{
			// std::cout << "type = " << type.key() << std::endl;
			// std::cout << (*type) << std::endl;

			this->actor_map.add_type(
				type.key(), 										// type name
				conf->second->data["path"].get<std::string>(),		// base path
				&(*type)											// type config
			);
		}

		//
		// INIT
		//

		if(conf->second->data.contains("init") == false) continue;
		for(auto message = conf->second->data["init"].begin(); message != conf->second->data["init"].end(); ++message)
		{
			messages.push_back( (*message) );
		}

	}


	std::cout << _YELLOW_ << "\n--------------------------------------------" << _BASE_TEXT_ << std::endl;
	std::cout << _YELLOW_ << "NODE INIT" << _BASE_TEXT_ << std::endl;
	std::cout << _YELLOW_ << "--------------------------------------------\n" << _BASE_TEXT_ << std::endl;

	//
	// SEND MESSAGE
	//
	
	for(auto message = messages.begin(); message != messages.end(); ++message)
	{
		// auto msg = std::make_shared<message_t>((*message)["data"]);
		auto msg = tegia::message::init((*message)["data"]);
		this->send_message(
			(*message)["actor"].get<std::string>(),
			(*message)["action"].get<std::string>(),
			msg,
			60
		);
	}
	
	
	return true;
};



//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////





bool node::run()
{
	std::cout << _YELLOW_ << "\n--------------------------------------------" << _BASE_TEXT_ << std::endl;
	std::cout << _YELLOW_ << "RUN NODE" << _BASE_TEXT_ << std::endl;
	std::cout << _YELLOW_ << "--------------------------------------------\n" << _BASE_TEXT_ << std::endl;

	LNOTICE("\n--------------------------------------------\nRUN NODE\n--------------------------------------------\n")

	//
	// Инициализируем запись журнала
	//

	tegia::logger::instance();

	//
	// Читаем конфигурацию
	//

	this->_config = new tegia::node::config();
	this->_config->load();

	//
	// Справочники
	//
	
	auto node_conf = this->_config->get("node");
	tegia::dicts::catalog_t::instance( (*node_conf)["dictionaries"] );


	tegia::dict_t::instance();


	//
	//
	//	
	
	auto _conf_db = this->_config->get("_db");
	if(_conf_db == nullptr)
	{
		std::cout << _ERR_TEXT_ << "not found '_db' config" << std::endl;
	}
	
	//
	// Инициализируем потоки
	//

	this->_threads = new tegia::threads::pool();		

	this->_threads->init(
		this->_config->thread_count, 
		std::bind(&tegia::node::node::init_thread,this,(*_conf_db)), 
		std::bind(&tegia::node::node::action,this)
	);

	/*
	this->_threads->init(
		this->_config->thread_count, 
		(*_conf_db), 
		std::bind(&tegia::node::node::action,this)
	);
	*/


	return true;
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


const nlohmann::json * const node::config(const std::string &name)
{
	auto pos = this->_config->_map.find(name);
	if(pos != this->_config->_map.end())
	{
		return const_cast<const nlohmann::json * const>(&pos->second->data);
	}
	else
	{
		return nullptr;
	}
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


std::string node::config_path(const std::string &name)
{
	auto pos = this->_config->_map.find(name);
	if(pos != this->_config->_map.end())
	{
		return pos->second->path;
	}
	else
	{
		return "";
	}	
};



}	// END namespace node
}	// END namespace tegia