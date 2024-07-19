#include <dlfcn.h>
#include <utility>

#include <tegia/core/const.h>

#include "map.h"

namespace tegia {
namespace actors {



//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


map_t::map_t()
{

};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


map_t::~map_t()
{
	
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


void map_t::add_domain(const std::string &name, int type)
{
	auto domain = new tegia::actors::domain_t();
	domain->_name = name;
	domain->_type = type;

	auto [pos,code] = this->_domains.insert({name,domain});
	if(code == false)
	{
		delete domain;
	}
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void map_t::add_type(
	const std::string &type_name, 
	const std::string &base_path, 
	nlohmann::json * data)
{
	//
	// CHECK TYPE
	//

	{
		auto pos = this->_types.find(type_name);
		if(pos != this->_types.end()) return;
	}

	//
	// CHECK DATA
	//

	if(data->contains("patterns") == false || (*data)["patterns"].is_array() == false)
	{
		return;
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
		return;
	}


	// typedef tegia::actors::type_base_t * (*init_type_t)(const std::string&);


    // Получение указателя на функцию
    // init_type_t _init_type = (init_type_t) dlsym(lib, "_init_type");
	auto _fn = ( tegia::actors::type_base_t * (*)(void) )dlsym(lib,"_init_type");
    const char* dlsym_error = dlerror();
    if (dlsym_error) {
        std::cerr << "Cannot load symbol '_init_type': " << dlsym_error << '\n';
        dlclose(lib);
        return;
    }

	// 

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

		this->_patterns.insert(std::make_pair(tmp,_fn()));
	}
	// END for()

	std::cout << _OK_TEXT_ << "ADD TYPE [ " << type_name << " ]" << std::endl;
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

	///////////////////////////////////////////////////////////////////////////////////////
	//
	// FIND ACTOR
	//
	///////////////////////////////////////////////////////////////////////////////////////

	{
		auto pos = this->actors.find(name);
		if(pos != this->actors.end())
		{
			return pos->second->get_action(action, message);
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
					// return 100;
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
				// std::cout << _YELLOW_ << "actor name = '" << actor_name << "'" << _BASE_TEXT_ << std::endl;

				auto pos = this->_patterns.find(actor_name);
				
				if(pos == this->_patterns.end())
				{
					// std::cout << "not found" << std::endl;
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
					
					auto actor = pos->second->create_actor(name);
					this->actors.insert({name,actor});
					// auto fn = actor->get_action(action, message);
					// return std::make_tuple(200,fn);
					return actor->get_action(action, message);

					// std::cout << _RED_TEXT_ << "END [" << name << "][" << action << "]" << _BASE_TEXT_ << std::endl; 

					//
					//
					//

					// return 200;
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
					// return 404;
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
			// return 404;
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
			// return 404;
			return std::make_tuple(404,nullptr);
		}
		break;

		default:
		{
			std::cout << _ERR_TEXT_ << "404 | NOT FOUND STATE = '" << state << "'" << std::endl;
			// return 404;
			return std::make_tuple(404,nullptr);
		}
		break;
	}

};





//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*


*/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////



}	// END namespace actors
}	// END namespace tegia