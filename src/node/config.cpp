#include <tegia/const.h>
#include "config.h"


namespace tegia {
namespace node {

config::config(){};
config::~config(){};

bool config::load()
{
	//
	// NODE CONFIG
	//

	nlohmann::json node_conf;

	{
		int code;
		std::string message;
		std::tie(code,message,node_conf) = tegia::json::_file("./config.json");

		auto conf = new _conf();
		conf->name = "node";
		conf->file = "./config.json";
		conf->data = node_conf;
		this->_map.insert({conf->name,conf});

		//
		// thread_count
		//

		if(node_conf.contains("/thread/count"_json_pointer) == true)
		{
			this->thread_count = node_conf["/thread/count"_json_pointer].get<int>();
		}
	}

	//
	// VALIDATORS
	//

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

	//
	// READ CONFIGURATONS
	//

	nlohmann::json dbs = nlohmann::json::object();

	for(auto config = node_conf["configurations"].begin(); config != node_conf["configurations"].end(); ++config)
	{
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
			exit(0);
		}

		if((*config)["isload"].get<bool>() == false)
		{
			continue;
		}

		auto conf = new _conf();
		conf->name = (*config)["name"].get<std::string>();
		conf->file = (*config)["file"].get<std::string>();

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
		this->_names.push_back(conf->name);

		//
		// Читаем секцию DB
		//

		for(auto db = conf->data["db"].begin(); db != conf->data["db"].end(); ++db)
		{
			std::string name = db.key();
			// std::cout << "name = " << db.key() << std::endl;

			try
			{
				validator_db.validate( db.value() );
			}

			catch(const std::exception& e)
			{
				std::cout << _ERR_TEXT_ << "db '" << name << "' not valud \n" << e.what() << std::endl;
				exit(0);
			}

			if(dbs.contains(name) == false)
			{
				dbs["db"][name] = db.value();
			}
			else
			{
				if(dbs["db"][name] != db.value())
				{
					std::cout << _ERR_TEXT_ << "not equal '" << name << "'" << std::endl;
					std::cout << "old: \n" << _YELLOW_ << dbs["db"][name] << _BASE_TEXT_ << std::endl;
					std::cout << "new: \n" << _YELLOW_ << db.value() << _BASE_TEXT_ << std::endl;
					exit(0);
				}
			}
			
			// std::cout << "data = " << db.value() << std::endl;			
		}

		std::cout << _OK_TEXT_ << "ADD CONFIG" << std::endl;
		std::cout << "      name = " << conf->name << std::endl;
		std::cout << "      file = " << conf->file << std::endl;
		// std::cout << _YELLOW_ << conf->data << _BASE_TEXT_ << std::endl;
		std::cout << " " << std::endl;
	}

	{
		auto conf = new _conf();
		conf->name = "_db";
		conf->file = "";
		conf->data = dbs;
		this->_map.insert({conf->name,conf});
	}


	return true;
};


const nlohmann::json * const config::get(const std::string &name)
{
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
