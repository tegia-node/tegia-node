#include "AbstractSocket.h"


namespace tegia::sockets
{

  class ServerSocket final : public AbstractSocket
  {	
    public:

      ServerSocket() : AbstractSocket() {}
      ServerSocket(std::string_view const ip_address, in_port_t const port_number) : AbstractSocket(ip_address, port_number) {}
      ServerSocket(std::string_view const ip_address, in_port_t const port_number, message_handler_func const process_message, 
        close_handler_func const process_close, connect_handler_func const process_connection, close_handler_func const process_client_close)
        : AbstractSocket(ip_address, port_number, process_message, process_close, process_connection) 
        {
          set_client_close_handler(process_client_close);
        }

      ~ServerSocket() noexcept override {};

      ServerSocket(ServerSocket const&) = delete;
      ServerSocket(ServerSocket&&) noexcept = delete;
      ServerSocket& operator = (ServerSocket const&) = delete;
      ServerSocket& operator = (ServerSocket&&) noexcept = delete;

      // ----------------------------------------------------------------------------------
      // API FUNCTIONS
      // ----------------------------------------------------------------------------------

      error_data send_message(std::string_view message_body) noexcept override;
      error_data send_message(sockaddr_in socket_address, std::string_view message_body) noexcept override;
      void set_client_close_handler(close_handler_func const process_client_close) noexcept 
      { 
        if (process_client_close != nullptr) process_client_close_.store(process_client_close); 
      }

    protected:

    private:
  
      std::map<int, sockaddr_in> client_sockets_{};
      std::atomic<close_handler_func> process_client_close_ = nullptr;

      // ----------------------------------------------------------------------------------   
      // SUPPORT FUNCTIONS 
      // ----------------------------------------------------------------------------------   

      bool accept_client() noexcept;

      void close_socket_descriptors() override;

      bool initialize_socket() override;
      int  fill_in_fd(fd_set& sockets) const noexcept override;
      bool process_events(int events_number, fd_set& sockets) noexcept override;
  };

}