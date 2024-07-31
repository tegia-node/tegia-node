#ifndef H_TEGIA_ACTOR_WS
#define H_TEGIA_ACTOR_WS

#include <tegia/actors/actor.h>
#include <tegia/actors/type.h>
#include <thread>
#include <chrono>

using namespace std::chrono_literals;

namespace tegia {
namespace actors {

template<typename actor_type>
class ws: public tegia::actors::actor_t<actor_type>
{	
	public:

		ws(
			const std::string &name, 
			tegia::actors::type_t<actor_type> * type): tegia::actors::actor_t<actor_type>(name,type)
		{
			std::cout << "CREATE WS [base]" << std::endl; 
		};

		~ws(){};  

		// ----------------------------------------------------------------------------------   
		// ACTION FUNCTIONS
		// ----------------------------------------------------------------------------------   

		int router(const std::shared_ptr<message_t> &message);

	private:

		// ----------------------------------------------------------------------------------   
		// SUPPORT FUNCTIONS 
		// ----------------------------------------------------------------------------------   

};

///////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
///////////////////////////////////////////////////////////////////////////////////////////////////

//
// Частичная специализация для всех actor_type, которые являются наследниками tegia::actors::ws
//

template<typename actor_type>
class type_t<actor_type, std::enable_if_t<std::is_base_of_v<tegia::actors::ws<actor_type>, actor_type>>> : public type_base_t 
{
    using action_fn_ptr = int (actor_type::*)(const std::shared_ptr<message_t> &);
	friend class actor_t<actor_type>;
	friend class ws<actor_type>;

	private:
		std::string _name;
		std::map<std::string, action_fn_ptr> action_map;

	public:
		type_t(const std::string &name) : _name(name) 
		{
			this->add_action("/router",&actor_type::router);
		}

		tegia::actors::actor_base_t * create_actor(const std::string &name) override 
		{
			std::cout << "CREATE ACTOR <actor_type>" << std::endl;
			return new actor_type(name, this);
		}

		inline std::string name() {
			return this->_name;
		}

		void add_action(const std::string& name, action_fn_ptr _action_fn) {
			this->action_map.insert({name, _action_fn});
		}
};


///////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
///////////////////////////////////////////////////////////////////////////////////////////////////


template<typename actor_type>
int tegia::actors::ws<actor_type>::router(const std::shared_ptr<message_t> &message)
{
	std::cout << "WS::router" << std::endl;

	//
	// CHECK ROLE
	//
	
	std::cout << "tid = " << tegia::context::tid() << std::endl;
	tegia::context::user()->print();

	// message->data["test"] = "test";
	// message->data["action"] = message->http["action"].get<std::string>();

	// std::this_thread::sleep_for(1000ms);
	// tegia::message::send(this->_name,"/router",tegia::message::init());

	return 200;
};


///////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
///////////////////////////////////////////////////////////////////////////////////////////////////


}	// END namespace actors
}	// END namespace tegi



#endif