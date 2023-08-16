#ifndef H_TEGIA_PLATFORM
#define H_TEGIA_PLATFORM

////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// INCLUDES                                                                               //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////
#include <tegia/const.h>
#include <tegia/core/core.h>

#include <tegia/node/logger.h>
#include <tegia/node/config.h>
#include <tegia/actors/actor_list.h>
#include <tegia/sockets/SocketManager.h>
#include "../threads/thread_pool.h"

#include <tegia/core/person.h>

////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// DEFINES                                                                                //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// NODE CLASS                                                                             //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

namespace tegia
{
	class node final 
	{
      private:	
		std::map<std::string, tegia::config *> config_map;
		tegia::actors::list *actors_list = nullptr;
		tegia::logger *logger = nullptr;

		mutable std::shared_mutex remote_nodes_guard_{};
		std::unordered_map<std::string, sockaddr_in> client_nodes_{};

		std::unique_ptr<tegia::sockets::SocketManager> const& socket_manager_ = tegia::sockets::SocketManager::instance();

		std::string uuid_{};
		tegia::sockets::NodeType node_type_ = tegia::sockets::NodeType::STANDALONE;

		static node *_self;

  		static void process_socket_message(sockaddr_in const socket_address, std::string_view const message) noexcept
		{
			instance()->process_socket_message_(socket_address, message);
		}

		void process_socket_message_(sockaddr_in socket_address, std::string_view message) noexcept;
		void process_action_message_(nlohmann::json& json_data) noexcept;
		void process_welcom_message_(sockaddr_in socket_address, nlohmann::json const& json_data) noexcept;
		void process_config_message_(sockaddr_in socket_address, nlohmann::json& json_data) noexcept;
		void on_socket_connected(std::string node_uuid, sockaddr_in socket_address) noexcept;

  		static void process_socket_connection(sockaddr_in const socket_address) noexcept
		{
			instance()->process_socket_connection_(socket_address);
		}

		void process_socket_connection_(sockaddr_in socket_address) noexcept;

  		static void process_client_close(sockaddr_in const socket_address) noexcept
		{
			instance()->process_client_close_(socket_address);
		}

		void process_client_close_(sockaddr_in socket_address) noexcept;

		void do_exit() noexcept;

		node();

      public:

		node(node const&) = delete;
		node(node&&) noexcept = delete;
		node& operator = (node const&) = delete;
		node& operator = (node&&) noexcept = delete;

		~node();
		
		static tegia::sockets::error_data send_message_to_server_socket(std::string_view message_body) noexcept
		{
			return instance()->socket_manager_->send_message_to_server_socket(message_body);
		}

		static tegia::sockets::error_data send_message_to_client_socket(sockaddr_in const socket_address, std::string_view message_body) noexcept
		{
			return instance()->socket_manager_->send_message_to_client_socket(socket_address, message_body);
		}

		void init();
		static node *instance();
		static void stop_node(int signum);

		bool is_server_node() const noexcept { return node_type_ == tegia::sockets::NodeType::SERVER; }
		bool is_client_node() const noexcept { return node_type_ == tegia::sockets::NodeType::CLIENT; } 
		std::string node_uuid() const noexcept { return uuid_; } 

		tegia::sockets::error_data send_message_to_client(std::string const& node_uuid, nlohmann::json&& json_data) noexcept;
		tegia::sockets::error_data send_message_to_server(nlohmann::json&& json_data) const noexcept;

		tegia::config *load_config(
			const std::string &config_name,
			const std::string &filename);

		tegia::config *get_config(const std::string &config_name);

		int send_message(
			const std::string &actor_name,
			const std::string &action_name,
			const std::shared_ptr<message_t> &message,
			int priority = _PLOW_);

		int send_message(
			const std::string &actor_name,
			const std::string &action_name,
			nlohmann::json data = nlohmann::json(),
			int priority = _PLOW_);

		std::string call_actor(
			const std::string &actor_name,
			const std::string &action_name,
			const std::shared_ptr<message_t> &message);

		tegia::actors::actor_base * get_actor(const std::string &actor_name);
		tegia::actors::actor_base * create_actor(const std::string &actor_type, const std::string &actor_name, bool share, nlohmann::json &data);
		bool delete_actor(const std::string &actor_name);

		std::string find_pattern(
			const std::string &actor_name,
			const std::string &action_name);


		int thread_count = 10;
		
	}; // end class node

} // end namespace tegia

#endif