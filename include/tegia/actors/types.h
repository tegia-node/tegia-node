#ifndef H_TEGIA_ACTORS_TYPES
#define H_TEGIA_ACTORS_TYPES

#include <iostream>
#include <tegia/core/json.h>
#include <tegia/core/const.h>
#include <tegia/actors/actor_base_t.h>
#include <tegia/actors/message_t.h>


namespace tegia {
namespace actors {



//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// БАЗОВЫЙ КЛАСС ТИПА АКТОРА
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


class type_base
{
	protected:
		std::string name;

	public:
		type_base(const std::string &_name): name(_name){};
		virtual ~type_base() noexcept = default;

		// virtual std::shared_ptr<tegia::actors::actor_base> create_actor(const std::string &name, nlohmann::json data) = 0;
		virtual tegia::actors::actor_base * create_actor(const std::string &name, nlohmann::json data) = 0;

		std::string get_name()
		{
			return this->name;
		};

		virtual std::function<int()> bind_action(
			tegia::actors::actor_base * _actor,
			const std::string &action_name, 
			const std::shared_ptr<message_t> &message
		) = 0;
};



class actor_t
{
	public:

		actor_t() = default;
		~actor_t(){};

		std::string name;
		
		// std::shared_ptr<tegia::actors::actor_base> actor;
		// std::shared_ptr<tegia::actors::type_base> type;

		tegia::actors::actor_base * actor;
		tegia::actors::type_base * type;

		/*
		actor_t& operator=(actor_t& _actor) // примечание: Ссылка не является константной
		{
			if (&_actor == this)
				return *this;
	
			delete this->actor; 
			this->actor = _actor.actor; 
			_actor.actor = nullptr;

			delete this->type; 
			this->type = _actor.type; 
			_actor.type = nullptr;

			this->name = _actor.name;
			
			return *this;
		};
		*/
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


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// КОНТЕЙНЕР ТИПОВ АКТОРОВ
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


class map
{
	private:
		
		//
		// Типы акторов
		//

		std::unordered_map<
			std::string, 					// имя типа акторв
			tegia::actors::type_base *		// 
		> actor_types;
		
		//
		// Паттерны имен акторов
		//

		std::unordered_map<
			std::string, 					// паттерн имени актора
			tegia::actors::type_base *		// 
		> name_patterns;

		//
		// Инициализированные акторы
		//

		std::unordered_map<
			std::string,					// имя актора 
			tegia::actors::actor_t			// объек актора
		> actor_list;

		std::mutex actor_list_mutex;

		//
		// Домены
		//

		std::unordered_map<
			std::string,					// имя домена
			std::string						// тип домена [local, remote]
		> _domains;

		//
		//
		//

		std::tuple<bool,tegia::actors::type_base *> resolve(const std::string &name);
		tegia::actors::actor_base * create_actor(const std::string &name, tegia::actors::type_base * actor_type);

	public:

		map(){};
		~map(){};

		map(map const&) = delete;
		map(map&&) noexcept = delete;
		map& operator = (map const&) = delete;
		map& operator = (map&&) noexcept = delete;


		bool load_type(const std::string &type_name, const std::string &base_path, const nlohmann::json &type_config);
		bool add_domain(const std::string &domain, const std::string &type);
		std::tuple<int,std::function<void()>> send_message(const std::string &name, const std::string &action, const std::shared_ptr<message_t> &message);

};


} // namespace actors
} // namespace tegia

#endif