#ifndef H_TEGIA_PLATFORM_ACTORS_ROUTERS_MAP
#define H_TEGIA_PLATFORM_ACTORS_ROUTERS_MAP

#include <tegia/node/logger.h>
#include <tegia/actors/add_task.h>
#include <tegia/tegia.h>
#include <tegia/sockets/Helper.h>

#define action_fn std::function<std::string(T *,const std::shared_ptr<message_t> &, const nlohmann::json &)>
#define _DEBUG_MODE_ true

namespace tegia {
namespace actors {

	template<class T>
	struct router_info
	{
		bool final = false;
		action_fn _fn;
		std::vector<std::string> params;
		std::vector<std::string> params_value;
		std::string pattern;
	};



struct router_base
{
	std::string actor_type;

	router_base() = default;
	virtual ~router_base() noexcept = default;

	virtual std::string get_actor_type()
	{
		return actor_type;
	};

	virtual tegia::actors::actor_base* create_actor(const std::string &name, nlohmann::json &data) = 0;

	virtual nlohmann::json get_action_names() const noexcept = 0;

	virtual std::string find_pattern(const std::string &action_name) = 0;

	virtual int init_job(
		tegia::actors::actor_base * _actor,
		const std::string &action_name, 
		const std::shared_ptr<message_t> &message, 
		int priority) = 0;

	virtual std::string call(
		tegia::actors::actor_base * _actor,
		const std::string &action_name, 
		const std::shared_ptr<message_t> &message) = 0;		
};

template<class T>
using router_actions_map = std::unordered_map<std::string, router_info<T>, 
						   tegia::common::case_insensitive_unordered_map::hash, tegia::common::case_insensitive_unordered_map::equal_to>;

template<class T>
class router_common: public router_base
{
	protected: 
		// std::map <std::string, action_fn > fn_map;
		router_actions_map<T> route{};

		std::vector<std::string> action_names_{};


		/////////////////////////////////////////////////////////////////////////////////////////////////////
		/*

		*/
		/////////////////////////////////////////////////////////////////////////////////////////////////////


		router_info<T> match(router_actions_map<T> const& route, std::string path)
		{
			router_info<T> _route{};

			auto find = route.find(path);
			if(find != route.end())
			{
				return find->second;
			}

			path = path + "/";

			std::string::size_type pos = 0;
			std::string::size_type find_pos;

			std::string pattern = "";
			std::string _pattern = "";

			find_pos = path.find("/", pos);
			while(find_pos != std::string::npos)
			{
				if(pos != find_pos)
				{
					std::string param = path.substr(pos, find_pos - pos);
					_pattern = pattern + "/" + param;
					
					pos = find_pos + 1;
					find_pos = path.find("/", pos);


					find = route.find(_pattern);
					if(find != route.end())
					{
						pattern = _pattern;
						continue;
					}

					_pattern = pattern + "/{}";

					find = route.find(_pattern);
					if(find != route.end())
					{
						_route.params_value.push_back(param);
						pattern = _pattern;
						continue;
					}

					return _route;
				}
				else
				{
					pos = find_pos + 1;
					find_pos = path.find("/", pos);
				}
			}
			
			_route.final = find->second.final;
			_route._fn = find->second._fn;
			_route.params = find->second.params;
			_route.pattern = find->second.pattern;

			return _route;
		};

	public:
		router_common() = default;
		~router_common() noexcept override 
		{
			route.clear();
			action_names_.clear();
		}


		nlohmann::json get_action_names() const noexcept override
		{
			nlohmann::json actions = nlohmann::json::array();
			for (auto const& action : action_names_)
			{
				actions.emplace_back(action);
			}
			return actions;
		}


		/////////////////////////////////////////////////////////////////////////////////////////////////////
		/*

		*/
		/////////////////////////////////////////////////////////////////////////////////////////////////////


		std::string find_pattern(const std::string &action_name)
		{
			if (auto it = match(this->route, action_name); it.final) return it.pattern;

			std::cout << _ERR_TEXT_ << "[find_pattern] " << action_name << " not found" << std::endl;
			LERROR("[find_pattern] " << action_name << " not found")
			return "";
		};


		/////////////////////////////////////////////////////////////////////////////////////////////////////
		/*

		*/
		/////////////////////////////////////////////////////////////////////////////////////////////////////


		bool add(const std::string &action_name, action_fn fn)
		{
			auto v = core::explode(action_name,"/",false);

			if(v.size() < 1)
			{
				return false;
			}

			action_names_.emplace_back(action_name);

			router_info<T> _route {false,nullptr};

			std::string pattern;
			size_t it = 0;

			for(it = 0; it < v.size()-1; it++)
			{
				if(v[it][0] == '{')
				{
					pattern = pattern + "/{}";
					_route.params.push_back( v[it].substr(1,(v[it].size()-2)) );
				}
				else
				{
					pattern = pattern + "/" + v[it];
				}

				if(this->route.find(pattern) == this->route.end())
				{
					this->route.insert({pattern,{false,nullptr}});
				}
			}
			
			if(v[it][0] == '{')
			{
				pattern = pattern + "/{}";
				_route.params.push_back( v[it].substr(1,(v[it].size()-2)) );
			}
			else
			{
				pattern = pattern + "/" + v[it];
			}

			_route.final = true;
			_route._fn = fn;
			_route.pattern = pattern;

			this->route.insert({pattern,_route});
			return true;
		};

};


template<class T>
class router: public router_common<T>
{
	public:
		router(const std::string &actor_type){};
		~router() noexcept override = default;


		/////////////////////////////////////////////////////////////////////////////////////////////////////
		/*

		*/
		/////////////////////////////////////////////////////////////////////////////////////////////////////


		tegia::actors::actor_base* create_actor(const std::string &name, nlohmann::json &data)
		{
			return new T(name, data);
		};

		/////////////////////////////////////////////////////////////////////////////////////////////////////
		/*

		*/
		/////////////////////////////////////////////////////////////////////////////////////////////////////


		std::string call(tegia::actors::actor_base * _actor,const std::string &action_name, const std::shared_ptr<message_t> &message)
		{
			auto actor = static_cast<T*>(_actor);
			auto it = this->match(this->route, action_name);
			if(it.final == false)
			{
				std::cout << _ERR_TEXT_ << "[2] callActor " << actor->get_name() << action_name << " not found action" << std::endl;
				LERROR("callActor " << actor->get_name() << action_name << " not found action")
				return "#callerror";				
			}

			nlohmann::json jparams;
			for(size_t i = 0; i < it.params.size(); i++)
			{
				jparams[it.params[i]] = it.params_value[i];
			}

			return it._fn(actor,message,jparams);
		};



		/////////////////////////////////////////////////////////////////////////////////////////////////////
		/*

		*/
		/////////////////////////////////////////////////////////////////////////////////////////////////////



		int init_job(tegia::actors::actor_base * _actor,const std::string &action_name, const std::shared_ptr<message_t> &message, int priority)
		{
			auto actor = static_cast<T*>(_actor);

			//
			// NEW
			//

			auto it = this->match(this->route, action_name);

			if(it.final == false)
			{
				std::cout << _ERR_TEXT_ << "[2] sendMessage " << actor->get_name() << action_name << " not found action" << std::endl;
				LERROR("sendMessage " << actor->get_name() << action_name << " not found action")
				return 2;
			}
		
			//
			// GENERATE TASK JOB
			//

			nlohmann::json jparams;
			for(size_t i = 0; i < it.params.size(); i++)
			{
				jparams[it.params[i]] = it.params_value[i];
			}

			actor->countMessage++;
			// std::cout << "countMessage = " << actor->countMessage.load() << std::endl; 

			// ---------------------------------------------------------------------------------------------------

			auto fn = std::bind([] (
				action_fn fn,
				T * actor,
				const std::string &action_name, 
				const std::shared_ptr<message_t> &message, 
				nlohmann::json route_params) 
			{
				auto start_time = std::chrono::high_resolution_clock::now();
				
				LDEBUG("run actor " << actor->get_name() << action_name)
				#if _DEBUG_MODE_ == true
				std::cout << _OK_TEXT_ << "run actor " << actor->get_name() << action_name << std::endl; 
				#endif	
				
				std::string result = "";

				try
				{
					result = fn(actor,message,route_params);	
				}
				
				catch(const std::exception& e)
				{
					std::cout << _ERR_TEXT_ << "[" << actor->get_name() << action_name << "] " << e.what() << std::endl;
					exit(0);
				}

				// std::cout << message->pipeline.dump() << std::endl;

				if(message->pipeline.is_null() == true)
				{
					auto callback = message->get_callback();
					if(callback.actor != "")
					{
						tegia::message::send(callback.actor, callback.action, message);
					}
				}
				else
				{
					std::string _a = actor->get_name() + action_name;
					// std::cout << _a << " " << result << std::endl;

					if(message->pipeline.contains(_a) == true)
					{
						if(message->pipeline[_a].contains(result) == true)
						{
							if(message->pipeline[_a][result].is_object() == true)
							{
								tegia::message::send(
									message->pipeline[_a][result]["actor"].get<std::string>(), 
									message->pipeline[_a][result]["action"].get<std::string>(), 
									message);
							}
							else
							{
								if(message->pipeline[_a][result].is_string() == true)
								{
									message->pipeline = nlohmann::json{};
									auto callback = message->get_callback();
									if(callback.actor != "")
									{
										tegia::message::send(callback.actor, callback.action, message);
									}										
								}
							}
						}
						else
						{
							std::cout << _ERR_TEXT_ << "message->pipeline not contains [" << actor->get_name() << action_name << "] [" << result << "]" << std::endl;
						}
					}
					else
					{
						std::cout << _ERR_TEXT_ << "message->pipeline not contains [" << actor->get_name() << action_name << "]" << std::endl;
					}
				}
				
				actor->countMessage--;
				// std::cout << "countMessage = " << actor->countMessage.load() << std::endl;

				auto end_time = std::chrono::high_resolution_clock::now();
			},it._fn,actor,action_name,message,jparams);

			// ---------------------------------------------------------------------------------------------------

			// 
			tegia::threads::add_task(fn,priority);
			//

			LDEBUG("sendMessage " << actor->get_name() << action_name)
			#if _DEBUG_MODE_ == true
			std::cout << _OK_TEXT_ << "sendMessage " << actor->get_name() << action_name << std::endl;
			#endif

			return 0;
		};
};


class remote_router: public router_common<tegia::actors::router_base>
{
	public:
		remote_router(std::string _actor_type){ actor_type = std::move(_actor_type); };
		~remote_router() noexcept override = default;


		tegia::actors::actor_base* create_actor(const std::string &name, nlohmann::json &data)
		{
			return nullptr;
		};


		std::string call(tegia::actors::actor_base * _actor,const std::string &action_name, const std::shared_ptr<message_t> &message)
		{
			return "not available"s;
		};


		int init_job(tegia::actors::actor_base * _actor,const std::string &action_name, const std::shared_ptr<message_t> &message, int priority)
		{
			return -1;
		};
};


} // namespace actors
} // namespace tegia


#define _DEBUG_MODE_ false

#endif