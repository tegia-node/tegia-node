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
	std::function<tegia::actors::actor_base_t * (const std::string &)> create_actor = nullptr;
};

//
//
//

template<typename actor_type>
class type_t
{
	using action_fn_ptr = int (actor_type::*)(const std::shared_ptr<message_t> &);
	// using action_fn_ptr = std::function<int(actor_type * actor, const std::shared_ptr<message_t> &)>;

	friend class actor_t<actor_type>;

	private:
		std::string _name;
		std::map<std::string, action_fn_ptr> action_map;

	public:
		type_t(const std::string &name):_name(name){};
		
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


