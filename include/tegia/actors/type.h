#ifndef H_TEGIA_ACTOR_TYPES
#define H_TEGIA_ACTOR_TYPES

#include <iostream>
#include <tegia/core/json.h>
#include <tegia/core/const.h>
#include <tegia/actors/actor.h>
#include <tegia/actors/message_t.h>


////
#undef _LOG_LEVEL_
#define _LOG_LEVEL_ _LOG_WARNING_
#define _LOG_PRINT_ true
#include <tegia/context/log.h>
////


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// БАЗОВЫЙ КЛАСС ТИПА АКТОРА
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


namespace tegia {
namespace actors {

struct action_t
{
	std::string type;
	std::string action;
	tegia::json::validator validator;
	action_fn_ptr fn;
	unsigned long long int roles = 0;
};

struct route_t
{
	std::string method;
	std::string pattern;
	nlohmann::json data;
};


//
//
//


class type_base_t
{
	protected:

	public:
		virtual actor_t * create_actor(const std::string &name) = 0;

		std::string type;
		std::unordered_map<std::string,action_t *> fmap;
		std::unordered_map<std::string,route_t> route_by_key;
		std::unordered_map<std::string,std::string> route_action_index;

		type_base_t(const std::string &type): type(type){};

		int add_action(const std::string &action, const std::string &filename, action_fn_ptr fn, unsigned long long int roles)
		{
			tegia::json::validator validator;
			if(filename != "")
			{
				if(validator.load(filename) == false)
				{
					tegia::log::event_t event;
					event.code = "MnqiwyIrUWyx";
					event._data = {
						{ "type", this->type },
						{ "action", action },
						{ "filename", filename }
					};

					L3ERROR(event);
					exit(0);
				}
			}

			action_t * _action = new action_t{this->type, action, validator, fn, roles};
			this->fmap.insert({this->type + action,_action});
			return 0;			
		};

		bool validate_route_mapping(const nlohmann::json &mapping, std::string &error) const
		{
			if(mapping.is_object() == false)
			{
				error = "route.mapping must be object";
				return false;
			}

			for(auto it = mapping.begin(); it != mapping.end(); ++it)
			{
				if(it.key() != "")
				{
					try
					{
						nlohmann::json::json_pointer ptr(it.key());
					}
					catch(const std::exception &e)
					{
						error = "invalid mapping key json pointer: " + it.key();
						return false;
					}
				}

				if(it.value().is_string() == true)
				{
					std::string value = it.value().get<std::string>();
					if(value.empty() == false && value[0] == '/')
					{
						try
						{
							nlohmann::json::json_pointer ptr(value);
						}
						catch(const std::exception &e)
						{
							error = "invalid mapping value json pointer: " + value;
							return false;
						}
					}
				}
			}

			return true;
		}

		bool validate_route_data(const nlohmann::json &data, std::string &error) const
		{
			if(data.is_object() == false)
			{
				error = "route data must be object";
				return false;
			}

			if(data.contains("actor") == false || data["actor"].is_string() == false)
			{
				error = "route.actor must be string";
				return false;
			}

			if(data.contains("action") == false || data["action"].is_string() == false)
			{
				error = "route.action must be string";
				return false;
			}

			std::string action = data["action"].get<std::string>();
			if(action.empty() == true || action[0] != '/')
			{
				error = "route.action must start with '/'";
				return false;
			}

			if(data.contains("mapping") == false)
			{
				error = "route.mapping is required";
				return false;
			}

			return validate_route_mapping(data["mapping"], error);
		}

		int add_route(const std::string &method, const std::string &pattern, const nlohmann::json &data)
		{
			if(method != "POST")
			{
				tegia::log::event_t event;
				event.code = "WS_ROUTE_METHOD_INVALID";
				event._data = {
					{ "type", this->type },
					{ "method", method },
					{ "pattern", pattern }
				};

				L3ERROR(event);
				exit(0);
			}

			std::string error;
			if(validate_route_data(data, error) == false)
			{
				tegia::log::event_t event;
				event.code = "WS_ROUTE_DATA_INVALID";
				event._data = {
					{ "type", this->type },
					{ "method", method },
					{ "pattern", pattern },
					{ "error", error },
					{ "data", data.dump() }
				};

				L3ERROR(event);
				exit(0);
			}

			std::string key = method + " " + pattern;
			auto pos = this->route_by_key.find(key);
			if(pos != this->route_by_key.end())
			{
				tegia::log::event_t event;
				event.code = "WS_ROUTE_DUPLICATE";
				event._data = {
					{ "type", this->type },
					{ "method", method },
					{ "pattern", pattern }
				};

				L3ERROR(event);
				exit(0);
			}

			std::string action = data["action"].get<std::string>();
			auto action_pos = this->route_action_index.find(action);
			if(action_pos != this->route_action_index.end())
			{
				tegia::log::event_t event;
				event.code = "WS_ROUTE_ACTION_DUPLICATE";
				event._data = {
					{ "type", this->type },
					{ "action", action },
					{ "old_route", action_pos->second },
					{ "new_route", key }
				};

				L3ERROR(event);
				exit(0);
			}

			this->route_by_key.insert({key, route_t{method, pattern, data}});
			this->route_action_index.insert({action, key});
			return 0;
		}

		const std::unordered_map<std::string,route_t> & routes() const
		{
			return this->route_by_key;
		}
};


//
//
//


#define ADD_ACTION(action, func, ...) \
    type->add_action(action, "", static_cast<tegia::actors::action_fn_ptr>(func), tegia::user::roles(__VA_ARGS__))


#define ADD_ACTION2(action, filename, func, ...) \
    type->add_action(action, filename, static_cast<tegia::actors::action_fn_ptr>(func), tegia::user::roles(__VA_ARGS__))

#define ADD_WS_ROUTE(method, pattern, data_json) \
    type->add_route(method, pattern, data_json)

#define ADD_WS_ACTION_ROUTE(method, pattern, route_json, func, ...) \
    do { \
        nlohmann::json __route = route_json; \
        std::string __route_error = ""; \
        if(__route.is_object() == false) { \
            __route_error = "route_json must be object"; \
        } else if(__route.contains("actor") == false || __route["actor"].is_string() == false) { \
            __route_error = "route_json.actor must be string"; \
        } else if(__route.contains("action") == false || __route["action"].is_string() == false) { \
            __route_error = "route_json.action must be string"; \
        } else if(__route["action"].get<std::string>().empty() || __route["action"].get<std::string>()[0] != '/') { \
            __route_error = "route_json.action must start with '/'"; \
        } else if(__route.contains("mapping") == false) { \
            __route_error = "route_json.mapping is required"; \
        } \
        if(__route_error.empty() == false) { \
            std::cout << _ERR_TEXT_ << "[WS_ROUTE_JSON_INVALID] " \
                      << "type='" << type->type \
                      << "' method='" << method \
                      << "' pattern='" << pattern \
                      << "' error='" << __route_error \
                      << "' route_json='" << __route.dump() \
                      << "'" << _BASE_TEXT_ << std::endl; \
            exit(0); \
        } \
        std::string __action = __route["action"].get<std::string>(); \
        type->add_action(__action, "", static_cast<tegia::actors::action_fn_ptr>(func), tegia::user::roles(__VA_ARGS__)); \
        type->add_route(method, pattern, __route); \
    } while(0)


//
//
//


template <typename actor_type, typename Enable = void>
class type_t: public type_base_t
{
	public:
		type_t(const std::string &type): type_base_t(type){};

		actor_t * create_actor(const std::string &name) override
		{
			// std::cout << _YELLOW_ << "create actor " << this->type << " " << name << _BASE_TEXT_<< std::endl;
			return new actor_type(name);
		};
};


} // namespace actors
} // namespace tegia



#endif
