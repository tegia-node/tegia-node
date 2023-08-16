// -------------------------------------------------------------------------------------- //
//                                      INCLUDES                                          //
// -------------------------------------------------------------------------------------- //

	#include "../Node2.h"

	#include <tegia/Actor/ActorList.h>
	#include "../threads/thread.h"

// -------------------------------------------------------------------------------------- //
//                                       CLASS                                            //
// -------------------------------------------------------------------------------------- //

	
ActorList::ActorList()
{
	this->ActorRouter = new std::map<std::string, std::function<std::string(const std::shared_ptr<message_t2>, const nlohmann::json &)> >;
};

	 
void ActorList::init()
{    	
	nlohmann::json confs = context::node()->config["configurations"];

	for(auto it = confs.begin(); it != confs.end(); it++)
	{
		if((*it)["isload"].get<bool>() == true)
		{
			std::string filename = "";
			std::string name = "";
			std::string path = "";

			if((*it)["file"].is_null())
			{
				std::string conf_file = (*it)["conf_file"].get<std::string>();
				path = (*it)["path"].get<std::string>();
				
				name = (*it)["name"].get<std::string>();	
				filename = path + "/bin/" + conf_file;
			}
			else
			{
				name = (*it)["name"].get<std::string>();
				filename	= (*it)["file"].get<std::string>();
			}

			std::cout << std::endl;
			std::cout << "LOAD CONFIGURATION: [\033[32;1m" << name << "\033[0m] [" << filename << "]" << std::endl;

			// Читаем файл конфигурации
			if( !std::filesystem::exists(filename) )
			{
				std::cout << _RED_TEXT_ << "configurations file not found" << _BASE_TEXT_ << std::endl;				
				exit(0);
			}

			nlohmann::json conf = core::json::file(filename);

			if(conf["path"].is_null() == true)
			{
				conf["path"] = path;
			}
			core::json::save(filename,conf,1,'\t');
			
			path = conf["path"].get<std::string>();
					
			// Если есть секция "db"   
			if(conf["db"].is_null() == false)
			{
				for(auto it = conf["db"].begin(); it != conf["db"].end(); it++)   
				{ 
					db::mysql::add( (*it) );                                                                
				}
			}

			// Если есть секция "loads"
			if(conf["loads"].is_null() == false)
			{
				for(auto loads = conf["loads"].begin(); loads != conf["loads"].end(); loads++)
				{
					this->loadActorType( path, (*loads).get<std::string>() );
				}

				for(auto actors = conf["actors"].begin(); actors != conf["actors"].end(); actors++)
				{
					std::string actor_name = (*actors)["name"].get<std::string>();
					this->create((*actors)["type"].get<std::string>(), actor_name, nlohmann::json() )( nlohmann::json() ); 
				}                  
			}
			else
			{
				for(auto actors = conf["actors"].begin(); actors != conf["actors"].end(); actors++)
				{
					std::cout << "   "; 
					this->loadActorType( path, (*actors)["type"].get<std::string>() );

					std::string actor_name = (*actors)["name"].get<std::string>();

					if(actor_name != "#notcreate")
					{
						this->create((*actors)["type"].get<std::string>(), actor_name, nlohmann::json() )( nlohmann::json() );
					}						
				}
			}

			// Если есть секция "init"   
			if(conf["init"].is_null() == false)
			{
				for(auto init = conf["init"].begin(); init != conf["init"].end(); init++)
				{
					Platform::sendMessage( (*init)["name"].get<std::string>(),(*init)["action"].get<std::string>(),(*init)["message"]);
				}
			}
		}
	} // for(auto it = confs.begin(); it != confs.end(); it++)


/*
	std::cout << "RUN TEST ACTORS" << std::endl;
	for(int i = 0; i < 50000; i++)
	{
		std::string actor_name = "egrul_" + std::to_string(i);
		this->create("GEgrul", actor_name, nlohmann::json() )( nlohmann::json() );
	}
	std::cout << "END TEST ACTORS" << std::endl;
*/
		 
}; 



ActorList::~ActorList()
{   
	// Очистить все объекты
	for(auto it = this->actors.begin(); it != this->actors.end(); it++)
	{
		delete (it->second);
	}
	this->actors.clear();
};


/////////////////////////////////////////////////////////////////////////////////////////////
std::function<int(const nlohmann::json &)> ActorList::create(const std::string & type, const std::string & name, nlohmann::json data)
{
	auto it = this->actorFactory.find(type);
	if (it != this->actorFactory.end())
	{
		auto actor = it->second(name,data);

		std::cout << _OK_TEXT_ << " create actor " << name << " [\033[37m" << type << "\033[0m]" << std::endl;

		return std::bind([] (auto actor, const nlohmann::json &data) { 
			return actor->init(data); }, actor, std::placeholders::_1);
		}

		auto ret = std::bind([type, name] (const nlohmann::json &data) { 
			std::cout << _ERR_TEXT_ << " create actor " << name << " \033[31m not found factory " << type << "\033[0m" << std::endl;
			return 0; }, std::placeholders::_1);
		return ret;
};




/////////////////////////////////////////////////////////////////////////////////////////////
//

std::string ActorList::SendMessageS(const std::string & actor, const std::shared_ptr<message_t2> message)
{  LOG_TRACE 
	//////////////////////////////////////////////////////////////////////////////////////////

	// Ищем нужный актор
	for(auto it = this->ActorRouter->begin(); it != this->ActorRouter->end(); it++)
	{
		auto res = this->compare(it->first, actor);
		if(res.first == true)
		{
			return it->second(message,res.second);
		}
	}
	return "";
};





/////////////////////////////////////////////////////////////////////////////////////////////
//

std::string ActorList::SendMessage(
	const std::string & actor, 
	const std::shared_ptr<message_t2> message,
	int priority)
{	LOG_TRACE 
	//////////////////////////////////////////////////////////////////////////////////////////

	std::cout << "send message: " << actor << std::endl;
		 
	// Ищем нужный актор
	for(auto it = this->ActorRouter->begin(); it != this->ActorRouter->end(); it++)
	{
		auto res = this->compare(it->first, actor);
		if(res.first == true)
		{
			// message->address = actor_name;
			ThreadPool *pool = ThreadPool::instance();
			pool->createJob(std::make_shared<fn_type>(std::bind(&ActorList::execute, this, actor, it->first, it->second, message, res.second)), priority );
			return message->getUUID();
		}
	}

	return "#notfound";
};







	 /////////////////////////////////////////////////////////////////////////////////////////////
	 //
	 bool ActorList::find(const std::string &name)
	 {  LOG_TRACE 
			//////////////////////////////////////////////////////////////////////////////////////////

				 for(auto it = this->ActorRouter->begin(); it != this->ActorRouter->end(); it++)
				 {
						auto res = this->compare(it->first, name);
						if(res.first == true)
						{
							 return true;
						}
				 }

				 std::cout << "\033[31mactor " << name << " not found\033[0m" << std::endl;

				 return false;

			
			/*
			
			старый вариант поиска

			auto it = this->actors_worker.find(name);
			if (it != this->actors_worker.end())
			{
				 return true;
			}
			else
			{
				 std::cout << "\033[31mactor " << name << " not found\033[0m" << std::endl;
				 return false;
			}*/

	 };

	
/////////////////////////////////////////////////////////////////////////////////////////////
//
//  Функция обеспечиваает загрузку библиотеки актора
//
/////////////////////////////////////////////////////////////////////////////////////////////

bool ActorList::loadActorType(const std::string &path, const std::string &type)
{
	void * lib;
	std::string lib_name = path + "/bin/lib" + type + ".so";

	lib = dlopen(lib_name.c_str(), RTLD_LAZY);
	if (!lib)
	{
		std::string message = "[" + std::string(dlerror()) + "]";
		std::cout << _ERR_TEXT_ " load " << type << " " << message << std::endl;
		return false;
	}

	this->actorFactory.insert(std::make_pair(type,( Actor2*(*)(const std::string &, nlohmann::json) )dlsym(lib,"new_actor")));
	return true;
};





/////////////////////////////////////////////////////////////////////////////////////////////
//

void ActorList::add_route(
	const std::string &name, 
	std::function<std::string(const std::shared_ptr<message_t2>, const nlohmann::json &)> fn)
{	LOG_TRACE 
	//////////////////////////////////////////////////////////////////////////////////////////
	
	// TODO: !!! проверка на уникальность !!!
	this->ActorRouter->insert ( std::make_pair (name, fn) );

	//////////////////////////////////////////////////////////////////////////////////////////
};
	


/////////////////////////////////////////////////////////////////////////////////////////////
//

void ActorList::execute(
	const std::string &name, 
	const std::string &mask,
	std::function<std::string(const std::shared_ptr<message_t2>, const nlohmann::json &)> fn,
	const std::shared_ptr<message_t2> message,
	const nlohmann::json &route_params)
{	LOG_TRACE 
	//////////////////////////////////////////////////////////////////////////////////////////

	auto start_time = std::chrono::high_resolution_clock::now();
			std::string res = fn(message,route_params);

			//////////////////////////////////////////////////////////////////////////////////////////
			//
			//  Запоминаем какой актор сейчас выполнялся
			//
			//////////////////////////////////////////////////////////////////////////////////////////
			
			nlohmann::json tmp;
			tmp["name"] = name; 
			tmp["status"] = res;
			message->data["actors"].push_back(tmp);

			//////////////////////////////////////////////////////////////////////////////////////////
			//
			//  
			//
			//////////////////////////////////////////////////////////////////////////////////////////

			if(message->pipeline.is_object() == true)
			{
				 try
				 {
						bool flag = false;
						if("callback" == message->pipeline.at(mask).at(res).get<std::string>())
						{
							 message->pipeline = nlohmann::json();
							 flag = true;
						}

						if("END" == message->pipeline.at(mask).at(res).get<std::string>())
						{
							 message->pipeline = nlohmann::json();
							 return;
						}

						if(flag == false)
						{
							 std::string next_actor = message->pipeline.at(mask).at(res).get<std::string>();
							 //this->sendMessage2(next_actor,message);
							 this->SendMessage(next_actor,message); 
						}            
				 }

				 catch (nlohmann::json::out_of_range& e)
				 {
						std::cout << mask << " not found" << std::endl;
						exit(0);
				 }
			}

			//////////////////////////////////////////////////////////////////////////////////////////
			//
			//  
			//
			//////////////////////////////////////////////////////////////////////////////////////////

			if(message->pipeline.is_null() == true)
			{
				 if(message->callback.size() > 0)
				 {
				 // Получаем информацию о callback
						if(!message->callback.empty())
						{
							 LDEBUG("[" << message->getUUID() << "] next actor: " << message->callback.top().actor << message->callback.top().action );

							 /*

								 REM: При выполнении sendMessage с уже существующим сообщением возможна следующая проблема:
											- один актор уже отработал и вызвал callback, но не успел удалить callback из списка;
											- второй актор успел войти в if(!message->callback.empty())
											- первый актор удаляет callback
											- второй актор падает про попытке выполнить message->callback.pop();

								 TODO: Нужно сделать защиту от "дурака" !!!

											 1. Заменить все callback на pipeline
											 2. Добавить проверки/блокировки - вынести получение callback в отдельную функцию message_t2 и поставить на нее мьютекс
							 */

							 auto callback = message->callback.top();
							 message->action = callback.action;
							 message->recipient = callback.actor;

							 message->callback.pop();

							 Platform::Node::instance()->SendMessage(callback.actor + callback.action, message);
						}
				 }       
			}

			auto end_time = std::chrono::high_resolution_clock::now();

			//////////////////////////////////////////////////////////////////////////////////////////
	 };


	 /////////////////////////////////////////////////////////////////////////////////////////////
	 //
	 /*void ActorList::_execute(const std::string &name, const std::shared_ptr<message_t2> message)
	 {  LOG_TRACE 
			//////////////////////////////////////////////////////////////////////////////////////////
			
			auto start_time = std::chrono::high_resolution_clock::now();

			std::cout << "======================================" << std::endl;
			std::cout << "ActorList::execute " << name << " [" << ThreadPool::instance()->getCurrThread()->getTID() << "]" << std::endl;

	 // Ищем нужный актор
			for(auto it = this->ActorRouter->begin(); it != this->ActorRouter->end(); it++)
			{
				 auto res = this->compare(it->first, name);
				 if(res.first == true)
				 {
						std::cout << "res.second: " << core::cast<std::string>(res.second) << std::endl;
						it->second(message,res.second);
				 }
			}

			// TODO: если не нашли??


			auto end_time = std::chrono::high_resolution_clock::now();

			std::cout << std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time).count() << "s "
								<< std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count() << "ms" << std::endl;
			std::cout << "======================================" << std::endl;

			//////////////////////////////////////////////////////////////////////////////////////////
	 };*/
	





	std::pair<bool,nlohmann::json> ActorList::compare(std::string pattern, std::string path)
	{
		nlohmann::json root;
		//cout<<"pattern = "<<pattern<<endl;
		//cout<<"path = "<<path<<endl;

		std::vector<std::string> arr_pattern = core::explode(pattern,"/",false);
		std::vector<std::string> arr_path = core::explode(path,"/",false);

		if(arr_pattern.size() != arr_path.size())
		{
			//cout<<"route not found"<<endl<<endl;
			return std::pair<bool,nlohmann::json>(false,root);
		}

		// state = 0  - начальное состояние
		// state = 1  - пока вектора равны
		// state = 2  - вектора не равны

		int state = 0;
	 	
		for(int i = 0; i < arr_pattern.size(); i++)
		{
			if(arr_pattern[i] == arr_path[i])
			{
				state = 1;
			}
			else
			{
				// Обрабатываем ситуацию с шаблонными параметрами
				std::string start_pattern = arr_pattern[i].substr(0,1);

				//
				//std::cout << "start_pattern = " << start_pattern << std::endl;
									
				if(start_pattern == "{")
				{
					state = 1;
					// Формируем пару "имя = значение"
					root[arr_pattern[i].substr(1, arr_pattern[i].size()-2)] = arr_path[i];

					//std::cout << "==============================\n";
					//std::cout << "name = " << arr_pattern[i].substr(1, arr_pattern[i].size()-2) 
					//          << "; value = " << arr_path[i] << std::endl;
				}
				else
				{
					state = 2;
					//cout<<"route not found"<<endl<<endl;
					return std::pair<bool,nlohmann::json>(false,root);
				}
			}
		}
		return std::pair<bool,nlohmann::json>(true,root);
	};