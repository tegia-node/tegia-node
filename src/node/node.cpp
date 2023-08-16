#undef  _DEBUG_PRINT_MODE_
#define _DEBUG_PRINT_MODE_ false
#include <tegia/node/logger_print.h>

// -------------------------------------------------------------------------------------- //
//                                      INCLUDES                                          //
// -------------------------------------------------------------------------------------- //
#include <signal.h>

#include "node.h"

// -------------------------------------------------------------------------------------- //
//                                        CLASS                                           //
// -------------------------------------------------------------------------------------- //


namespace tegia {

node* node::_self = nullptr;

////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////////////////


node* node::instance()
{  
	if(_self == nullptr)
	{
		_self = new node();
		_self->init();
	}
	return _self;
};


////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////////////////


void node::stop_node(int const signum)
{
	if (signum != SIGINT) return; 
	if(_self != nullptr)
	{
		_self->do_exit();
		delete _self;
		_self = nullptr;
	}
	::exit(0);
}


////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////////////////



void node::do_exit() noexcept
{
	socket_manager_->close_socket();
}



////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////////////////


node::node(){}; 


////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////////////////


node::~node()
{ 
	delete actors_list;

	for(auto it = this->config_map.begin(); it != this->config_map.end(); it++)
	{
		delete it->second;
	}

	delete this->logger;
};


////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////////////////


void node::init()
{
	//
	// Инициализируем логирование
	//

	this->logger = tegia::logger::instance("./",0);

	//
	// Загружаем конфигурацию роли tegia_node
	//

	auto conf = this->load_config("node","./config.json");
	if(conf == nullptr)
	{
		std::cout << _ERR_TEXT_ << "load node role config" << std::endl;
		exit(0);
	}

	//
	// uuid
	//

	if(nullptr == (*conf)["uuid"])
	{
		std::cout << _ERR_TEXT_ << "not found node uuid" << std::endl;
		exit(0);
	}
	this->uuid_ = (*conf)["uuid"]->get<std::string>();

	//auto const* const uuid_ptr = (*conf)["UUID"];
	//uuid_ = uuid_ptr != nullptr && uuid_ptr->is_string() ? uuid_ptr->get<std::string>() : tegia::random::uuid();

	//
	// Запускаем потоки для выполнения акторов
	//

	unsigned int n = std::thread::hardware_concurrency();
	auto count_thread = (*conf)["/thread/count"_json_pointer];
	if(count_thread == nullptr)
	{
		std::cout << _ERR_TEXT_ << " config file must be a contain [thread] data" << std::endl;
		std::cout << "4 thread init default" << std::endl;
		std::cout << n << " concurrent threads are supported." << std::endl;
		this->thread_count = 4;
		auto pool = tegia::threads::pool::instance(4);
	}
	else
	{
		this->thread_count = count_thread->get<int>();
		std::cout << this->thread_count << " thread init" << std::endl;
		std::cout << n << " concurrent threads are supported." << std::endl;
		auto pool = tegia::threads::pool::instance(this->thread_count);
	}
	
	//
	// Pагружаем конфигурации акторов
	//

	actors_list = new tegia::actors::list();
	actors_list->load_configurations();

	//
	//	Инициируем сокеты (if configured)
	//
	if (auto* const socket_config = (*conf)[tegia::sockets::SOCKET_SECTION]; socket_config != nullptr)
	{
		node_type_ = socket_manager_->
			init_socket(*socket_config, &node::process_socket_message, &node::process_socket_connection, &node::process_client_close);
	}

	//
	// Инициализируем конфигурации акторов
	//

	actors_list->init();
	tegia::person::names::instance();

	NOUT(fmt::format("Node UUID = '{}', node type = '{}'.", uuid_, (is_server_node() ? "SERVER" : (is_client_node() ? "CLIENT" : "STANDALONE"))))

	std::cout << _GREEN_TEXT_ << " " << std::endl;	
	std::cout << "-------------------------------" << std::endl;	
	std::cout << "[OK] tegia::platform::node load" << std::endl;	
	std::cout << "-------------------------------" << std::endl;	
	std::cout << _BASE_TEXT_ << std::endl;	

	/*
	nlohmann::json data = nlohmann::json::object();
	for(long long int i = 0; i < 10000; ++i)
	{
		tegia::actors::_new("MPerson","person/" + core::cast<std::string>(i),data);
	}

	std::cout << _GREEN_TEXT_ << " " << std::endl;	
	std::cout << "-------------------------------" << std::endl;	
	std::cout << "[OK] actors load" << std::endl;	
	std::cout << "-------------------------------" << std::endl;	
	std::cout << _BASE_TEXT_ << std::endl;	
	*/

};


////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////////////////

tegia::config * node::load_config(const std::string &config_name, const std::string &filename)
{
	
	auto pos = this->config_map.find(config_name);
	if(pos == this->config_map.end())
	{
		auto conf = new tegia::config(config_name,filename);
		this->config_map.insert({config_name, conf});
		return conf;
	}
	return pos->second;
};

////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////////////////


tegia::config * node::get_config(const std::string & config_name)
{
	auto pos = this->config_map.find(config_name);
	if(pos != this->config_map.end())
	{
		return pos->second;
	}
	return nullptr;
};


////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////////////////


int node::send_message(
	const std::string &actor_name, 
	const std::string &action_name, 
	const std::shared_ptr<message_t> &message,
	int priority)
{
	// std::cout << "[2] node::send_message()" << std::endl;
	return this->actors_list->send_message(actor_name, action_name, message, priority);
};


////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////////////////


int node::send_message(
	const std::string &actor_name, 
	const std::string &action_name, 
	nlohmann::json data,
	int priority)
{
	return this->actors_list->send_message(actor_name, action_name, data, priority);
};


////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////////////////


tegia::actors::actor_base * node::get_actor(const std::string &actor_name)
{
	return this->actors_list->get_actor(actor_name);
};


////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////////////////


tegia::actors::actor_base * node::create_actor(const std::string &actor_type,const std::string &actor_name, bool const share, nlohmann::json &data)
{
	return this->actors_list->create_actor(actor_type, actor_name, share, data);
};


////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////////////////


bool node::delete_actor(const std::string &actor_name)
{
	return this->actors_list->delete_actor(actor_name);
};


////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////////////////


	std::string node::find_pattern(const std::string &actor_name, const std::string &action_name)
	{
		// std::cout << "node::find_pattern()" << std::endl;
		auto pattern = this->actors_list->find_pattern(actor_name, action_name);
		// If pattern was not found for client node it should not be considered as error - probably it can be on server node!
		if (pattern.empty() && is_client_node()) pattern = action_name;
		return pattern;
	};


	///
	/// Process 'tegia' message obtained from another 'node' via TCP/IP socket.
	///
	void node::process_socket_message_(sockaddr_in const socket_address, std::string_view const socket_message) noexcept
	{
		// std::string( symbol.strVw()
		// auto message_data = nlohmann::json::parse(core::string::decompress_string(std::string(socket_message)), nullptr, false, true);
		auto message_data = nlohmann::json::parse(socket_message, nullptr, false, true);

		//
		//	Process message with action request.
		//
		if (message_data.contains(ACTION_MESSAGE_DATA))
		{
			if (!message_data[ACTION_MESSAGE_DATA].is_null())
			{
				process_action_message_(message_data[ACTION_MESSAGE_DATA]);
			}
		}
		//
		//	Process handshake message with client tegia-node configuration sent.
		//
		else if (message_data.contains(CONFIG_MESSAGE_DATA))
		{
			if (!message_data[CONFIG_MESSAGE_DATA].is_null())
			{
				process_config_message_(socket_address, message_data[CONFIG_MESSAGE_DATA]);
			}
		}
		//
		//	Process message with server-node welcome request.
		//
		else if (message_data.contains(WELCOME_DATA))
		{
			if (!message_data[WELCOME_DATA].is_null())
			{
				process_welcom_message_(socket_address, message_data[WELCOME_DATA]);
			}
		}
		//
		//	Message with unknown format received - nothing to do!
		//
		else
		{
			NOUT(fmt::format("Message received from '{}:{}': '{}'", inet_ntoa(socket_address.sin_addr), ntohs(socket_address.sin_port), socket_message))
		}		
	}


	///
	/// Process special message obtained from client-node at handshake/configuration change with its actors configuration.
 	/// Note: is used in server-node only!
	///
	void node::process_config_message_(sockaddr_in const socket_address, nlohmann::json& json_data) noexcept
	{
		//NOUT(fmt::format("Config message {} obtained from client '{}:{}'", json_data.dump(), ::inet_ntoa(socket_address.sin_addr), ntohs(socket_address.sin_port)))
		if (is_server_node())
		{
			if (json_data.contains(NODE_UUID) && json_data[NODE_UUID].is_string())
			{
    			auto node_uuid = json_data[NODE_UUID].get<std::string>();
				{
					std::scoped_lock lock{ remote_nodes_guard_ };
					client_nodes_[node_uuid] = socket_address;
				}

				if (json_data.contains(HANDSHAKE_MESSAGE))
				{
					//
					// Process handshake message - initial client configuration is loading in blocking mode
					//
					if (json_data.contains(ADD_ACTOR_LIST) && json_data[ADD_ACTOR_LIST].is_array())
					{
						actors_list->add_client_node_actors(node_uuid, std::move(json_data[ADD_ACTOR_LIST]));
					}	
							
					//
					//	Send reply to the client
					//
					auto fn1 = [socket_address, json_data = nlohmann::json {{WELCOME_DATA, {{HANDSHAKE_MESSAGE, true}}}}] () -> void
					{ 
						auto const [error_code, error_string] = node::send_message_to_client_socket(socket_address, json_data.dump());
						if (error_code != tegia::sockets::SUCCESS && error_string.has_value())
						{
							LERROR(*error_string)
							EOUT(*error_string)
						}
					};
						
					tegia::threads::add_task(fn1, _PHIGHT_);

					//
					//	Run actions possibly defined in node's configuration
					//
					auto fn2 = [this, socket_address, node_uuid = std::move(node_uuid)] () -> void
					{ 
						on_socket_connected(std::move(node_uuid), socket_address);
					};
					tegia::threads::add_task(fn2, _PLOW_);
				}
				else
				{
					//
					// Process possible actor creation and/or deleting messages (in non-blocking mode)
					//
					if (json_data.contains(ADD_ACTOR_LIST) && json_data[ADD_ACTOR_LIST].is_array())
					{
						auto fn = [this, node_uuid = std::move(node_uuid), actors = std::move(json_data[ADD_ACTOR_LIST])] () -> void
						{ 
							actors_list->add_client_node_actors(std::move(node_uuid), std::move(actors));
						};
							
						tegia::threads::add_task(fn, _PHIGHT_);
					}	
					if (json_data.contains(REMOVE_ACTOR_LIST) && json_data[REMOVE_ACTOR_LIST].is_array())
					{
						auto fn = [this, actors = std::move(json_data[REMOVE_ACTOR_LIST])] () -> void
						{ 
							actors_list->remove_client_node_actors(std::move(actors));
						};
							
						tegia::threads::add_task(fn, _PHIGHT_);
					}	
				}
			}	
		}
	}


	///
	/// Process welcome message obtained from server-node at handshake - send the list of locally configured actors. 
	/// Note: is used in client-node only!
	///
	void node::process_welcom_message_(sockaddr_in socket_address, nlohmann::json const& json_data) noexcept
	{
		//NOUT(fmt::format("Welcome message {} obtained from server-node.", json_data.dump()))
		if (is_client_node())
		{
			//
			//	Just double-check that message is from valid tegia server-node:
			//
			if (json_data.contains(NODE_UUID))
			{
				//
				//	Send client's configuration (list of actors/actions) to the server node
				//
				if (actors_list != nullptr)
				{
					auto fn = [ json_data = nlohmann::json {{CONFIG_MESSAGE_DATA, {{NODE_UUID, uuid_}, {HANDSHAKE_MESSAGE, true}, 
						{ADD_ACTOR_LIST, actors_list->shared_local_actors()}}}} ] () -> void
					{ 
						auto const [error_code, error_string] = node::send_message_to_server_socket(json_data.dump());
						if (error_code != tegia::sockets::SUCCESS && error_string.has_value())
						{
							LERROR(*error_string)
							EOUT(*error_string)
						}
					};
						
					tegia::threads::add_task(fn, _PHIGHT_);
				}
			}
			else if (json_data.contains(HANDSHAKE_MESSAGE))
			{
				//
				//	Run actions possibly defined in node's configuration
				//
				auto fn = [this, socket_address] () -> void
				{ 
					on_socket_connected("", socket_address);
				};
				tegia::threads::add_task(fn, _PLOW_);
			}
		}
	}


	///
	/// Process 'tegia' message obtained from another 'node' requesting for some action.
	///
	void node::process_action_message_(nlohmann::json& json_data) noexcept
	{
		//NOUT(fmt::format("Action message {} obtained from another node.", json_data.dump()))

		int priority = json_data.contains(tegia::ACTION_PRIORITY) && json_data[tegia::ACTION_PRIORITY].is_number_integer() ?
			json_data[tegia::ACTION_PRIORITY].get<int>() : _PLOW_;

		auto fn = [this, priority, json_data = std::move(json_data)] () mutable -> void
		{ 
			if (json_data.contains(tegia::ACTOR_NAME)  && json_data[tegia::ACTOR_NAME].is_string() &&
				json_data.contains(tegia::ACTION_NAME) && json_data[tegia::ACTION_NAME].is_string())
			{
				auto const actor_name  = json_data[tegia::ACTOR_NAME].get<std::string>();
				auto const action_name = json_data[tegia::ACTION_NAME].get<std::string>();
				
				auto const message = std::make_shared<message_t>();
				if (json_data.contains(tegia::ACTION_MESSAGE) && !json_data[tegia::ACTION_MESSAGE].is_null()) 
				{
					message->deserialize(json_data[tegia::ACTION_MESSAGE]);
				}

       			tegia::message::send(actor_name, action_name, message);
			}
		};

		tegia::threads::add_task(fn, priority);
	}


	///
	/// When any client has disconnected remove its socket address from node's configuration.
 	/// Note: is used in server-node only!
	///
	void node::process_client_close_(sockaddr_in const socket_address) noexcept
	{
		if (is_server_node())
		{
		    std::scoped_lock lock{ remote_nodes_guard_ };
			if (auto const it = std::find_if(client_nodes_.cbegin(), client_nodes_.cend(), [socket_address] (auto const& item)
				{ return item.second.sin_port == socket_address.sin_port && item.second.sin_addr.s_addr == socket_address.sin_addr.s_addr; }); 
				it != client_nodes_.cend())
			{
				client_nodes_.erase(it);
				NOUT(fmt::format("Remove address for disconnected client '{}:{}'", ::inet_ntoa(socket_address.sin_addr), ntohs(socket_address.sin_port)))
			}
		}
	}


	///
	/// When client node has connected to server node - send the welcome notification message.
	//
	void node::process_socket_connection_(sockaddr_in const socket_address) noexcept
	{
		//NOUT(fmt::format("Request from socket '{}:{}'", ::inet_ntoa(socket_address.sin_addr), ntohs(socket_address.sin_port)))
		if (is_server_node()) 
		{
			auto fn = [socket_address, json_data = nlohmann::json {{WELCOME_DATA, {{NODE_UUID, uuid_}}}}] () -> void
			{ 
				std::this_thread::sleep_for(tegia::sockets::WAIT_FOR_ERROR);
				auto const [error_code, error_string] = node::send_message_to_client_socket(socket_address, json_data.dump());
				if (error_code != tegia::sockets::SUCCESS && error_string.has_value())
				{
					LERROR(*error_string)
					EOUT(*error_string)
				}
			};
				
			tegia::threads::add_task(fn, _PHIGHT_);
		}
	}


	///
	/// Sends action message to the remote client tegia node via TCP/IP socket.
	///
	tegia::sockets::error_data node::send_message_to_client(std::string const& node_uuid, nlohmann::json&& json_data) noexcept
	{
		tegia::sockets::error_data rc {tegia::sockets::SUCCESS, std::nullopt};
		if (is_server_node() && ! node_uuid.empty() && !json_data.is_null() && !json_data.empty())
		{
			sockaddr_in socket_address{};
			auto socket_message = json_data.dump();

			{
				//
				// Get the address of the required socket. Client sockets are defined by their UUID, server socket is always the only one first item.
				//
				std::shared_lock lock{ remote_nodes_guard_ };
				if (auto const it = client_nodes_.find(node_uuid); it == client_nodes_.end())
				{
					rc = std::make_pair(tegia::sockets::NOT_FOUND_ERROR, 
						std::make_optional(fmt::format("Cannot send TCP/IP message: socket address is not found for node '{}'.", node_uuid)));
				}
				else
				{
					socket_address = it->second;
				}
			}

			//
			//	In case if required socket address was successfully found, try to send message to it
			//
			if (rc.first == tegia::sockets::SUCCESS)
			{
				// auto const socket_message = core::string::compress_string(json_data.dump());
				if (rc = socket_manager_->send_message_to_client_socket(socket_address, socket_message); rc.first == tegia::sockets::NOT_FOUND_ERROR)
				{
					//
					// In case of not found socket with the specified address, remove the the entry for this client tegia node in the map
					//
					rc = std::make_pair(tegia::sockets::NOT_FOUND_ERROR, 
						std::make_optional(fmt::format("Cannot send TCP/IP message: socket not found for address '{}:{}'. Removing node '{}'...", 
						::inet_ntoa(socket_address.sin_addr), ntohs(socket_address.sin_port), node_uuid)));
					std::scoped_lock lock{ remote_nodes_guard_ };
					client_nodes_.erase(node_uuid);
				}
				//
				// If socket is not connected cash the message to try to resend it later on
				//
				else if (rc.first != tegia::sockets::BAD_REQUEST_ERROR) 
				{
					return rc;
				}
			}
			
			//
			// If the message cannot be sent, cash the message to try to resend it later on.
			//
			socket_manager_->add_message_to_queue(node_uuid, std::move(socket_message));
		}
		else
		{
			rc.first = tegia::sockets::BAD_REQUEST_ERROR;
		}
		return rc;
	}


	///
	/// Sends action message to the remote server tegia node via TCP/IP socket.
	///
	tegia::sockets::error_data node::send_message_to_server(nlohmann::json&& json_data) const noexcept
	{
		tegia::sockets::error_data rc {tegia::sockets::BAD_REQUEST_ERROR, std::nullopt};
		if (is_client_node() && !json_data.is_null() && !json_data.empty())
		{
			auto socket_message = json_data.dump();

			//
			//	Try to send message to the server socket (it is always a single one!)
			//
			rc = socket_manager_->send_message_to_server_socket(socket_message);
			if (rc.first == tegia::sockets::BAD_REQUEST_ERROR || rc.first == tegia::sockets::NOT_FOUND_ERROR)
			{
				//
				// If the socket is not connected or found, cash the message to try to resend it later on.
				//
				socket_manager_->add_message_to_queue("", std::move(socket_message));
			}
		}
		return rc;
	}


	///
	/// Performs actions defined in node configuration after TCP/IP socket is established.
	///	On server-node is called after processing configuration message from client-node, 
	///	and on client-node is called after processing server welcome message.
	///
	void node::on_socket_connected(std::string node_uuid, sockaddr_in socket_address) noexcept
	{
		if (auto* const config = get_config("node"); config != nullptr)
		{
			if (auto* const connect = (*config)["connect"]; connect != nullptr)
			{
				for (auto const& item : *connect)
				{
					send_message(item["actor"].get<std::string>(), item["action"].get<std::string>(), item["message"]);
				}	
			}
		}
		//
		// Ask manager to send all the queued messages if any exists
		//
		if (is_client_node())
		{
            std::thread t{&tegia::sockets::SocketManager::send_queued_server_messages, socket_manager_.get()};
            t.detach();
		}
		else if (is_server_node())
		{
		 	std::thread t(&tegia::sockets::SocketManager::send_queued_client_messages, socket_manager_.get(), std::move(node_uuid), socket_address);
            t.detach();
		}

	}


} // end namespace tegia





