#include <dlfcn.h>
#include <utility>

#include <tegia/core/const.h>
#include <tegia/context/context.h>
#include "../threads/data.h"

#include "map.h"

namespace tegia {
namespace actors {


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


int map_t::add_domain(const std::string &name, int type)
{
	auto domain = new tegia::actors::domain_t();
	domain->_name = name;
	domain->_type = type;

	auto [pos,code] = this->_domains.insert({name,domain});
	if(code == false)
	{
		delete domain;
	}

	return 0;
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


int map_t::add_type(
	const std::string &type_name, 
	const std::string &base_path, 
	nlohmann::json * data)
{
	//
	// CHECK TYPE
	//

	{
		auto pos = this->_types.find(type_name);
		if(pos != this->_types.end()) return 0;
	}

	//
	// CHECK DATA
	//

	if(data->contains("patterns") == false || (*data)["patterns"].is_array() == false)
	{
		return 10;
	}

	//
	// LOAD LIBRARY
	//

	void * lib;
	std::string path = base_path + (*data)["path"].get<std::string>();

	lib = dlopen(path.c_str(), RTLD_LAZY);
	if (!lib)
	{
		// TODO: write log

		std::string message = "[" + std::string(dlerror()) + "]";
		std::cout << _ERR_TEXT_ << "load " << type_name << " " << message << std::endl;
		return 20;
	}

    // Получение указателя на функцию
	auto _fn = ( tegia::actors::type_base_t * (*)(void) )dlsym(lib,"_init_type");
    const char* dlsym_error = dlerror();
    if (dlsym_error) {
        std::cerr << "Cannot load symbol '_init_type': " << dlsym_error << '\n';
        dlclose(lib);
        return 30;
    }

	//
	// ADD TYPE
	//

	auto _type = _fn();
	this->_types.insert({type_name,_type});

	//
	// ADD PATTERNS
	//

	for(auto it = (*data)["patterns"].begin(); it != (*data)["patterns"].end(); ++it)
	{
		std::string tmp = "";
		std::string pattern = it->get<std::string>();

		for(size_t k = 0; k < pattern.size(); ++k)
		{
			if(pattern[k] == '/')
			{
				//
				// TODO: проверять дубликаты
				//

				// std::cout << "tmp = " << tmp << std::endl;

				this->_patterns.insert({tmp,nullptr});
				tmp = tmp + pattern[k];
			}
			else
			{
				tmp = tmp + pattern[k];
			}
		}

		//
		// 
		//

		this->_patterns.insert(std::make_pair(tmp,_type));
	}
	// END for()

	//
	// ADD ACTIONS
	//

	for(auto it = _type->fmap.begin(); it != _type->fmap.end(); ++it)
	{
		this->_actions.insert({it->first,it->second});	
	}
	
	std::cout << _OK_TEXT_ << "ADD TYPE [ " << type_name << " ]" << std::endl;
	return 0;
};


///////////////////////////////////////////////////////////////////////////////////////
/*

	100 - remote domain
	200 - ok
	403 - firbidden
	
	404 - not found
*/
///////////////////////////////////////////////////////////////////////////////////////


std::tuple<int,std::function<void()>> map_t::send_message(
	const std::string &name, 
	const std::string &action, 
	const std::shared_ptr<message_t> &message)
{
	// auto start_sendmessage_time = std::chrono::high_resolution_clock::now();

	std::cout << _YELLOW_ << "send message " << name << " " << action << _BASE_TEXT_<< std::endl;

	///////////////////////////////////////////////////////////////////////////////////////
	//
	// FIND ACTOR
	//
	///////////////////////////////////////////////////////////////////////////////////////

	{
		auto pos = this->_actors.find(name);
		if(pos != this->_actors.end())
		{
			// tegia::actors::action_fn_ptr _fn = this->_actions[pos->second->type + action];
			auto _action = this->_actions[pos->second->type + action];

			return std::move(std::make_tuple(200,
				[
					_actor = pos->second,
					_action,
					message,
					user = tegia::threads::data->user
				]()
				{
					/*
					std::cout << _YELLOW_ << "run action " << _actor->name << " " << _action->action << _BASE_TEXT_<< std::endl;
					std::cout << "   tid           = " << tegia::context::tid() << std::endl;
					std::cout << "   context  user = " << tegia::context::user()->uuid() << std::endl;
					std::cout << "   function user = " << user->uuid() << std::endl;
					std::cout << "   actor ws ws   = " << _actor->ws << std::endl;
					std::cout << "   actor ws name = " << _actor->name << std::endl;
					std::cout << "   user ws       = " << user->_ws << std::endl;
					std::cout << "   user roles    = " << user->_roles.to_ullong() << std::endl;
					std::cout << "   action roles  = " << _action->roles << std::endl;
					std::cout << "   &             = " << (user->_roles.to_ullong() & _action->roles) << std::endl;
					*/

					auto _match = user->_roles.to_ullong() & _action->roles;

					if(_match == 0)
					{
						std::cout << _ERR_TEXT_ << _RED_TEXT_ << "send message \n" 
									<< "      [403] YOU DO NOT HAVE ACCCESS RIGHTS\n" 
									<< "      status = '" << _actor->status << "'\n" 
									<< "      actor  = '" << _actor->name << "'\n"
									<< "      action = '" << _action << "'" << _BASE_TEXT_ << std::endl;

						message->http["response"]["status"] = 403;
						message->http["response"]["type"] = "application/json";
					}
					else if(_match <= 6)
					{
						tegia::threads::data->user = user;
						(_actor->*_action->fn)(message);
					}
					else if(_match > 6 && _actor->ws == user->_ws)
					{
						tegia::threads::data->user = user;
						(_actor->*_action->fn)(message);
					}
					else
					{
						std::cout << _ERR_TEXT_ << _RED_TEXT_ << "send message \n" 
									<< "      [403] YOU DO NOT HAVE ACCCESS RIGHTS\n" 
									<< "      status = '" << _actor->status << "'\n" 
									<< "      actor  = '" << _actor->name << "'\n"
									<< "      action = '" << _action << "'" << _BASE_TEXT_ << std::endl;

						message->http["response"]["status"] = 403;
						message->http["response"]["type"] = "application/json";
					}
				
					auto callback = message->callback.get();
					if(callback.is_addr == true)
					{
						tegia::message::send(callback.actor, callback.action, message);
					}
				}
			));
		}
	
	}

	///////////////////////////////////////////////////////////////////////////////////////
	//
	// CREATE ACTOR
	//
	///////////////////////////////////////////////////////////////////////////////////////


	tegia::actors::domain_t * domain = nullptr;

	size_t index = 0;
	int state = 100;
	std::string actor_name = "";

	size_t domain_index = 0;
	bool found_domain = false;

	size_t action_index = 0;
	bool found_action = false;
	bool is_pattern = false;

	//
	//
	//

	while(index <= name.size())
	{
		// std::cout << "[" << state << "] [" << index << "] [" << name.substr(0,index) << "]" << std::endl;

		switch(state)
		{
			//
			//
			//

			case 100:
			{
				if(name[index] == '/' || index == name.size())
				{
					state = 110;
					break;
				}
				index++;				
			}
			break;

			//
			//
			//
			
			case 110:
			{
				auto pos = this->_domains.find( name.substr(0,index) );
				if(pos == this->_domains.end())
				{
					if(found_domain == true)
					{
						action_index = domain_index;
						state = 190;
					}
					else
					{
						index++;
						state = 100;						
					}
				}
				else
				{
					domain = pos->second;
					found_domain = true;
					domain_index = index;
					index++;
					state = 100;
				}
			}
			break;

			//
			//
			//

			case 190:
			{
				if(domain->_type != tegia::domain::LOCAL)
				{
					std::cout << _ERR_TEXT_ << _RED_TEXT_ << "send message \n" 
								<< "      [100] remoute domain\n" 
								<< "      actor  = '" << name << "'\n" 
								<< "      action = '" << action << "'\n"
								<< "      domain = '" << domain->_name << "'" << _BASE_TEXT_ << std::endl;
					return std::make_tuple(100,nullptr);
				}

				actor_name = name.substr(0, index);
				state = 210;
			}
			break;

			//
			//
			//
			
			case 200:
			{
				if(name[index] == '/' || index == name.size())
				{
					actor_name = name.substr(0,index);
					state = 210;
					break;
				}
				index++;
			}
			break;

			//
			//
			//

			case 210:
			{
				auto pos = this->_patterns.find(actor_name);
				
				if(pos == this->_patterns.end())
				{
					state = 220;
					break;
				}
				else
				{
					is_pattern = false;

					if(pos->second == nullptr)
					{
						state = 200;
						action_index = index;
						index++;
						break;
					}
					
					auto _actor = pos->second->create_actor(name);
					switch(_actor->status)
					{
						case 200:
						{
							// CONTINUE
						}
						break;

						case 403:
						{
							std::cout << _ERR_TEXT_ << _RED_TEXT_ << "send message \n" 
										<< "      [403] YOU DO NOT HAVE ACCCESS RIGHTS\n" 
										<< "      status = '" << _actor->status << "'\n" 
										<< "      actor  = '" << name << "'\n"
										<< "      action = '" << action << "'\n"
										<< "      domain = '" << domain->_name << "'" << _BASE_TEXT_ << std::endl;

							message->http["response"]["status"] = 403;
							message->http["response"]["type"] = "application/json";

							// std::cout << tegia::user::roles(ROLES::GLOBAL::ADMIN,ROLES::GLOBAL::USER) << std::endl;

							delete _actor;
							return std::make_tuple(403,nullptr);
						}
						break;

						default:
						{
							
							std::cout << _ERR_TEXT_ << _RED_TEXT_ << "send message \n" 
										<< "      [500] ACTOR NOT CREATE\n" 
										<< "      status = '" << _actor->status << "'\n" 
										<< "      actor  = '" << name << "'\n"
										<< "      action = '" << action << "'\n"
										<< "      domain = '" << domain->_name << "'" << _BASE_TEXT_ << std::endl;

							message->http["response"]["status"] = 500;
							message->http["response"]["type"] = "application/json";

							delete _actor;
							return std::make_tuple(500,nullptr);
						}
						break;
					} // END switch(_actor->status)

					// std::cout << _YELLOW_ << _actor->type + action << _BASE_TEXT_ << std::endl;
					// std::cout << _YELLOW_ << name << _BASE_TEXT_ << std::endl;

					this->_actors.insert({name,_actor});
					
					tegia::actors::action_t * _action = nullptr;

					{
						auto _pos = this->_actions.find(_actor->type + action);
						if(_pos ==  this->_actions.end())
						{
							std::cout << _ERR_TEXT_ << _RED_TEXT_ << "send message \n" 
										<< "      [500] NOT FOUND ACTION\n" 
										<< "      actor  = '" << name << "'\n" 
										<< "      action = '" << action << "'\n"
										<< "      domain = '" << domain->_name << "'" << _BASE_TEXT_ << std::endl;
							return std::make_tuple(500,nullptr);							
						}

						_action = _pos->second;
					}

					//
					// GENERATE TASK FUNCTION
					//

					return std::move(std::make_tuple(200,
						[
							_actor,
							_action,
							message,
							user = tegia::threads::data->user
						]()
						{
							/*
							std::cout << _YELLOW_ << "run action " << _actor->name << " " << _action->action << _BASE_TEXT_<< std::endl;
							std::cout << "   tid           = " << tegia::context::tid() << std::endl;
							std::cout << "   context  user = " << tegia::context::user()->uuid() << std::endl;
							std::cout << "   function user = " << user->uuid() << std::endl;
							std::cout << "   actor ws ws   = " << _actor->ws << std::endl;
							std::cout << "   actor ws name = " << _actor->name << std::endl;
							std::cout << "   user ws       = " << user->_ws << std::endl;
							std::cout << "   user roles    = " << user->_roles.to_ullong() << std::endl;
							std::cout << "   action roles  = " << _action->roles << std::endl;
							std::cout << "   &             = " << (user->_roles.to_ullong() & _action->roles) << std::endl;
							*/

							auto _match = user->_roles.to_ullong() & _action->roles;

							// std::cout << "min = " << tegia::user::roles(ROLES::PUBLIC, ROLES::USER) << std::endl;

							if(_match == 0)
							{
								std::cout << _ERR_TEXT_ << _RED_TEXT_ << "send message \n" 
											<< "      [403] YOU DO NOT HAVE ACCCESS RIGHTS\n" 
											<< "      status = '" << _actor->status << "'\n" 
											<< "      actor  = '" << _actor->name << "'\n"
											<< "      action = '" << _action << "'" << _BASE_TEXT_ << std::endl;

								message->http["response"]["status"] = 403;
								message->http["response"]["type"] = "application/json";
							}
							else if(_match <= 6)
							{
								tegia::threads::data->user = user;
								(_actor->*_action->fn)(message);
							}
							else if(_match > 6 && _actor->ws == user->_ws)
							{
								tegia::threads::data->user = user;
								(_actor->*_action->fn)(message);
							}
							else
							{
								std::cout << _ERR_TEXT_ << _RED_TEXT_ << "send message \n" 
											<< "      [403] YOU DO NOT HAVE ACCCESS RIGHTS\n" 
											<< "      status = '" << _actor->status << "'\n" 
											<< "      actor  = '" << _actor->name << "'\n"
											<< "      action = '" << _action << "'" << _BASE_TEXT_ << std::endl;

								message->http["response"]["status"] = 403;
								message->http["response"]["type"] = "application/json";
							}
						
							auto callback = message->callback.get();
							if(callback.is_addr == true)
							{
								tegia::message::send(callback.actor, callback.action, message);
							}
						}
					)); 
					// END return
				}
			}
			break;

			//
			//
			//

			case 220:
			{
				// std::cout << "[220]" << std::endl;

				if(is_pattern == false)
				{
					// std::cout << _YELLOW_ << "actor name = '" << name.substr(0, index) << "'" << _BASE_TEXT_ << std::endl;
					// std::cout << _YELLOW_ << "actor name = '" << name.substr(0, action_index) << "'" << _BASE_TEXT_ << std::endl;

					actor_name = name.substr(0, action_index) + "/*";
					state = 210;
					is_pattern = true;
				}
				else
				{
					std::cout << _ERR_TEXT_ << _RED_TEXT_ << "send message \n" 
								<< "      [404] NOT FOUND ACTOR TYPE\n" 
								<< "      actor  = '" << name << "'\n" 
								<< "      action = '" << action << "'\n"
								<< "      domain = '" << domain->_name << "'" << _BASE_TEXT_ << std::endl;
					return std::make_tuple(404,nullptr);
				}
			}
			break;
		}
		// END switch(state)
	}
	// END while(true)

	//
	//
	//

	switch(state)
	{
		case 100:
		{
			std::cout << _ERR_TEXT_ << _RED_TEXT_ << "send message \n" 
						<< "      [404] NOT FOUND ACTOR DOMAIN\n" 
						<< "      actor  = '" << name << "'\n" 
						<< "      action = '" << action << "'" << _BASE_TEXT_ << std::endl;
			return std::make_tuple(404,nullptr);
		}
		break;

		case 200:
		{
			std::cout << _ERR_TEXT_ << _RED_TEXT_ << "send message \n" 
						<< "      [404] NOT FOUND ACTOR TYPE\n" 
						<< "      actor  = '" << name << "'\n" 
						<< "      action = '" << action << "'\n"
						<< "      domain = '" << domain->_name << "'" << _BASE_TEXT_ << std::endl;
			return std::make_tuple(404,nullptr);
		}
		break;

		default:
		{
			std::cout << _ERR_TEXT_ << "404 | NOT FOUND STATE = '" << state << "'" << std::endl;
			return std::make_tuple(404,nullptr);
		}
		break;
	}

};



/*
		std::tuple<int,std::function<int(void)>> send_message(const std::string &actor, const std::string &action, const std::shared_ptr<message_t> &message)
		{
			// std::cout << "actor  = " << actor << std::endl;
			// std::cout << "action = " << action << std::endl;

			{
				auto pos = this->actors.find(actor);
				if(pos != this->actors.end())
				{
					tegia::actors::action_fn_ptr _fn = this->actions[pos->second->type + action];
					return std::move(std::make_tuple(200,[_actor = pos->second,_fn,message]()
					{
						return (_actor->*_fn)(message);
					}));
					
					//(pos->second->*fn)(tegia::message::init());
				}
			}

			{
				auto pos = this->patterns.find(actor);
				if(pos != this->patterns.end())
				{
					auto _actor = pos->second->create_actor(actor);
					tegia::actors::action_fn_ptr _fn = this->actions[_actor->type + action];
					return std::move(std::make_tuple(200,[_actor,_fn,message]()
					{
						return (_actor->*_fn)(message);
					}));

					//(_actor->*fn)(tegia::message::init());
				}
			}

			return std::move(std::make_tuple(500,nullptr));			
		};
*/

}	// END namespace actors
}	// END namespace tegia
