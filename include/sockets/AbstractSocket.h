#pragma once

////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// INCLUDES                                                                               //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

#include <cstdlib>
#include <cstring>

//	C++ STL

#include <memory>
#include <map>
#include <string>
#include <string_view>
#include <atomic>
//#include <chrono> 
#include <mutex>
//#include <shared_mutex>
#include <optional>
#include <tuple>
#include <thread>

// Socket support
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <fcntl.h>

//	VENDORS 
#include <fmt/core.h>

//#define DEVELOPER_VERSION

//  Common project constants and methods
#include "SocketConstants.h"
#include "SocketHelper.h"


namespace tegia::sockets
{
  class AbstractSocket
  {	
    public:

      AbstractSocket();
      AbstractSocket(std::string_view const ip_address, in_port_t const port_number) : AbstractSocket()
      {
        set_ip_address(ip_address, port_number);
        start_socket();
      }
      AbstractSocket(std::string_view const ip_address, in_port_t const port_number, 
        message_handler_func process_message, close_handler_func process_close, connect_handler_func process_connection) 
        : AbstractSocket(ip_address, port_number)
      {
        set_message_handler(process_message); 
        set_close_handler(process_close); 
        set_connection_handler(process_connection);
      }

      virtual ~AbstractSocket() noexcept;

      AbstractSocket(AbstractSocket const&) = delete;
      AbstractSocket(AbstractSocket&&) noexcept = delete;
      AbstractSocket& operator = (AbstractSocket const&) = delete;
      AbstractSocket& operator = (AbstractSocket&&) noexcept = delete;

      // ----------------------------------------------------------------------------------
      // API FUNCTIONS
      // ----------------------------------------------------------------------------------

      bool is_connected() const noexcept { return is_connected_.load(); }

      //
      // These event handlers are always called from the socket's listener thread!
      //
      void set_message_handler(message_handler_func const process_message) noexcept { if (process_message != nullptr) process_message_.store(process_message); }
      void set_close_handler(close_handler_func const process_close) noexcept { if (process_close != nullptr) process_close_.store(process_close); }
      void set_connection_handler(connect_handler_func const process_connection) noexcept { if (process_connection != nullptr) process_connect_.store(process_connection); }


      bool start_socket();
      bool start_socket(std::string_view ip_address, in_port_t port_number);

      void close_socket();

      virtual error_data send_message(std::string_view message_body) noexcept = 0;
      virtual error_data send_message(sockaddr_in socket_address, std::string_view message_body) noexcept = 0;
      error_data send_message(std::string_view ip_address, in_port_t port_number, std::string_view message_body) noexcept;

      sockaddr_in get_socket_address() const noexcept { return socket_address_; }

    protected:

      std::atomic<bool> is_connected_  = false;
      std::atomic<bool> is_connecting_ = false;
      std::mutex socket_guard_{};

      sockaddr_in socket_address_{};
      std::atomic<int> socket_id_ = INVALID_SOCKET;

      std::atomic<connect_handler_func> process_connect_ = nullptr;

      // ----------------------------------------------------------------------------------   
      // SUPPORT FUNCTIONS 
      // ----------------------------------------------------------------------------------   

      bool is_connecting() const noexcept { return is_connecting_.load(); }

      error_data send_message(int socket_id, sockaddr_in ip_address, std::string_view message_body) noexcept;

      virtual void close_socket_descriptors();

      virtual bool process_events(int events_number, fd_set& sockets) noexcept = 0;

      bool read_data(int socket_id, sockaddr_in ip_address, int flags) noexcept;

    private:

      void* send_buffer = nullptr;
      std::size_t send_buffer_size = 0U;

      void* read_buffer = nullptr;
      std::size_t read_buffer_size = 0U;

      std::atomic<message_handler_func> process_message_ = nullptr;
      std::atomic<close_handler_func> process_close_ = nullptr;

      std::unique_ptr<std::thread> listener_ = nullptr;

      void set_ip_address(std::string_view ip_address, in_port_t port_number) noexcept;

      ssize_t read_n_bytes(int socket_id, void* buf, std::size_t N, int flag) const;

      void stop_listener();
      bool start_listener();

      virtual bool initialize_socket() = 0;
      virtual int  fill_in_fd(fd_set& sockets) const noexcept = 0;

      void event_loop();

      int  wait_for_events(fd_set& sockets);
    };

}