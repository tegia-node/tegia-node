#ifndef H_TEGIA_ACTORS_MAP
#define H_TEGIA_ACTORS_MAP

#include <iostream>
#include <tegia/core/json.h>
#include <tegia/core/const.h>
#include <tegia/actors/types.h>


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// КОНТЕЙНЕР ДЛЯ ИНСТАНЦИРОВАННОГО АКТОРА
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


namespace tegia {
namespace actors {

class actor_t
{
	public:

		actor_t() = default;
		~actor_t(){};

		std::string name;

		// Указатель на экземпляр класса актора
		tegia::actors::actor_base * actor;

		// Указатель на экземпляр типа автора
		tegia::actors::type_base * type;
};

} // namespace actors
} // namespace tegia


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// МАССИВ ВСЕХ ИНСТАНЦИРОВАННЫХ АКТОРОВ
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


namespace tegia {
namespace actors {


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