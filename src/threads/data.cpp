#include "data.h"

// LOGGER
#include "../node/logger.h"
#undef _LOG_LEVEL_
#define _LOG_LEVEL_ _LOG_NOTICE_
#include "../node/log.h"
// LOGGER


namespace tegia {
namespace threads {

_data::_data()
{
	this->user = new ::tegia::user();
	this->mysql_provider = new ::tegia::mysql::provider();

	std::cout << "create thread data" << std::endl;
};


_data::~_data()
{
	delete this->user;
	delete this->mysql_provider;

	std::cout << "delete thread data" << std::endl;
};

void _data::init(const nlohmann::json &config)
{
	std::cout << "init context" << std::endl;
	this->tid = core::cast<std::string>(std::this_thread::get_id());

	//
	// Соединение с БД
	//
	
	if(config.contains("connections") == true)
	{
		for (auto it = config["connections"].begin(); it != config["connections"].end(); ++it)
		{
			this->mysql_provider->add_connection(it.key(),it.value());
		}
	}

	if(config.contains("databases") == true)
	{
		for (auto it = config["databases"].begin(); it != config["databases"].end(); ++it)
		{
			std::cout << "db_name = " << it.key() << std::endl;
			
			this->mysql_provider->add_database(it.key(),it.value());
		}
	}

	std::cout << "[init] tid = " << this->tid << std::endl;
};


tegia::mysql::records * _data::query(const std::string &context,const std::string &query, bool trace)
{
	return this->mysql_provider->query(context,query,trace);
};


thread_local tegia::threads::_data * const data = new tegia::threads::_data();

}  // namespace threads
}  // namespace tegia



// --------------------------------------------------------------------