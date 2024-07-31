#ifndef H_TEGIA_ACTOR_TYPES
#define H_TEGIA_ACTOR_TYPES

#include <iostream>
#include <tegia/core/json.h>
#include <tegia/core/const.h>
#include <tegia/actors/message_t.h>


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// БАЗОВЫЙ КЛАСС ТИПА АКТОРА
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


namespace tegia {
namespace actors {


template<typename actor_type>
class actor_t;

class actor_base_t;

//
//
//

struct type_base_t
{
	virtual tegia::actors::actor_base_t * create_actor(const std::string &name) = 0;
};

//
//
//

template<typename actor_type, typename Enable = void>
class type_t: public type_base_t
{
	using action_fn_ptr = int (actor_type::*)(const std::shared_ptr<message_t> &);
	friend class actor_t<actor_type>;

	private:
		std::string _name;
		std::map<std::string, action_fn_ptr> action_map;

	public:
		type_t(const std::string &name):_name(name)
		{
			std::cout << "Common constructor" << std::endl;
		};
		
		tegia::actors::actor_base_t * create_actor(const std::string &name) override
		{
			
			return new actor_type(name,this);
		};

		inline std::string name()
		{
			return this->_name;
		};

		void add_action(const std::string& name, action_fn_ptr _action_fn)
		{
			this->action_map.insert({name, _action_fn});
		};
};

//
//
//

} // namespace actors
} // namespace tegia


#endif


