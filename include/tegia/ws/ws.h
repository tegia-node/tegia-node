#ifndef H_TEGIA_ACTOR_WS
#define H_TEGIA_ACTOR_WS

#include <thread>
#include <chrono>

#include <tegia/actors/actor.h>
#include <tegia/actors/type.h>
#include <tegia/db/mysql/mysql.h>
#include <tegia/ws/router.h>

//
//
//

using namespace std::chrono_literals;

namespace tegia {
namespace actors {


class ws_t: public actor_t
{
	template <typename actor_type, typename Enable> friend class type_t;
	
	protected:
		nlohmann::json data;
		tegia::app::router_t * _router;

		ws_t(
			const std::string &type, 
			const std::string &name,
			const std::string &connection,
			const std::string &table,
			unsigned long long int creators);

		~ws_t();	

		// ----------------------------------------------------------------------------------   
		// ACTION FUNCTIONS
		// ----------------------------------------------------------------------------------   

		int init(const std::shared_ptr<message_t> &message);
		int router(const std::shared_ptr<message_t> &message);
		int member_add(const std::shared_ptr<message_t> &message);
		int member_remove(const std::shared_ptr<message_t> &message);

	protected:
		int commit();

	private:
		std::string table;
		std::string connection;
		std::unordered_map<std::string, unsigned long long int> members;
		unsigned long long int creators;

		int _create();
};

//
//
//

template<typename actor_type>
class type_t<actor_type, std::enable_if_t<std::is_base_of_v<tegia::actors::ws_t, actor_type>>> : public type_base_t 
{
	protected: 

	public:
		type_t(const std::string &type): type_base_t(type)
		{
			this->add_action(
				"/init",
				"",
				static_cast<tegia::actors::action_fn_ptr>(&tegia::actors::ws_t::init),
				tegia::user::roles(ROLES::WS::OWNER)
			);

			this->add_action(
				"/router",
				"",
				static_cast<tegia::actors::action_fn_ptr>(&tegia::actors::ws_t::router),
				tegia::user::roles(ROLES::SESSION::PUBLIC, ROLES::SESSION::USER)
			);

			this->add_action(
				"/member/add",
				"",
				static_cast<tegia::actors::action_fn_ptr>(&tegia::actors::ws_t::member_add),
				tegia::user::roles(ROLES::WS::OWNER, ROLES::WS::ADMIN)
			);

			this->add_action(
				"/member/remove",
				"",
				static_cast<tegia::actors::action_fn_ptr>(&tegia::actors::ws_t::member_remove),
				tegia::user::roles(ROLES::WS::OWNER, ROLES::WS::ADMIN)
			);
		};

		actor_t * create_actor(const std::string &name) override
		{
			std::cout << _YELLOW_ << "[create ws] " << this->type << " " << name << _BASE_TEXT_<< std::endl;
			return new actor_type(name);
		};
};



}	// END namespace actors
}	// END namespace tegi



#endif