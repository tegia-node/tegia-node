#include <tegia/tegia.h>
#include <tegia/db/mysql/mysql.h>

#include <tegia/core/json.h> 

#include "node.h"

// dictionares
#include "../dictionaries/catalog.h"

#include <sys/prctl.h>
#include <csignal>

//	LOGGER
#include "logger.h"
#undef _LOG_LEVEL_
#define _LOG_LEVEL_ _LOG_NOTICE_
#include <tegia/context/log.h>
//	LOGGER



namespace tegia {
namespace node {


node::node()
{

};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


node::~node()
{

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
	return 200;
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


bool node::action()
{
	std::cout << _YELLOW_ << "\n--------------------------------------------" << _BASE_TEXT_ << std::endl;
	std::cout << _YELLOW_ << "NODE CONFIGURATIONS" << _BASE_TEXT_ << std::endl;
	std::cout << _YELLOW_ << "--------------------------------------------\n" << _BASE_TEXT_ << std::endl;

	LNOTICE("\n--------------------------------------------\nNODE CONFIGURATIONS\n--------------------------------------------\n")

	auto cluster = this->_config->get("cluster");

	//
	// INIT DICTIONARES
	//
	
	if(cluster->contains("/dictionaries"_json_pointer) == true)
	{
		auto catalog = tegia::dictionaries::catalog_t::instance();
		catalog->_list( (*cluster)["dictionaries"].get<std::string>() );
	}

	//
	// INIT DOMAINS
	//

	this->_actor_map->add_domain("base",tegia::domain::LOCAL);
	this->_actor_map->add_domain("parsers",tegia::domain::LOCAL);
	this->_actor_map->add_domain("http",tegia::domain::LOCAL);
	this->_actor_map->add_domain("test",tegia::domain::LOCAL);
	this->_actor_map->add_domain("app",tegia::domain::LOCAL);

	this->_actor_map->add_domain("ws",tegia::domain::LOCAL);
	this->_actor_map->add_domain("task",tegia::domain::LOCAL);
	
	this->_actor_map->add_domain("enrichment",tegia::domain::LOCAL);
	this->_actor_map->add_domain("models",tegia::domain::LOCAL);
	this->_actor_map->add_domain("growth",tegia::domain::LOCAL);
	this->_actor_map->add_domain("leaks",tegia::domain::LOCAL);
	this->_actor_map->add_domain("example",tegia::domain::LOCAL);

	this->_actor_map->add_domain("auth",tegia::domain::LOCAL);
	this->_actor_map->add_domain("user",tegia::domain::LOCAL);
	this->_actor_map->add_domain("data",tegia::domain::LOCAL);
	this->_actor_map->add_domain("enrich",tegia::domain::LOCAL);

	//
	// ADD PATTERNS
	//

	auto patterns = this->_config->get("patterns");
	for(auto it = patterns->begin(); it != patterns->end(); ++it)
	{
		this->_actor_map->add_pattern(it.key(),it->get<std::string>());
	}

	std::cout << _YELLOW_ << "\n--------------------------------------------" << _BASE_TEXT_ << std::endl;
	std::cout << _YELLOW_ << "NODE INIT" << _BASE_TEXT_ << std::endl;
	std::cout << _YELLOW_ << "--------------------------------------------\n" << _BASE_TEXT_ << std::endl;

	//
	// SEND MESSAGE
	//
	
	auto messages = this->_config->get("messages");
	for(auto message = messages->begin(); message != messages->end(); ++message)
	{
		auto msg = tegia::message::init((*message)["data"]);
		this->_actor_map->send_message(
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
	//
	// INIT
	//
	
	this->_logger    = new tegia::node::logger("logs/");
	this->_config    = new tegia::node::config();
	this->_manager   = new tegia::threads::manager_t();
	this->_actor_map = new tegia::actors::map_t();
	this->_manager->init(this, this->_config, this->_logger, this->_actor_map);

	//
	// INIT CONFIGURATIONS
	//

	auto cluster = this->_config->cluster();

	for(auto it = (*cluster)["configurations"].begin(); it != (*cluster)["configurations"].end(); ++it)
	{
		if(it->get<bool>() == false)
		{
			continue;
		}
		
		auto config = this->_config->configuration(it.key());
		std::string path = (*config)["path"].get<std::string>();
		for(auto it = (*config)["types"].begin(); it != (*config)["types"].end(); ++it)
		{
			this->_actor_map->add_type(
				it.key(), 							// type name
				path + it->get<std::string>()		// lib path
			);
		}
	}
	
	//
	// RUN MANTICORE
	//

	std::system("searchd -c ./configs/manticore.conf > /dev/null");

	auto signal_handler = 
		[](int signal)
		{
			std::system("searchd -c ./configs/manticore.conf --stop > /dev/null");
			std::signal(signal, SIG_DFL);
			std::raise(signal);
		};
	auto atexit_handler =
		[]()
		{
			std::system("searchd -c ./configs/manticore.conf --stop > /dev/null");
		};

	std::signal(SIGINT, signal_handler);
	std::signal(SIGTERM, signal_handler);
	std::signal(SIGQUIT, signal_handler);
	std::signal(SIGHUP, signal_handler);
	std::atexit(atexit_handler);

	//
	// INIT THREADS
	//

	tegia::mysql::init();

	std::cout << _YELLOW_ << std::endl;
	std::cout << "INIT THREADS" << std::endl;
	std::cout << _BASE_TEXT_ << std::endl;

	// this->_manager->init(this, this->_config, this->_logger, this->_actor_map);
	this->_actor_map->pool = new tegia::threads::pool_t();
	this->_actor_map->pool->_callback = std::bind(&tegia::node::node::action,this);
	this->_actor_map->pool->run(8);

	return true;
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


const nlohmann::json * const node::config(const std::string &name)
{
	return this->_config->get(name);
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


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////



}	// END namespace node
}	// END namespace tegia