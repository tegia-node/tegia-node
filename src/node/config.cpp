#include <tegia/core/const.h>
#include "config.h"


namespace tegia {
namespace node {

config::config()
{
	this->messages = nlohmann::json::array();
	this->validator_db.set_root_schema(nlohmann::json::parse(R"(
	{
		"$schema": "http://json-schema.org/draft-07/schema#",
		"title": "db schema",
		"type": "object",
		"properties": 
		{
			"type":
			{
				"description": "Тип подключения",
				"type": "string"	
			},
			"dbname":
			{
				"description": "",
				"type": "string"
			},
			"host":
			{
				"description": "",
				"type": "string"				
			},
			"port":
			{
				"description": "",
				"type": "number"				
			},
			"user":
			{
				"description": "",
				"type": "string"				
			},
			"password":
			{
				"description": "",
				"type": "string"				
			}
		},
		"required": ["type","dbname","host","port","user","password"]
	})"));
};


config::~config(){};


/////////////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////


conf_t * config::load(const std::string &name, const std::string &file)
{
	auto conf = new conf_t();
	conf->name = name;
	conf->file = file;

	int code;
	std::string message;
	std::tie(code,message,conf->data) = tegia::json::_file(conf->file);
	
	if(code != 0)
	{
		std::cout << _ERR_TEXT_ << "LOAD CONFIG FILE" << std::endl;			
		std::cout << "      name    = " << conf->name << std::endl;
		std::cout << "      file    = " << conf->file << std::endl;
		std::cout << "      message = " << message << std::endl;
		exit(0);
	}

	if(conf->data.contains("init") == true)
	{
		for(auto message = conf->data["init"].begin(); message != conf->data["init"].end(); ++message)
		{
			this->messages.push_back( (*message) );
		}
	}

	this->_map.insert({conf->name,conf});
	return conf;
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////


const nlohmann::json * config::cluster()
{
	auto config = this->load("cluster","./configs/cluster.json");

	//
	// thread_count
	//

	if(config->data.contains("/nodes/data/thread/count"_json_pointer) == true)
	{
		this->thread_count = config->data["/nodes/data/thread/count"_json_pointer].get<int>();
	}

	/*
	std::cout << _OK_TEXT_ << "LOAD CONFIG" << std::endl;
	std::cout << "      name = " << config->name << std::endl;
	std::cout << "      file = " << config->file << std::endl;
	std::cout << "      path = " << config->path << std::endl;
	*/

	return const_cast<const nlohmann::json * const>(&config->data);
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////


const nlohmann::json * config::configuration(const std::string &name)
{
	auto config = this->load(name,"./configurations/" + name + ".json");

	//
	// CONNECTIONS
	//

	for(auto db = config->data["db"]["connections"].begin(); db != config->data["db"]["connections"].end(); ++db)
	{
		std::string _name = db.key();
		// std::cout << "db connection name = " << name << std::endl;

		/*
		try
		{
			validator_db.validate( db.value() );
		}

		catch(const std::exception& e)
		{
			std::cout << _ERR_TEXT_ << "db '" << name << "' not valud \n" << e.what() << std::endl;
			exit(0);
		}
		*/

		if(this->dbc["connections"].contains(_name) == false)
		{
			this->dbc["connections"][_name] = db.value();
		}
		else
		{
			if(this->dbc["connections"][_name] != db.value())
			{
				std::cout << _ERR_TEXT_ << "not equal '" << _name << "'" << std::endl;
				std::cout << "old: \n" << _YELLOW_ << this->dbc["connections"][_name] << _BASE_TEXT_ << std::endl;
				std::cout << "new: \n" << _YELLOW_ << db.value() << _BASE_TEXT_ << std::endl;
				exit(0);
			}
		}
	}

	//
	// DATABASES
	//

	for(auto db = config->data["db"]["databases"].begin(); db != config->data["db"]["databases"].end(); ++db)
	{
		std::string _name = db.key();
		// std::cout << "db name = " << _name << std::endl;

		/*
		try
		{
			validator_db.validate( db.value() );
		}

		catch(const std::exception& e)
		{
			std::cout << _ERR_TEXT_ << "db '" << _name << "' not valud \n" << e.what() << std::endl;
			exit(0);
		}
		*/

		if(this->dbc["databases"].contains(_name) == false)
		{
			this->dbc["databases"][_name] = db.value();
		}
		else
		{
			if(dbc["databases"][_name] != db.value())
			{
				std::cout << _ERR_TEXT_ << "not equal '" << _name << "'" << std::endl;
				std::cout << "old: \n" << _YELLOW_ << this->dbc["databases"][_name] << _BASE_TEXT_ << std::endl;
				std::cout << "new: \n" << _YELLOW_ << db.value() << _BASE_TEXT_ << std::endl;
				exit(0);
			}
		}
	}

	//
	// PATH
	//

	// std::cout << conf->data << std::endl;
	config->path = config->data["path"].get<std::string>();

	/*
	std::cout << _OK_TEXT_ << "LOAD CONFIG" << std::endl;
	std::cout << "      name = " << config->name << std::endl;
	std::cout << "      file = " << config->file << std::endl;
	std::cout << "      path = " << config->path << std::endl;
	*/

	return const_cast<const nlohmann::json * const>(&config->data);
};




bool config::load()
{
	///////////////////////////////////////////////////////////////////////////////////////
	//
	// NODE CONFIG
	//
	///////////////////////////////////////////////////////////////////////////////////////

	nlohmann::json node_conf;

	{
		int code;
		std::string message;
		std::tie(code, message, node_conf) = tegia::json::_file("./configs/cluster.json");
		
		auto conf = new conf_t();
		conf->name = "node";
		conf->file = "./configs/cluster.json";
		conf->data = node_conf;
		this->_map.insert({conf->name,conf});

		//
		// thread_count
		//

		if(node_conf.contains("/nodes/data/thread/count"_json_pointer) == true)
		{
			this->thread_count = node_conf["/nodes/data/thread/count"_json_pointer].get<int>();
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////
	//
	// VALIDATORS
	//
	///////////////////////////////////////////////////////////////////////////////////////

	nlohmann::json_schema::json_validator validator_conf;
	validator_conf.set_root_schema(nlohmann::json::parse(R"(
	{
		"$schema": "http://json-schema.org/draft-07/schema#",
		"title": "config schema",
		"type": "object",
		"properties": 
		{
			"file":
			{
				"description": "Путь до файла конфигурации",
				"type": "string"	
			},
			"isload":
			{
				"description": "Флаг, определяющий загружать конфигурацию или нет",
				"type": "boolean"
			},
			"name":
			{
				"description": "Имя конфигурации",
				"type": "string"				
			}
		},
		"required": ["file","isload","name"]
	})"));

	nlohmann::json_schema::json_validator validator_db;
	validator_db.set_root_schema(nlohmann::json::parse(R"(
	{
		"$schema": "http://json-schema.org/draft-07/schema#",
		"title": "db schema",
		"type": "object",
		"properties": 
		{
			"type":
			{
				"description": "Тип подключения",
				"type": "string"	
			},
			"dbname":
			{
				"description": "",
				"type": "string"
			},
			"host":
			{
				"description": "",
				"type": "string"				
			},
			"port":
			{
				"description": "",
				"type": "number"				
			},
			"user":
			{
				"description": "",
				"type": "string"				
			},
			"password":
			{
				"description": "",
				"type": "string"				
			}
		},
		"required": ["type","dbname","host","port","user","password"]
	})"));


	///////////////////////////////////////////////////////////////////////////////////////
	//
	// READ CONFIGURATONS
	//
	///////////////////////////////////////////////////////////////////////////////////////


	nlohmann::json dbs = nlohmann::json::object();
	dbs["connections"] = nlohmann::json::object();
	dbs["databases"]   = nlohmann::json::object();

	for(auto config = node_conf["configurations"].begin(); config != node_conf["configurations"].end(); ++config)
	{
		/*
		try
		{
			validator_conf.validate( (*config) );
		}

		catch (const std::exception &e)
		{
			std::cout << _ERR_TEXT_ << "parse_error" << std::endl;
			std::cout << "      configuration description in ./config.json must be:" << std::endl;
			std::cout << "      {" << std::endl;
			std::cout << "         \"isload\": <bool>," << std::endl;
			std::cout << "         \"name\": <string>," << std::endl;
			std::cout << "         \"file\": <string: path to config file>" << std::endl;
			std::cout << "      }" << std::endl;
			std::cout << config->dump() << std::endl;
			exit(0);
		}
		*/

		if(config->get<bool>() == false)
		{
			continue;
		}

		auto conf = new conf_t();
		conf->name = config.key();
		conf->file = "./configurations/" + conf->name + ".json";

		// std::cout << "conf" << std::endl;
		// std::cout << "  name = " << conf->name << std::endl;
		// std::cout << "  file = " << conf->file << std::endl;

		int code;
		std::string message;
		std::tie(code,message,conf->data) = tegia::json::_file(conf->file);
		
		if(code != 0)
		{
			std::cout << _ERR_TEXT_ << "ADD CONFIG" << std::endl;			
			std::cout << "      name    = " << conf->name << std::endl;
			std::cout << "      file    = " << conf->file << std::endl;
			std::cout << "      message = " << message << std::endl;
			exit(0);
		}

		this->_map.insert({conf->name,conf});
		// this->_names.push_back(conf->name);

		//
		// CONNECTIONS
		//

		for(auto db = conf->data["db"]["connections"].begin(); db != conf->data["db"]["connections"].end(); ++db)
		{
			std::string name = db.key();
			// std::cout << "db connection name = " << name << std::endl;

			/*
			try
			{
				validator_db.validate( db.value() );
			}

			catch(const std::exception& e)
			{
				std::cout << _ERR_TEXT_ << "db '" << name << "' not valud \n" << e.what() << std::endl;
				exit(0);
			}
			*/

			if(dbs["connections"].contains(name) == false)
			{
				dbs["connections"][name] = db.value();
			}
			else
			{
				if(dbs["connections"][name] != db.value())
				{
					std::cout << _ERR_TEXT_ << "not equal '" << name << "'" << std::endl;
					std::cout << "old: \n" << _YELLOW_ << dbs["connections"][name] << _BASE_TEXT_ << std::endl;
					std::cout << "new: \n" << _YELLOW_ << db.value() << _BASE_TEXT_ << std::endl;
					exit(0);
				}
			}
		}

		//
		// DATABASES
		//

		for(auto db = conf->data["db"]["databases"].begin(); db != conf->data["db"]["databases"].end(); ++db)
		{
			std::string name = db.key();
			// std::cout << "db name = " << name << std::endl;

			/*
			try
			{
				validator_db.validate( db.value() );
			}

			catch(const std::exception& e)
			{
				std::cout << _ERR_TEXT_ << "db '" << name << "' not valud \n" << e.what() << std::endl;
				exit(0);
			}
			*/

			if(dbs["databases"].contains(name) == false)
			{
				dbs["databases"][name] = db.value();
			}
			else
			{
				if(dbs["databases"][name] != db.value())
				{
					std::cout << _ERR_TEXT_ << "not equal '" << name << "'" << std::endl;
					std::cout << "old: \n" << _YELLOW_ << dbs["databases"][name] << _BASE_TEXT_ << std::endl;
					std::cout << "new: \n" << _YELLOW_ << db.value() << _BASE_TEXT_ << std::endl;
					exit(0);
				}
			}
		}

		//
		// PATH
		//

		// std::cout << conf->data << std::endl;
		conf->path = conf->data["path"].get<std::string>();

		std::cout << _OK_TEXT_ << "ADD CONFIG" << std::endl;
		std::cout << "      name = " << conf->name << std::endl;
		std::cout << "      file = " << conf->file << std::endl;
		std::cout << "      path = " << conf->path << std::endl;
		// std::cout << " " << std::endl;
	}

	{
		auto conf = new conf_t();
		conf->name = "dbc";
		conf->file = "";
		conf->path = "";
		conf->data = dbs;
		this->_map.insert({conf->name,conf});
	}

	return true;
};



const nlohmann::json * const config::get(const std::string &name)
{
	if(name == "dbc")
	{
		return const_cast<const nlohmann::json * const>(&this->dbc);
	}

	if(name == "messages")
	{
		return const_cast<const nlohmann::json * const>(&this->messages);
	}

	auto pos = this->_map.find(name);
	if(pos != this->_map.end())
	{
		return const_cast<const nlohmann::json * const>(&pos->second->data);
	}
	else
	{
		return nullptr;
	}
};



}	// END namespace node
}	// EMD namespace tegia
