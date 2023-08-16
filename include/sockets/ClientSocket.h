#include "AbstractSocket.h"


namespace tegia::sockets
{

  class ClientSocket final : public AbstractSocket
  {	
    public:

      ClientSocket() : AbstractSocket() {}
      ClientSocket(std::string_view const ip_address, in_port_t const port_number) : AbstractSocket(ip_address, port_number) {}
      ClientSocket(std::string_view const ip_address, in_port_t const port_number, message_handler_func process_message, 
        close_handler_func process_close, connect_handler_func process_connection)
        : AbstractSocket(ip_address, port_number, process_message, process_close, process_connection) {}

      ~ClientSocket() noexcept override {};

      ClientSocket(ClientSocket const&) = delete;
      ClientSocket(ClientSocket&&) noexcept = delete;
      ClientSocket& operator = (ClientSocket const&) = delete;
      ClientSocket& operator = (ClientSocket&&) noexcept = delete;

      // ----------------------------------------------------------------------------------
      // API FUNCTIONS
      // ----------------------------------------------------------------------------------

      error_data send_message(std::string_view message_body) noexcept override;
      error_data send_message(sockaddr_in socket_address, std::string_view message_body) noexcept override;

    protected:

    private:

      // ----------------------------------------------------------------------------------   
      // SUPPORT FUNCTIONS 
      // ----------------------------------------------------------------------------------   

      bool initialize_socket() override;
      int  fill_in_fd(fd_set& sockets) const noexcept override;
      bool process_events(int events_number, fd_set& sockets) noexcept override;
  
    };

}