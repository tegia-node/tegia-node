#undef  _DEBUG_PRINT_MODE_
#define _DEBUG_PRINT_MODE_ false
#include <tegia/node/logger_print.h>

////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// INCLUDES                                                                               //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

#include <tegia/tegia.h>
#include "node/node.h"
#include "threads/thread_pool.h"


namespace tegia {
namespace actors {

	tegia::actors::actor_base * get(const std::string &actor_name)
	{
		return tegia::node::instance()->get_actor(actor_name);
	};

	tegia::actors::actor_base * _new(const std::string &actor_type,const std::string &actor_name, nlohmann::json &data, bool const share)
	{
		return tegia::node::instance()->create_actor(actor_type, actor_name, share, data);
	};

	bool _delete(const std::string &actor_name)
	{
		return tegia::node::instance()->delete_actor(actor_name);
	};

	std::string find_pattern(const std::string &actor_name, const std::string &action_name)
	{
		// std::cout << "tegia::actors::find_pattern()" << std::endl;
		return tegia::node::instance()->find_pattern(actor_name,action_name);
	};

}	// end namespace actors
}	// end namespace tegia



namespace tegia {
namespace threads {

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
	tegia::threads::data * const current()
	{
		return tegia::threads::pool::instance()->get_current();
	};
#pragma GCC diagnostic pop

	std::string jwt()
	{
		return tegia::threads::pool::instance()->get_current()->get_jwt();
	};

	std::string user_uuid()
	{
		return tegia::threads::pool::instance()->get_current()->get_user_uuid();
	};

	int count()
	{
		return tegia::threads::pool::instance()->get_count();
	};

}	// end namespace thread
}	// end namespace tegia



namespace tegia {
namespace context {

bool is_role(const int _role)
{
	return tegia::threads::pool::instance()->get_current()->is_role(_role);
};

} // end namespace context
} // end namespace tegia




namespace tegia::sockets 
{
	///
	/// Sends configuration message to the remote server tegia-node via TCP/IP socket (non-cashed!).
	///
	void send_config_message_to_server(std::string_view const message_body) noexcept
	{
		auto const [error_code, error_string] = tegia::node::instance()->send_message_to_server_socket(message_body);
		if (error_code != SUCCESS && error_string.has_value())
		{
			LWARNING(*error_string)
			EOUT(*error_string)
		}
	};

	///
	/// Sends action message to the remote client tegia-node via TCP/IP socket (cacheable).
	///
	void send_message_to_client
		(std::string actor_name, std::string action_name, std::string node_uuid, std::shared_ptr<message_t> const message, int const priority) noexcept
	{
       	auto output = nlohmann::json {{tegia::ACTION_MESSAGE_DATA, {{tegia::ACTOR_NAME, actor_name}, {tegia::ACTION_NAME, action_name}, 
									  {tegia::ACTION_MESSAGE, message->serialize()}, {tegia::ACTION_PRIORITY, priority}}}};

		auto const [error_code, error_string] = tegia::node::instance()->send_message_to_client(node_uuid, std::move(output));
		if (error_code != SUCCESS && error_string.has_value())
		{
			LERROR(*error_string)
			EOUT(*error_string)
		}
	}

	///
	/// Sends action message to the remote server tegia-node via TCP/IP socket (cacheable).
	///
	void send_message_to_server (std::string actor_name, std::string action_name, std::shared_ptr<message_t> const message, int const priority) noexcept
	{
       	auto output = nlohmann::json {{tegia::ACTION_MESSAGE_DATA, {{tegia::ACTOR_NAME, actor_name}, {tegia::ACTION_NAME, action_name}, 
									  {tegia::ACTION_MESSAGE, message->serialize()}, {tegia::ACTION_PRIORITY, priority}}}};

		auto const [error_code, error_string] = tegia::node::instance()->send_message_to_server(std::move(output));
		if (error_code != SUCCESS && error_string.has_value())
		{
			LERROR(*error_string)
			EOUT(*error_string)
		}
	}


	bool is_server_node() noexcept
	{
		return tegia::node::instance()->is_server_node();
	}

	bool is_client_node() noexcept
	{
		return tegia::node::instance()->is_client_node();
	}

	std::string node_uuid() noexcept { return tegia::node::instance()->node_uuid(); }

}	// END namespace tegia::sockets


namespace tegia {
namespace context {

	std::string user_uuid()
	{
		return "uuid";
	};

}	// END namespace context
}	// END namespace tegia 




namespace tegia {
namespace message {
	int send(
		const std::string &actor_name, 
		const std::string &action_name, 
		const std::shared_ptr<message_t> &message,
		int priority)
	{
		// std::cout << "[1] tegia::message::send()" << std::endl;
		return tegia::node::instance()->send_message(actor_name, action_name, message, priority);
	};

	int send(
		const std::string &actor_name, 
		const std::string &action_name, 
		nlohmann::json data,
		int priority)
	{
		// std::cout << "[1] tegia::message::send() " << actor_name << " " << action_name << std::endl;
		return tegia::node::instance()->send_message(actor_name, action_name, data, priority);
	};

	const std::shared_ptr<message_t> init(nlohmann::json data)
	{
		// std::shared_ptr<message_t> message(new message_t( data ));
		return std::make_shared<message_t>(data);
	};

}	// end namespace message
}	// end namespace tegia



namespace tegia {
namespace conf {

	std::string path(const std::string &config_name)
	{
		auto conf = tegia::node::instance()->get_config(config_name);
		if( (*conf)["path"] == nullptr)
		{
			return "";
		}
		else
		{
			return (*conf)["path"]->get<std::string>();
		}
	};

	tegia::config * load(
		const std::string &config_name, 
		const std::string &filename)
	{
		return tegia::node::instance()->load_config(config_name, filename);
	};

	tegia::config * get(const std::string & config_name)
	{
		return tegia::node::instance()->get_config(config_name);
	};

}	// end namespace conf
}	// end namespace tegia




/*
namespace tegia {

	tegia::config * load_config(
		const std::string &config_name, 
		const std::string &filename)
	{
		return tegia::node::instance()->load_config(config_name, filename);
	};

	tegia::config * get_config(const std::string & config_name)
	{
		return tegia::node::instance()->get_config(config_name);
	};

}  // end namespace tegia

*/

