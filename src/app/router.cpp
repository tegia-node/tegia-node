#include <string>

#include <tegia/app/router.h>
#include <tegia/context/context.h>



void actor_name(nlohmann::json * _params)
{
	std::string _actor_name_old = (*_params)["actor"].get<std::string>();
	std::string _actor_name_new;
	_actor_name_new.reserve(_actor_name_old.size() * 2);

	int state = 0;
	size_t start = 0;

	for(size_t i = 0; i < _actor_name_old.size(); ++i)
	{
		switch(state)
		{
			case 0:
			{
				if(_actor_name_old[i] == '{')
				{
					state = 10;
					start = i+1;
				}
				else
				{
					_actor_name_new.append(std::string(1,_actor_name_old[i]));
				}
			}
			break;

			case 10:
			{
				if(_actor_name_old[i] == '}')
				{
					std::string path = "/params" + _actor_name_old.substr(start,i-start);
					// std::cout << "path = '" << path << "'" << std::endl;

					nlohmann::json::json_pointer ptr(path);
					// std::cout << (*_params)[ptr].get<std::string>() << std::endl;

					_actor_name_new.append((*_params)[ptr].get<std::string>());

					state = 0;
				}
			}
			break;
		}

		// std::cout << "_actor_name_old = '" << _actor_name_old << "'" << std::endl;
		// std::cout << "_actor_name_new = '" << _actor_name_new << "'" << std::endl;
	}

	(*_params)["actor"] = _actor_name_new;
};

namespace tegia {
namespace app {


////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////


bool router_t::add(const std::string &method, const std::string &pattern, nlohmann::json data)
{
	std::string _pattern = pattern + "/" + method;
	std::string _path = "";
	std::string _elm = "";

	auto fn = [&_elm,&_path,this,&data](bool final)
	{
		//
		// TODO: проверять дубликаты
		//

		// std::cout << "_elm  = " << _elm << std::endl;

		if(_elm[0] == '{')
		{
			// std::cout << "param = " << _elm.substr(1,_elm.length() - 2) << std::endl;

			data["params"].push_back( _elm.substr(1,_elm.length() - 2) );
			_path = _path + "*";
		}
		else
		{
			_path = _path + _elm;
		}

		if(final == false)
		{
			this->_route_map.insert({_path,nullptr});
		}
		else
		{
			this->_route_map.insert({_path,data});
		}
		
	};

	for(size_t k = 0; k < _pattern.size(); ++k)
	{
		if(_pattern[k] == '/')
		{
			fn(false);
			_path = _path + _pattern[k];
			_elm = "";
		}
		else
		{
			_elm = _elm + _pattern[k];
		}
	}

	fn(true);
	return true;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////


void router_t::print()
{
	for(auto it = this->_route_map.begin(); it != this->_route_map.end(); ++it)
	{
		std::cout << it->first << std::endl;
		std::cout << it->second << std::endl; 
	}
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::tuple<int, nlohmann::json> router_t::match(const std::string &method, const std::string &path, nlohmann::json * post)
{
	std::string _path = path + "/" + method;
	std::string prev_pattern = "";
	std::string curr_pattern = "";
	
	nlohmann::json params_value;
	nlohmann::json params_name;
	int params_offset = 0;

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
				if(_path[k] == '/')
				{
					state = 10;
					break;
				}

				curr_pattern = curr_pattern + _path[k];

				if(k == _path.size() - 1)
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
				prev_pattern = prev_pattern + curr_pattern + _path[k];
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

				// std::cout << _ERR_TEXT_ << "[1] NOT FOUND" << std::endl;
				return std::make_tuple(404,nullptr);
			}
			break;

			//
			//
			//

			case 3:
			{
				nlohmann::json _params = params_name;
				_params.erase("params");

				//
				// MAKE PARAMS
				//

				for(int i = 0; i < params_value.size(); ++i)
				{
					_params["params"][params_name["params"][i].get<std::string>()] = params_value[i].get<std::string>();
				}

				_params["post"] = *post;
				for(auto it = _params["mapping"].begin(); it != _params["mapping"].end(); ++it)
				{
					nlohmann::json::json_pointer key(it.key());
					nlohmann::json::json_pointer value(it.value().get<std::string>());

					_params["data"][key] = _params[value];
				}

				//
				// CHECK SECURITY
				//

				if(_params["security"].get<bool>() == true)
				{
					if(tegia::context::user()->status() != 200)
					{
						return std::move(std::make_tuple(403,_params));
					}
				}

				//
				// CHECK ACTOR NAME
				//

				// std::cout << _params << std::endl;

				actor_name(&_params);
				
				// std::cout << _params << std::endl;
				// exit(0);
								
				return std::move(std::make_tuple(200,_params));
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

				auto pos = this->_route_map.find(prev_pattern + curr_pattern);
				if(pos == this->_route_map.end())
				{
					state = 20;
					break;
				}

				if(pos->second == nullptr)
				{
					state = 1;
					break;
				}

				if(k < path.size() - 1)
				{
					state = 2;
					break;
				}

				params_name = pos->second;
				state = 3;
				
				// return std::move(std::make_tuple(true,prev_pattern + curr_pattern,pos->second));				
			}
			break;

			//
			//
			//

			case 20:
			{
				// std::cout << "prev_pattern = " << prev_pattern << std::endl;
				// std::cout << "curr_pattern = " << curr_pattern << std::endl;

				auto pos = this->_route_map.find(prev_pattern + "*");
				if(pos == this->_route_map.end())
				{
					// NOT FOUND
					// std::cout << _ERR_TEXT_ << "[2] NOT FOUND" << std::endl;
					return std::make_tuple(404,nullptr);
				}

				if(pos->second == nullptr)
				{
					params_value.push_back(curr_pattern);
					curr_pattern = "*";
					state = 1;
					break;
				}

				if(k < path.size() - 1)
				{
					state = 2;
					break;
				}

				params_value.push_back(curr_pattern);
				params_name = pos->second;
				curr_pattern = "*";
				state = 3;

				// return std::move(std::make_tuple(true,prev_pattern + "*",pos->second));
			}
			break;
		}
		//	END switch(state)
		
	}

	// std::cout << _ERR_TEXT_ << "[3] NOT FOUND" << std::endl;
	return std::make_tuple(404,nullptr);
};


}	// END namespace app
}	// END namespace tegia

