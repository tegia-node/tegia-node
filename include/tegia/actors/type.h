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
};


//
//
//


#define ADD_ACTION(action, func, ...) \
    type->add_action(action, "", static_cast<tegia::actors::action_fn_ptr>(func), tegia::user::roles(__VA_ARGS__))


#define ADD_ACTION2(action, filename, func, ...) \
    type->add_action(action, filename, static_cast<tegia::actors::action_fn_ptr>(func), tegia::user::roles(__VA_ARGS__))


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


