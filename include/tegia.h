#ifndef _H_TEGIA_PLATFORM_
#define _H_TEGIA_PLATFORM_

////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// INCLUDES                                                                               //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

#include <tegia/const.h>
#include <tegia/core/core.h>
#include <tegia/core/cast.h>
#include <tegia/core/string.h>

#include <tegia/node/logger.h>
#include <tegia/node/config.h>
#include <tegia/actors/message_t.h>

#include <tegia/context/thread_context.h>


#include <tegia/db/mysql/mysql.h>
#include <tegia/db/sphinx/sphinx.h>


////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// ACTORS FUNCTIONS                                                                       //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////


namespace tegia {
namespace actors {

	class actor_base;

	tegia::actors::actor_base * get(const std::string &actor_name);
	tegia::actors::actor_base * _new(const std::string &actor_type,const std::string &actor_name, nlohmann::json &data, bool const share = false);
	bool _delete(const std::string &actor_name);

	std::string find_pattern(const std::string &actor_name, const std::string &action_name);

} // end namespace actors
} // end namespace tegia


////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// MESSAGE FUNCTIONS                                                                      //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////


namespace tegia {
namespace message {
	int send(
		const std::string &actor_name, 
		const std::string &action_name, 
		const std::shared_ptr<message_t> &message,
		int priority = _PLOW_);

	int send(
		const std::string &actor_name, 
		const std::string &action_name, 
		nlohmann::json data = nlohmann::json(),
		int priority = _PLOW_);


	const std::shared_ptr<message_t> init(nlohmann::json data = nlohmann::json());

	template<class T>
	const std::shared_ptr<T> init(nlohmann::json data = nlohmann::json())
	{
		std::shared_ptr<T> message(new T( data ));
		return message;
	};

} // end namespace message
} // end namespace tegia


////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// SOCKETS FUNCTIONS                                                                      //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

typedef uint16_t in_port_t;

namespace tegia::sockets 
{

	void send_config_message_to_server(std::string_view message_body) noexcept;

	void send_message_to_server(std::string actor_name, std::string action_name, std::shared_ptr<message_t> const message, int const priority) noexcept;
	void send_message_to_client
		 (std::string actor_name, std::string action_name, std::string node_uuid, std::shared_ptr<message_t> const message, int const priority) noexcept;

	bool is_server_node() noexcept;
	bool is_client_node() noexcept;
	std::string node_uuid() noexcept;

}	

////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// THREADS FUNCTIONS                                                                      //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////


namespace tegia {
namespace threads {

tegia::threads::data * const current();
std::string jwt();
std::string user_uuid();
int count();

} // end namespace threads
} // end namespace tegia


namespace tegia {
namespace context {

bool is_role(const int _role);

} // end namespace context
} // end namespace tegia




////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// CONFIG FUNCTIONS	                                                                      //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////


namespace tegia {
namespace conf {

std::string path(const std::string &name);

tegia::config * load(
	const std::string &config_name, 
	const std::string &filename);

tegia::config * get(const std::string & config_name);

}	// end namespace conf
}	// end namespace tegia



namespace tegia {

tegia::config * load_config(
	const std::string &config_name, 
	const std::string &filename);

// tegia::config * get_config(const std::string & config_name);

}	// end namespace tegia


#endif