#include <tegia/sockets/ClientSocket.h>

namespace tegia::sockets
{
    ///
    ///  Initializes TCP/IP client socket and tries to connect to the server.
    ///
    bool ClientSocket::initialize_socket() 
    {
        std::scoped_lock lock{ socket_guard_ };

        //
        //  Create and initialize socket
        //
        if (socket_id_ == INVALID_SOCKET)
        {
            if (socket_id_= ::socket(AF_INET, SOCK_STREAM, 0); socket_id_ == INVALID_SOCKET)
            {
                auto const str = fmt::format("Cannot create TCP/IP socket: {}.", strerror(errno));
                LERROR(str)
                EOUT(str)
                return false;
            }
        }

        //
        //  Connect socket to the server
        //
        if (!is_connected())
        {
            if (::connect(socket_id_, reinterpret_cast<sockaddr*>(&socket_address_), sizeof(socket_address_)) != SUCCESS)
            {
                auto const str = fmt::format("Cannot connect TCP/IP socket to server '{}:{}': {}.", 
                    ::inet_ntoa(socket_address_.sin_addr), ntohs(socket_address_.sin_port), ::strerror(errno));
                LWARNING(str)
                EOUT(str)
                return false;
            }

            if (auto const connect_handler = process_connect_.load(); connect_handler != nullptr) 
            {
                connect_handler(socket_address_);
            }
        }
        
        return true;
    }


    //
    // Fills-in the required structure with socket file descriptors which we want to observe.
    //
    int ClientSocket::fill_in_fd(fd_set& sockets) const noexcept
    {
        FD_ZERO(&sockets);
        FD_SET(socket_id_, &sockets);
        return socket_id_;
    }


    ///
    /// Reads message data from the client socket, parses them and performs the required action:
    ///
    bool ClientSocket::process_events(int events_number, fd_set& sockets) noexcept
    {
        //
        //  Incoming message from the server came, try to read it:
        //
        if (events_number > 0 && FD_ISSET(socket_id_, &sockets))
        {
            return read_data(socket_id_, socket_address_, MSG_DONTWAIT);
        }
        return true;
    }


    ///
    /// Sends TCP/IP message to the specified socket.
    ///
    error_data ClientSocket::send_message(sockaddr_in const socket_address, std::string_view const message_body) noexcept
    {
        //
        //  Check is the message should be sent to the server which client is connected to.
        //
        if (socket_address_.sin_port != socket_address.sin_port || socket_address_.sin_addr.s_addr != socket_address.sin_addr.s_addr)
        {
            return std::make_pair(NOT_FOUND_ERROR, std::make_optional(fmt::format("Cannot send TCP/IP message: requested socket has invalid address '{}:{}'.", 
                ::inet_ntoa(socket_address.sin_addr), ntohs(socket_address.sin_port))));
        }

        return send_message(message_body);
    }

    ///
    /// Sends TCP/IP message to the server socket.
    ///
    error_data ClientSocket::send_message(std::string_view const message_body) noexcept
    {
        auto rc = AbstractSocket::send_message(socket_id_, socket_address_, message_body);
        if (rc.first != SUCCESS) is_connected_.store(false);
        return rc;
    }
}