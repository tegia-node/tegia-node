#include <tegia2/context/context.h>
#include <tegia2/context/context2.h>
#include <tegia2/db/mysql/mysql.h>

#include <tegia2/core/json.h> 

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


node * node::_self = nullptr;
node * node::instance()
{
	if(!_self)
	{
		std::cout << _RED_TEXT_ << "create node" << _BASE_TEXT_ << std::endl;
		_self = new node();
	}
	return _self;
};


node::node()
{

};

node::~node()
{
	delete this->_threads;
	delete this->_config;
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


::tegia::context2 const * node::init_thread(const nlohmann::json &config)
{
	::tegia::context2 const * context = new ::tegia::context2();
	::tegia::threads::data->init(config);
	return context;
};


int node::send_message(const std::string &actor, const std::string &action, nlohmann::json data)
{
	int result = 0;
	std::function<void(::tegia::context2 const *)> _fn;

	std::tie(result,_fn) = this->actor_map.send_message(
		actor,
		action,
		data
	);

	if(result == 200)
	{
		this->_threads->add_task(_fn,10);
	}

	return 0;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
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

	std::vector<nlohmann::json> messages;

	for(auto conf = this->_config->_map.begin(); conf != this->_config->_map.end(); ++conf)
	{
		if(conf->first == "_db") continue;
		if(conf->first == "node")
		{
			// std::cout << "LOAD " << conf->first << std::endl; 
			// std::cout << conf->second->data << std::endl; 

			continue;
		} 

		//
		// TYPES
		//

		for(auto type = conf->second->data["types"].begin(); type != conf->second->data["types"].end(); ++type)
		{
			// std::cout << "type = " << type.key() << std::endl;
			// std::cout << (*type) << std::endl;

			this->actor_map.load_type(
				type.key(), 										// type name
				conf->second->data["path"].get<std::string>(),		// base path
				(*type)												// type config
			);
		}

		//
		// INIT
		//

		for(auto message = conf->second->data["init"].begin(); message != conf->second->data["init"].end(); ++message)
		{
			messages.push_back( (*message) );
		}

	}

	this->actor_map.add_domain("base","local");
	this->actor_map.add_domain("http","local");
	this->actor_map.add_domain("test","remote");

	std::cout << _YELLOW_ << "\n--------------------------------------------" << _BASE_TEXT_ << std::endl;
	std::cout << _YELLOW_ << "NODE INIT" << _BASE_TEXT_ << std::endl;
	std::cout << _YELLOW_ << "--------------------------------------------\n" << _BASE_TEXT_ << std::endl;

	//
	// SEND MESSAGE
	//
	
	for(auto message = messages.begin(); message != messages.end(); ++message)
	{
		this->send_message(
			(*message)["actor"].get<std::string>(),
			(*message)["action"].get<std::string>(),
			(*message)["data"]
		);
	}

	/*
	this->actor_map.send_message("base/example/1","/action1",std::move(nlohmann::json::object()));
	this->actor_map.send_message("base/example/1/test","/action1",std::move(nlohmann::json::object()));
	this->actor_map.send_message("http/session/1","/action1",std::move(nlohmann::json::object()));
	this->actor_map.send_message("http/sf435/34","/action1",std::move(nlohmann::json::object()));
	this->actor_map.send_message("test/sf","/action1",std::move(nlohmann::json::object()));
	this->actor_map.send_message("dhghg/ghshsh/sf","/action1",std::move(nlohmann::json::object()));

	this->actor_map.send_message("base/example/1","/action2",std::move(nlohmann::json::object()));
	this->actor_map.send_message("http/session/1","/action2",std::move(nlohmann::json::object()));
	this->actor_map.send_message("http/listener","/init",std::move(nlohmann::json::object()));
	*/
	
	
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


}	// END namespace node
}	// END namespace tegia