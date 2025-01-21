#include "provider.h"

//	LOGGER
#include "../../node/logger.h"
#undef _LOG_LEVEL_
#define _LOG_LEVEL_ _LOG_DEBUG_
#include "../../node/log.h"


////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// PROVIDER CLASS                                                                         //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

namespace tegia { 
namespace mysql {


provider::provider()
{
	LDEBUG("INTI MYSQL PROVIDER");
};

provider::~provider()
{
	//std::cout << "delete mysql provider" << std::endl;
};


/////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
/////////////////////////////////////////////////////////////////////////////////////////////////


bool provider::add_database(const std::string &name, nlohmann::json config)
{
	auto context = this->_contexts.find(name);
	if(context != this->_contexts.end())
	{
		// TODO
		std::cout << _ERR_TEXT_ << "context " << name << " is init" << std::endl;
	}
	else
	{
		auto conn = this->_connections.find(config["connection"].get<std::string>());
		if(conn == this->_connections.end())
		{
			// Error
			return false;
		}
		else
		{
			context_t context;
			context.dbname = config["dbname"].get<std::string>();
			context.connection_name = name;
			context.connection = conn->second;
			this->_contexts.insert({name,context});
		}
	}
	return true;
};


/////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
/////////////////////////////////////////////////////////////////////////////////////////////////


bool provider::add_connection(const std::string &name, nlohmann::json config)
{
	//
	// TODO: проверить корректность данных
	//

	/*
	std::cout << "name     = " << name << std::endl;
	std::cout << "type     = " << config["type"].get<std::string>() << std::endl;
	std::cout << "host     = " << config["host"].get<std::string>() << std::endl;
	std::cout << "password = " << config["password"].get<std::string>() << std::endl;
	std::cout << "port     = " << config["port"].get<int>() << std::endl;
	std::cout << "user     = " << config["user"].get<std::string>() << std::endl;
	*/

	//
	// Выполнить соединение с БД
	//

	auto conn = this->_connections.find(name);
	if(conn != this->_connections.end())
	{
		// TODO
		std::cout << _ERR_TEXT_ << "connection " << name << " is init" << std::endl;

	}
	else
	{
		auto connection = new tegia::mysql::connection(
			name,
			config["type"].get<std::string>(),
			config["host"].get<std::string>(),
			config["port"].get<int>(),
			config["user"].get<std::string>(),
			config["password"].get<std::string>()
		);

		auto [code,message] = connection->connect();
		if(code > 0)
		{
			std::cout << _ERR_TEXT_ << "connection " << name << " init error: " << message << std::endl;
			LERROR("connection '" + name + "' init error: " + message);
			exit(0);
		}
		else
		{
			this->_connections.insert({name,connection});
			
			// std::cout << _OK_TEXT_ << "connection " << name << " init" << std::endl;
			LNOTICE("connection '" + name + "' init");
		}

	}

	return true;
};



tegia::mysql::records * provider::query(const std::string &name, const std::string &query, bool trace)
{
	auto conn = this->_contexts.find(name);
	if(conn == this->_contexts.end())
	{
		std::cout << _ERR_TEXT_ << "connection '" << name << "' not found" << std::endl;
		exit(0);
		// return nullptr;
	}

	return conn->second.connection->query(conn->second.dbname, query);
};


std::string provider::strip(const std::string &input)
{
	auto conn = this->_contexts.begin();
	return conn->second.connection->strip(input);
};



}   // end namespace mysql
}   // end namespace tegia


