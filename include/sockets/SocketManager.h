#pragma once

////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// INCLUDES                                                                               //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

//	C++ STL

#include <memory>
#include <vector>
#include <string>
#include <string_view>
#include <atomic>
//#include <chrono> 
#include <shared_mutex>
#include <optional>
//#include <tuple>
#include <thread>

// Socket support
#include <netinet/ip.h>

//	VENDORS 
#include <nlohmann/json-schema.hpp>
#include <fmt/core.h>

//  Sockets
#include "ServerSocket.h"
#include "ClientSocket.h"
#include "SocketMessageQueue.h"

namespace tegia::sockets
{

    using socket_address = std::pair<std::optional<std::string>, std::optional<in_port_t>>;

    class SocketManager final
    {
    private:

      struct class_tag { int tag; };

    public:

      SocketManager() = delete;
      SocketManager(class_tag const tag) {};

      ~SocketManager() noexcept { close_socket(); };

      SocketManager(SocketManager const&) = delete;
      SocketManager(SocketManager&&) noexcept = delete;
      SocketManager& operator = (SocketManager const&) = delete;
      SocketManager& operator = (SocketManager&&) noexcept = delete;

      static std::unique_ptr<SocketManager>& instance() noexcept;

      NodeType init_socket(nlohmann::json const& config_data, message_handler_func process_received_message, 
          connect_handler_func process_connection, close_handler_func process_client_close);  

      void close_socket() noexcept;  

      error_data send_message_to_server_socket(std::string_view message_body) noexcept;    
      error_data send_message_to_client_socket(sockaddr_in socket_address, std::string_view message_body) noexcept;
      //error_data send_message(std::string_view ip_address, in_port_t port_number, std::string_view message_body) noexcept;

      static socket_address get_socket_address(nlohmann::json const& input_data) noexcept;

      static void process_received_message(sockaddr_in ip_address, std::string_view message) noexcept;

      static void process_close_socket(sockaddr_in const ip_address) noexcept
      {
        instance()->process_close_socket_(ip_address);
      }

      template <typename T>
      void add_message_to_queue(std::string const& node_uuid, T&& message)
      {
        if (message_queue_)  message_queue_->add_message(std::move(node_uuid), std::forward<T>(message));
      }

      void send_queued_server_messages() noexcept;
      void send_queued_client_messages(std::string&& node_uuid, sockaddr_in socket_address) noexcept;

    private:

      inline static std::unique_ptr<SocketManager> socket_manager_ = nullptr; 

		  std::unique_ptr<SocketMessageQueue> message_queue_ = nullptr;

      std::atomic<bool> is_closing_ = false;
      std::shared_mutex socket_guard_{};

      std::unique_ptr<AbstractSocket> socket_ = nullptr;
      //std::vector<std::unique_ptr<AbstractSocket>> client_sockets_{};

      template <typename T, typename... Args>
      error_data start_socket(nlohmann::json const& config_data, Args&&... args);

      void process_close_socket_(sockaddr_in ip_address) noexcept;

      static void restart_socket(sockaddr_in const ip_address) noexcept
      {
        instance()->restart_socket_(ip_address);
      }

      void restart_socket_(sockaddr_in ip_address) noexcept;
    };

}