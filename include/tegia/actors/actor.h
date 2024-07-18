#ifndef H_TEGIA_ACTOR_BASE
#define H_TEGIA_ACTOR_BASE

#include <iostream>
#include <tegia/core.h>


#define RETURN_TYPE(TYPE_T,ACTOR_TYPE)                             \
    auto type_base = new tegia::actors::type_base_t();             \
    type_base->create_actor = [TYPE_T](const std::string &name)    \
    {                                                              \
        return new ACTOR_TYPE(name, TYPE_T);                       \
    };                                                             \
    return type_base;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// БАЗОВЫЙ КЛАСС АКТОРА
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


namespace tegia {
namespace actors {


//
// Шаблонный базовый класс
//


class actor_base_t
{
	public:
		virtual std::tuple<int,std::function<void()>> get_action(const std::string &action, const std::shared_ptr<message_t> &message) = 0;
};


//
//
//


template<typename actor_type>
class actor_t: public actor_base_t
{
	protected:
		tegia::actors::type_t<actor_type> * _type = nullptr;
		std::string _name;

	public:
		actor_t(const std::string &name, tegia::actors::type_t<actor_type> * type): _name(name), _type(type)
		{
			// std::cout << "actor_t::constructor" << std::endl;
		};

		~actor_t()
		{
			delete this->_type;
		};

		actor_t(actor_t<actor_type> const&) = delete;
		actor_t(actor_t<actor_type> &&) noexcept = delete;
		actor_t& operator = (actor_t<actor_type> const&) = delete;
		actor_t& operator = (actor_t<actor_type> &&) noexcept = delete;
		
		std::string name()
		{
			return this->_name;
		};

		std::tuple<int,std::function<void()>> get_action(const std::string &action, const std::shared_ptr<message_t> &message) override
		{
			auto pos = this->_type->action_map.find(action);
			if(pos == this->_type->action_map.end())
			{
				return std::move(std::make_tuple(404,nullptr));
			}

			return std::move(std::make_tuple(200,[this,action,_action_fn = pos->second,message]()
			{
				// auto start_time = std::chrono::high_resolution_clock::now();

				try
				{
					int result = (static_cast<actor_type *>(this)->*_action_fn)(message);
					
					auto callback = message->callback.get();
					if(callback.is_addr == true)
					{
						tegia::message::send(callback.actor, callback.action, message);
					}
				}

				catch(const std::exception& e)
				{
					std::cout << _ERR_TEXT_ << "[" << this->name() << action << "] " << e.what() << std::endl;
					exit(0);
				}

				// auto end_time = std::chrono::high_resolution_clock::now();

			}));
		};
};


} // namespace actors
} // namespace tegia

#endif

