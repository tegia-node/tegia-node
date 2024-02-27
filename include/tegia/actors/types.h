#ifndef H_TEGIA_ACTORS_TYPES
#define H_TEGIA_ACTORS_TYPES

#include <iostream>
#include <tegia/core/json.h>
#include <tegia/core/const.h>
#include <tegia/actors/message_t.h>


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// БАЗОВЫЙ КЛАСС АКТОРА
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


#define _action_func_bind_(_function_) std::bind(_function_, std::placeholders::_1, std::placeholders::_2)
#define ADD_ACTION(_name_, _function_) actor_type->add_action( _name_, _action_func_bind_(_function_) )

namespace tegia {
namespace actors {


class actor_base
{
	protected:
		std::string type;
		std::string name; 

	public:

		actor_base(
			const std::string &_type,
			const std::string &_name,
			nlohmann::json init_data): type(_type),name(_name)
		{	
			std::cout << _OK_TEXT_ << "CREATE ACTOR" << std::endl;
			std::cout << "      type   = " << _type << std::endl;
			std::cout << "      name   = " << _name << std::endl;
		};

		virtual ~actor_base(){ };

		actor_base(actor_base const&) = delete;
		actor_base(actor_base&&) noexcept = delete;
		actor_base& operator = (actor_base const&) = delete;
		actor_base& operator = (actor_base&&) noexcept = delete;
};


} // namespace actors
} // namespace tegia


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// БАЗОВЫЙ КЛАСС ТИПА АКТОРА
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


namespace tegia {
namespace actors {

class type_base
{
	protected:
		std::string name;

	public:
		type_base(const std::string &_name): name(_name){};
		virtual ~type_base() noexcept = default;

		virtual tegia::actors::actor_base * create_actor(const std::string &name, nlohmann::json data) = 0;

		inline std::string get_name()
		{
			return this->name;
		};

		virtual std::function<int()> bind_action(
			tegia::actors::actor_base * _actor,
			const std::string &action_name, 
			const std::shared_ptr<message_t> &message
		) = 0;
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// ШАБЛОННЫЙ КЛАСС ТИПА АКТОРА
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


#define   action_fn    std::function<int(T *,const std::shared_ptr<message_t> &)>

template<class T>
using actions_map = std::unordered_map<std::string, action_fn>;


template<class T>
class type: public type_base
{
	private:
		actions_map<T> map{};
		std::string name;

	public:
		type(const std::string &_name): type_base(_name)
		{

		};

		~type() noexcept override 
		{
			map.clear();
		};

		bool add_action(const std::string &action_name, action_fn fn)
		{
			this->map.insert({action_name,fn});
			return true;
		};

		tegia::actors::actor_base * create_actor(const std::string &name, nlohmann::json data)
		{
			return new T(name, data);
		};


		std::function<int()> bind_action(
			tegia::actors::actor_base * _actor,
			const std::string &action_name, 
			const std::shared_ptr<message_t> &message)
		{
			auto pos = this->map.find(action_name);
			if(pos == this->map.end())
			{
				return nullptr;
			}

			return std::bind(pos->second, static_cast<T*>(_actor), message);
		};
};


} // namespace actors
} // namespace tegia

#endif