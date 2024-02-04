#include <tegia/actors/types.h>

#include <dlfcn.h>
#include <utility>

#include <tegia/core/const.h>
#include <tegia/tegia.h>

namespace tegia {
namespace actors {


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*


*/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


bool map::load_type(const std::string &type_name, const std::string &base_path, const nlohmann::json &type_config)
{
	//
	// Проверими загружен ли даный тип акторов
	//

	auto pos = this->actor_types.find(type_name);
	if(pos != this->actor_types.end())
	{
		// TODO: write log
		return true;
	}	

	//
	// Загружаем библиотеку актора
	//

	void * lib;
	std::string path = base_path + type_config["path"].get<std::string>();

	lib = dlopen(path.c_str(), RTLD_LAZY);
	if (!lib)
	{
		// TODO: write log

		std::string message = "[" + std::string(dlerror()) + "]";
		std::cout << _ERR_TEXT_ << "load " << type_name << " " << message << std::endl;
		return false;
	}

	//
	// Добавляем в список типов акторов
	//

	auto _fn = ( tegia::actors::type_base * (*)(void) )dlsym(lib,"_load_type");
	auto type = _fn();

	this->actor_types.insert({
		type_name,
		type
	});

	//
	// Добавляем алиасы имен
	//

	for(auto pattern = type_config["patterns"].begin(); pattern != type_config["patterns"].end(); ++pattern)
	{
		std::string _pattern = pattern->get<std::string>();
		std::string tmp = "";

		for(size_t k = 0; k < _pattern.size(); ++k)
		{
			if(_pattern[k] == '/')
			{
				//
				// TODO: проверять дубликаты
				//

				this->name_patterns.insert({tmp,nullptr});
				tmp = tmp + _pattern[k];
			}
			else
			{
				tmp = tmp + _pattern[k];
			}
		}
		this->name_patterns.insert({tmp,type});				
	}

	return true;
};



//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*


*/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////



bool map::add_domain(const std::string &domain, const std::string &type)
{
	this->_domains.insert({domain,type});
	return true;
};



//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*


*/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////



tegia::actors::actor_base * map::create_actor(const std::string &name, tegia::actors::type_base * actor_type)
{
	this->actor_list_mutex.lock();

	actor_t actor;
	actor.name  = name;
	actor.actor = actor_type->create_actor(name,std::move(nlohmann::json::object()));
	actor.type = actor_type;

	this->actor_list.insert({name,actor});
	this->actor_list_mutex.unlock();
	return actor.actor;
};




//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*

	parser/fns/egrul/loader
	parser/fns/egrip/loader
	parser/fns/egrul/parser
	parser/fns/inn
	parser/fns/npd
	parser/mvd/passports/invalid

	-----
	EXAMPLE 1
	-----

	domains:
		1	parser
	actors:
		{1}/fns/egrul/loader
		{1}/fns/egrip/loader
		{1}/fns/egrul/parser
		{1}/fns/inn
		{1}/fns/npd
		{1}/mvd/passports/invalid

	-----
	EXAMPLE 2
	-----

	domains:
		1	parser/fns
		2	parser/mvd

	actors:
		{1}/egrul/loader
		{1}/egrip/loader
		{1}/egrul/parser
		{1}/inn
		{1}/npd
		{2}/passports/invalid

	-----
	EXAMPLE 3
	-----

	domains:
		1	parser/fns
		2	parser/fns/egrul
		3	parser/mvd
	
	actors:
		{2}/loader
		{2}/parser
		{1}/egrip/loader
		{1}/inn
		{1}/npd
		{3}/passports/invalid
*/


std::tuple<bool,std::string,std::string,std::string> resolve_domain(const std::string &name, std::unordered_map<std::string,std::string> * domains)
{
	int state = 0;
	int i = 0;

	bool found_domain = false;
	std::string prev_domain = "";
	std::string domain = "";
	std::string type = "";

	while(true)
	{
		switch(state)
		{
			//
			// 
			//

			case 0:
			{
				if(i == name.size()-1)
				{
					if(found_domain == true)
					{
						/*
						std::cout << "[OK] FOUND DOMAIN" << std::endl;
						std::cout << "name   = " << name << std::endl;
						std::cout << "domain = " << prev_domain << std::endl;
						std::cout << "type   = " << type << std::endl;
						*/

						return std::move(std::make_tuple(true,name,prev_domain,type));
					}

					// ERROR
					// std::cout << _ERR_TEXT_ << "NOT FOUND DOMAIN" << std::endl;
					// exit(0);

					return std::move(std::make_tuple(false,name,"",""));
				}

				if(name[i] == '/')
				{
					state = 10;
					i++;
					break;
				}

				domain = domain + name[i];
				i++;
			}
			break;

			//
			//
			//
			
			case 10:
			{
				// std::cout << "name   = " << name << std::endl;
				// std::cout << "domain = " << domain << std::endl;

				auto pos = domains->find(domain);
				if(pos == domains->end())
				{
					if(found_domain == true)
					{
						/*
						std::cout << "[OK] FOUND DOMAIN" << std::endl;
						std::cout << "name   = " << name << std::endl;
						std::cout << "domain = " << prev_domain << std::endl;
						std::cout << "type   = " << type << std::endl;
						*/

						return std::move(std::make_tuple(true,name,prev_domain,type));
					}
	
					// std::cout << "NOT FOUND " << std::endl;
					domain = domain + "/";
					state = 0;
				}
				else
				{
					prev_domain = domain;
					domain = domain + "/";
					found_domain = true;
					type = pos->second;
					state = 0;
				}
			}
			break;
		}
	}

	return std::move(std::make_tuple(false,"","",""));
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*

	1. Ищем актор по имени в списке локальных загруженных акторов. Следовательно, нужен общий список 
	созданных в ноде акторов.
	* Если актор нашли, то создаем задачу
	* Если актор не нашли, то шаг 2

	2. Выполняем разрешение доменна актора
	* Если домен не найден, то ошибка (как возвращать ошибку?)
	* Если домен удаленный, то формируем транспортный пакет
	* Если домен локальный, то шаг 3

	3. 


	Коды ошибок:
	200 - успешно
	100 - сообщение отправлено на удаленный узел
	4хх - ошибка

	401 - не найден домен
	402 - не найден паттерн имени актора, поэтому не удалось определить тип актора
	403 - not found ection
	
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


std::tuple<int,std::function<void()>> add_task(
	tegia::actors::actor_base * actor,
	tegia::actors::type_base * type,
	const std::string &name, 
	const std::string &action, 
	const std::shared_ptr<message_t> &message)
{
	auto fn = type->bind_action(actor, action, message);
	if(fn != nullptr)
	{
		auto _fn = [=]()
		{
			auto start_time = std::chrono::high_resolution_clock::now();
			
			try
			{
				int result = fn();

				auto callback = message->callback.get();
				if(callback.is_addr == true)
				{
					tegia::message::send(callback.actor, callback.action, message);
				}	
			}
			catch(const std::exception& e)
			{
				std::cout << _ERR_TEXT_ << "[" << name << action << "] " << e.what() << std::endl;
				exit(0);
			}

			auto end_time = std::chrono::high_resolution_clock::now();
		};

		return std::move(std::make_tuple(200,std::move(_fn)));
	}

	std::cout << _ERR_TEXT_ << "send message \n" 
				<< "      [403] not found action\n" 
				<< "      actor  = '" << name << "'\n" 
				<< "      action = '" << action << "'" << std::endl; 
	return std::move(std::make_tuple(403,nullptr));	
};







std::tuple<int,std::function<void()>> map::send_message(const std::string &name, const std::string &action, const std::shared_ptr<message_t> &message)
{
	//
	//
	//

	{
		auto pos = this->actor_list.find(name);
		if(pos != this->actor_list.end())
		{
			return add_task(pos->second.actor,pos->second.type,name,action,message);
			// TODO: add task

			// auto message = std::make_shared<message_t>();
			// message->data = data;

			// auto fn = pos->second.type->bind_action(pos->second.actor, action, message);

		}
	}

	//
	// Определяем к какому домену принадлежит актор
	//

	auto [res,actor_name,actor_domain,domain_type] = resolve_domain(name, &this->_domains);

	//
	// Обрабатываем ошибку
	//

	if(res == false)
	{
		//
		// TODO: ERROR
		//

		std::cout << _ERR_TEXT_ << "401 | NOT FOUND DOMAIN" << std::endl;
		std::cout << "      actor name = " << name << std::endl;
		return std::make_tuple(401,nullptr);
	} 

	//
	// Отправляем траснпортный пакет на удаленный домен
	// 

	if(domain_type == "remote")
	{
		//
		// TODO: Формируем транспортный пакет
		//

		std::cout << _OK_TEXT_ << "100 | REMOTE DOMAIN" << std::endl;
		std::cout << "      actor name   = " << name << std::endl;
		std::cout << "      actor domain = " << actor_domain << std::endl;
		return std::make_tuple(100,nullptr);
	}

	//
	// Определям тип актора по его имени
	//

	{
		auto [res,actor_type] = this->resolve(name);
		if(res == true)
		{
			std::cout << _OK_TEXT_ << "200 | FOUND" << std::endl;
			std::cout << "      actor name = " << name << std::endl;
			std::cout << "      actor type = " << actor_type->get_name() << std::endl;

			//
			// Create actor
			//

			auto actor = this->create_actor(name,actor_type);

			return add_task(actor,actor_type,name,action,message);

			// TODO: add task

			// auto message = std::make_shared<message_t>();
			// message->data = data;

			/*
			auto fn = actor_type->bind_action(actor, action, message);
			if(fn != nullptr)
			{
				auto _fn = [fn]()
				{
					int result = fn();
					// std::cout << "result = [" << result << "]" << std::endl;
				};

				return std::make_tuple(200,std::move(_fn));
			}

			std::cout << _ERR_TEXT_ << "send message \n" 
			          << "      [403] not found action\n" 
					  << "      actor  = '" << name << "'\n" 
					  << "      action = '" << action << "'" << std::endl; 
			return std::make_tuple(403,nullptr);
			*/
		}
		else
		{
			std::cout << _ERR_TEXT_ << "402 | NOT FOUND PATTERN" << std::endl;
			std::cout << "      actor name = " << name << std::endl;
			return std::make_tuple(402,nullptr);
		}
	}
	
	return std::make_tuple(400,nullptr);
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*


*/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


std::tuple<bool,tegia::actors::type_base *> map::resolve(const std::string &name)
{
	std::string prev_pattern = "";
	std::string curr_pattern = "";
	int state = 0;
	size_t k = 0;

	while(true)
	{
		switch(state)
		{
			//
			//
			//

			case 0:
			{
				// std::cout << "[" << k << "] state = 0" << std::endl;

				if(name[k] == '/')
				{
					state = 10;
					break;
				}

				curr_pattern = curr_pattern + name[k];

				if(k == name.size() - 1)
				{
					state = 10;	
					break;
				}

				k++;
			}
			break;

			//
			//
			//

			case 1:
			{
				prev_pattern = prev_pattern + curr_pattern + name[k];
				curr_pattern = "";
				k++;
				state = 0;
			}
			break;

			case 2:
			{
				//
				// TODO: ERROR
				// 

				// std::cout << "k < name.size() - 1" << std::endl;
				return std::make_tuple(false,nullptr);
			}
			break;

			//
			//
			//

			case 10:
			{
				// std::cout << "[" << k << "] state = 10" << std::endl;

				// std::cout << "prev_pattern = " << prev_pattern << std::endl;
				// std::cout << "curr_pattern = " << curr_pattern << std::endl;

				auto pos = this->name_patterns.find(prev_pattern + curr_pattern);
				if(pos == this->name_patterns.end())
				{
					state = 20;
					break;
				}

				if(pos->second == nullptr)
				{
					state = 1;
					break;
				}

				if(k < name.size() - 1)
				{
					state = 2;
					break;
				}

				return std::move(std::make_tuple(true,pos->second));				
			}
			break;

			//
			//
			//

			case 20:
			{
				// std::cout << "[" << k << "] state = 20" << std::endl;

				auto pos = this->name_patterns.find(prev_pattern + "*");
				if(pos == this->name_patterns.end())
				{
					// NOT FOUND
					// std::cout << _ERR_TEXT_ << "NOT FOUND" << std::endl;
					return std::make_tuple(false,nullptr);
				}

				if(pos->second == nullptr)
				{
					curr_pattern = "*";
					state = 1;
					break;
				}

				if(k < name.size() - 1)
				{
					state = 2;
					break;
				}

				return std::move(std::make_tuple(true,pos->second));
			}
			break;
		}
		//	END switch(state)
		
	}


	return std::make_tuple(false,nullptr);
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////



}	// END namespace actors
}	// END namespace tegia