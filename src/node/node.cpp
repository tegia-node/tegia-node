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
	// auto start_time = std::chrono::high_resolution_clock::now();

	auto [result,_fn] = this->actor_map.send_message(
		actor,
		action,
		message
	);

	// auto end_time = std::chrono::high_resolution_clock::now();
	// auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);
	// std::cout << "[CORE] Время упаковки: " << duration.count() << " наносекунд" << std::endl;

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
	std::cout << _YELLOW_ << "NODE CONFIGURATIONS" << _BASE_TEXT_ << std::endl;
	std::cout << _YELLOW_ << "--------------------------------------------\n" << _BASE_TEXT_ << std::endl;

	LNOTICE("\n--------------------------------------------\nNODE CONFIGURATIONS\n--------------------------------------------\n")

	auto cluster = this->_config->get("cluster");

	/*
	std::cout << _YELLOW_ << "db connections" << _BASE_TEXT_ << std::endl;
	std::cout << this->_config->get("dbc")->dump() << std::endl;

	std::cout << _YELLOW_ << "messages" << _BASE_TEXT_ << std::endl;
	std::cout << this->_config->get("messages")->dump() << std::endl;

	std::cout << _YELLOW_ << "patterns" << _BASE_TEXT_ << std::endl;
	std::cout << (*cluster)["patterns"].dump() << std::endl;
	*/

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
	this->actor_map.add_domain("data",tegia::domain::LOCAL);

	//
	// ADD PATTERNS
	//

	auto patterns = this->_config->get("patterns");
	for(auto it = patterns->begin(); it != patterns->end(); ++it)
	{
		this->actor_map.add_pattern(it.key(),it->get<std::string>());
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
	//
	// INIT LOG
	//

	tegia::logger::instance("logs/");

	LNOTICE("\n--------------------------------------------\nNODE RUN\n--------------------------------------------\n")
	std::cout << _YELLOW_ << "\n--------------------------------------------" << _BASE_TEXT_ << std::endl;
	std::cout << _YELLOW_ << "NODE RUN" << _BASE_TEXT_ << std::endl;
	std::cout << _YELLOW_ << "--------------------------------------------\n" << _BASE_TEXT_ << std::endl;


	
	/*
	d1_t * d1 = new d1_t;
	d2_t * d2 = new d2_t;

	base_t *db1 = d1;
	base_t *db2 = d2;

	std::cout << typeid( db1 ).name() << std::endl;
	std::cout << typeid( *db1 ).name() << std::endl;
	std::cout << typeid( db2 ).name() << std::endl;
	std::cout << typeid( *db2 ).name() << std::endl;

	exit(0);
	*/

	//
	// INIT CONFIGURATIONS
	//

	this->_config = new tegia::node::config();
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
			this->actor_map.add_type(
				it.key(), 							// type name
				path + it->get<std::string>()		// lib path
			);
		}
	}
	
	//
	// INIT THREADS
	//

	std::cout << _YELLOW_ << std::endl;
	std::cout << "INIT THREADS" << std::endl;
	std::cout << _BASE_TEXT_ << std::endl;

	this->_threads = new tegia::threads::pool();		
	auto _conf_db = this->_config->get("dbc");
	this->_threads->init(
		this->_config->thread_count, 
		std::bind(&tegia::node::node::init_thread,this,(*_conf_db)), 
		std::bind(&tegia::node::node::action,this)
	);

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


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


int node::unload(const std::string &actor)
{
	return this->actor_map.unload(actor);
};



}	// END namespace node
}	// END namespace tegia