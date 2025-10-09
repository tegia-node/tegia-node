#include <string>

#include <tegia/ws/router.h>

namespace tegia {
namespace app {

////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////


router_t::router_t(const std::string &ws_name):ws_name(ws_name){};


////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////

void router_t::action_name(nlohmann::json * _params)
{
	std::string _action_name_old = (*_params)["action"].get<std::string>();
	std::string _action_name_new;
	_action_name_new.reserve(_action_name_old.size() * 2);


	int state = 0;
	size_t start = 0;

	for(size_t i = 0; i < _action_name_old.size(); ++i)
	{
		switch(state)
		{
			case 0:
			{
				if(_action_name_old[i] == '{')
				{
					state = 10;
					start = i+1;
				}
				else
				{
					_action_name_new.append(std::string(1,_action_name_old[i]));
				}
			}
			break;

			case 10:
			{
				if(_action_name_old[i] == '}')
				{
					std::string path = "/params" + _action_name_old.substr(start,i-start);
					// std::cout << "path = '" << path << "'" << std::endl;

					nlohmann::json::json_pointer ptr(path);
					// std::cout << (*_params)[ptr].get<std::string>() << std::endl;

					_action_name_new.append((*_params)[ptr].get<std::string>());

					state = 0;
				}
			}
			break;
		}

		// std::cout << "_actor_name_old = '" << _actor_name_old << "'" << std::endl;
		// std::cout << "_actor_name_new = '" << _actor_name_new << "'" << std::endl;
	}

	(*_params)["action"] = _action_name_new;
};



void router_t::actor_name(nlohmann::json * _params)
{
	std::string _actor_name_old = (*_params)["actor"].get<std::string>();
	std::string _actor_name_new;
	_actor_name_new.reserve(_actor_name_old.size() * 2);

	// std::cout << _params->dump() << std::endl;

	if(_actor_name_old == "{/this}")
	{
		(*_params)["actor"] = this->ws_name;
		return;
	}

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
		std::cout << "it->first  = " << it->first << std::endl;
		std::cout << "it->second = " << it->second << std::endl; 
	}
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::tuple<int, nlohmann::json> router_t::match(
	int role,
	const std::string &method, 
	const std::string &path, 
	nlohmann::json * post)
{
	//
	//
	//

	std::string _path = path + "/" + method;

	std::string prev_pattern = "";
	prev_pattern.reserve(_path.size());

	std::string curr_pattern = "/";
	curr_pattern.reserve(_path.size());

	nlohmann::json params_value;
	nlohmann::json params_name;
	int params_offset = 0;


	int state = 0;
	size_t k = 0;
	bool flag = true;

	while(flag)
	{
		switch(state)
		{
			//
			// READ DATA
			//

			case 0:
			{
				k++;
				// std::cout << "case 0 [" << k << "] '" << _path[k] << "'" << std::endl;

				if(_path[k] == '/' || k == _path.size())
				{
					state = 10;
					break;
				}

				curr_pattern = curr_pattern + _path[k];
			}
			break;

			//
			// FIND PATTERN
			//

			case 10:
			{
				// std::cout << "case 10 '" << _path[k] << "'" << std::endl;
				// std::cout << "prev_pattern = " << prev_pattern << std::endl;
				// std::cout << "curr_pattern = " << curr_pattern << std::endl;

				auto pos = this->_route_map.find(prev_pattern + curr_pattern);
				if(pos == this->_route_map.end())
				{
					state = 404;
					break;
				}

				prev_pattern = prev_pattern + curr_pattern;
				curr_pattern = "/";

				// std::cout << "prev_pattern = " << prev_pattern << std::endl;
				// std::cout << "curr_pattern = " << curr_pattern << std::endl;
				
				state = 0;

				if(pos->second != nullptr)
				{
					params_name = pos->second;
					state = 200;
				}
			}
			break;

			//
			// NOT FOUND PATTERN
			//

			case 404:
			{
				auto pos = this->_route_map.find(prev_pattern + "/*");
				if(pos == this->_route_map.end())
				{
					return std::make_tuple(404,nullptr);
				}

				params_value.push_back(curr_pattern.substr(1));
				prev_pattern = prev_pattern + "/*";
				curr_pattern = "/";			
				state = 0;

				if(pos->second != nullptr)
				{
					params_name = pos->second;
					state = 200;
				}

				// std::cout << "prev_pattern = " << prev_pattern << std::endl;
				// std::cout << "curr_pattern = " << curr_pattern << std::endl;

				// exit(0);
			}
			break;

			//
			// FOUND PATTERN
			//

			case 200:
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

					if(_params.contains(value) == true)
					{
						_params["data"][key] = _params[value];
					}
				}

				//
				// CHECK SECURITY
				//


				/*
				if(_params["role"].get<int>() > role)
				{
					return std::move(std::make_tuple(403,_params));
				}
				*/

				//
				// CHECK ACTOR NAME
				//

				actor_name(&_params);
				action_name(&_params);

				// std::cout << _YELLOW_ << "params: " << _BASE_TEXT_ << std::endl;
				// std::cout << _params << std::endl;

				return std::move(std::make_tuple(200,_params));
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

