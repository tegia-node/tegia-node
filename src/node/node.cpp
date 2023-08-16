#include <tegia2/context/context.h>
#include <tegia2/db/mysql/mysql.h>

#include <tegia2/core/json.h> 

#include "node.h"
#include "../threads/pool.h"
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

	bool node::action()
	{
		std::cout << _YELLOW_ << "\n--------------------------------------------" << _BASE_TEXT_ << std::endl;
		std::cout << _YELLOW_ << "ACTION NODE" << _BASE_TEXT_ << std::endl;
		std::cout << _YELLOW_ << "--------------------------------------------\n" << _BASE_TEXT_ << std::endl;

		LNOTICE("\n--------------------------------------------\nACTION NODE\n--------------------------------------------\n")

		//
		// Загружаем конфигурации
		//

		for(auto conf = this->_config->_map.begin(); conf != this->_config->_map.end(); ++conf)
		{
			if(conf->first == "_db") continue;
			if(conf->first == "node") continue;

			std::cout << "LOAD " << conf->first << std::endl; 
			std::cout << conf->second->data << std::endl; 
		}



	
		long long int i = 0;
		while(i < 5000000)
		{
			auto _fn = std::bind(fn,i);

			std::cout << _YELLOW_ << "[" << i << "]" << _BASE_TEXT_ << std::endl;

			// auto threads = tegia::threads::pool::instance();
			this->_threads->add_task(_fn,0);

			i++;
		}


		return true;
	};

	bool node::run()
	{
		std::cout << _YELLOW_ << "\n--------------------------------------------" << _BASE_TEXT_ << std::endl;
		std::cout << _YELLOW_ << "RUN NODE" << _BASE_TEXT_ << std::endl;
		std::cout << _YELLOW_ << "--------------------------------------------\n" << _BASE_TEXT_ << std::endl;

		LNOTICE("\n--------------------------------------------\nRUN NODE\n--------------------------------------------\n")

		//
		// Инициализируем хапись журнала
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
			(*_conf_db), 
			std::bind(&tegia::node::node::action,this)
		);

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