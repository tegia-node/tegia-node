#include <tegia/sockets/ServerSocket.h>

namespace tegia::sockets
{
    ///
    ///  Initializes TCP/IP server socket and binds to the port.
    ///
    bool ServerSocket::initialize_socket() 
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
            int optval = 1;
            ::setsockopt(socket_id_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

            ::fcntl(socket_id_, F_SETFL, O_NONBLOCK);
        }

        //
        //  Bind socket to the specified port:
        //
        if (!is_connected())
        {
            if (::bind(socket_id_, reinterpret_cast<sockaddr*>(&socket_address_), sizeof(socket_address_)) != SUCCESS ||
                ::listen(socket_id_, MAX_CLIENTS) != SUCCESS)
            {
                auto const str = fmt::format("Cannot bind TCP/IP socket to '{}:{}': {}.", 
                    ::inet_ntoa(socket_address_.sin_addr), ntohs(socket_address_.sin_port), ::strerror(errno));
                LWARNING(str)
                EOUT(str)
            }
        }
            
        return true;
    }


    ///
    ///  Closes all opened socket file descriptors.
    ///
    void ServerSocket::close_socket_descriptors() 
    {
        {
            std::scoped_lock lock{ socket_guard_ };
            //
            //  Close client sockets first (if any exists)
            //
            for (auto const& [client_socket_id, client_address] : client_sockets_)
            {
                ::close(client_socket_id);
            }
            client_sockets_.clear();
        }

        AbstractSocket::close_socket_descriptors();
    }


    //
    // Fills-in the required structure with socket file descriptors which we want to observe.
    //
    int ServerSocket::fill_in_fd(fd_set& sockets) const noexcept
    {
        FD_ZERO(&sockets);
        FD_SET(socket_id_, &sockets);
        int max_id = socket_id_;

        //
        //  Here we don't need the lock because it's always executed in the single listener thread
        //
        for (auto const [client_socket_id, client_address] : client_sockets_)
        {
            FD_SET(client_socket_id, &sockets);
            if (client_socket_id > max_id) max_id = client_socket_id;
        }

        return max_id;
    }


    ///
    /// Sends TCP/IP message to the specified socket.
    ///
    error_data ServerSocket::send_message(sockaddr_in const socket_address, std::string_view const message_body) noexcept
    {
       int socket_id{};

       {
            std::scoped_lock lock { socket_guard_ };
            //
            //  Find if specified client socket exists (is connected to the server)
            //
            auto const it = std::find_if(client_sockets_.cbegin(), client_sockets_.cend(), 
                [socket_address](auto const& item)
                { return item.second.sin_port == socket_address.sin_port && item.second.sin_addr.s_addr == socket_address.sin_addr.s_addr; });
            if (it == client_sockets_.cend())
            {
                return std::make_pair(NOT_FOUND_ERROR, std::make_optional(fmt::format("Cannot send TCP/IP message: no connected socket found for '{}:{}'.", 
                    ::inet_ntoa(socket_address.sin_addr), ntohs(socket_address.sin_port))));
            }
            socket_id = it->first;
        }
        
        auto rc = AbstractSocket::send_message(socket_id, socket_address, message_body);
        if (rc.first != SUCCESS)
        {
            //
            //  The connection was lost or the error happened - close the connection and remove socket
            //
            {
                std::scoped_lock lock { socket_guard_ };
                client_sockets_.erase(socket_id);
            }
            ::close(socket_id);
            //
            //  Asks node to perform some actions on client connection close (usually remove socket address from the list)
            //
            if (auto const close_handler = process_client_close_.load(); close_handler != nullptr) 
            {
                close_handler(socket_address);
            }

            auto const str = fmt::format("Connection with the client socket on '{}:{}' was closed.", 
                ::inet_ntoa(socket_address.sin_addr), ntohs(socket_address.sin_port));
            LNOTICE(str)
            NOUT(str)
        }
        return rc;
    }


    ///
    /// Sends TCP/IP message to all client sockets.
    ///
    error_data ServerSocket::send_message(std::string_view const message_body) noexcept
    {
        error_data rc {SUCCESS, std::string{"TCP/IP message was sent to all clients"}};
        auto it = client_sockets_.begin();
        auto const end = client_sockets_.end();

        std::scoped_lock lock { socket_guard_ };
        while (it != end)
        {
            auto rc1 = AbstractSocket::send_message(it->first, it->second, message_body);
            if (rc1.first != SUCCESS) 
            {
                auto const client_address = it->second;

                ::close(it->first);
                it = client_sockets_.erase(it);
                rc = std::move(rc1);

                //
                //  Asks node to perform some actions on client connection close (usually remove socket address from the list)
                //
                if (auto const close_handler = process_client_close_.load(); close_handler != nullptr) 
                {
                    close_handler(client_address);
                }

                auto const str = fmt::format("Connection with the client socket on '{}:{}' was closed.", 
                    ::inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
                LNOTICE(str)
                NOUT(str)
                continue;
            }
            ++it;
        }

        return rc;
    }


    ///
    /// Accepts client connections and gets data from the client sockets:
    ///
    bool ServerSocket::process_events(int events_number, fd_set& sockets) noexcept
    {
        //
        //  Loop for the already connected clients:
        //
        auto it = client_sockets_.begin();
        while (events_number > 0 && it != client_sockets_.end())
        {
            if (FD_ISSET(it->first, &sockets))
            {
                --events_number;
                //
                // Obtained data from the client. Try to read and process them.
                //
                if (!read_data(it->first, it->second, MSG_DONTWAIT))
                {
                    auto const client_address = it->second;

                    //
                    //  The connection was lost or the error happened - close the connection and remove socket
                    //
                    {
                        std::scoped_lock lock{ socket_guard_ };
                        ::close(it->first);
                        it = client_sockets_.erase(it);
                    }

                    //
                    //  Asks node to perform some actions on client connection close (usually remove socket address from the list)
                    //
                    if (auto const close_handler = process_client_close_.load(); close_handler != nullptr) 
                    {
                        close_handler(client_address);
                    }

                    auto const str = fmt::format("Connection with the client socket on '{}:{}' was closed.", 
                        ::inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
                    LNOTICE(str)
                    NOUT(str)

                    continue;
                }
            }
            ++it;
        }

        //
        //  New client connection request has come, try to accept it:
        //
        if (events_number > 0 && FD_ISSET(socket_id_, &sockets))
        {
            int tries = MAX_TRIES;
            while (!accept_client() && tries > 0)
            {
                std::this_thread::sleep_for(WAIT_FOR_NEXT_TRY);
                --tries;
            }
        }

        return true;
    }


    //
    //  Accepts the new client connection and creates corresponding client socket.
    //
    bool ServerSocket::accept_client() noexcept
    {
        sockaddr_in client_address{};
        socklen_t socket_size  = sizeof client_address;
        ::memset(&client_address, 0, socket_size);

        int const client_socket_id = ::accept(socket_id_, reinterpret_cast<sockaddr*>(&client_address), &socket_size);

        if (client_socket_id < 0)
        {
            auto const str = fmt::format("Error #{} happened while establishing connection with client: {}.", errno, ::strerror(errno));
            LERROR(str)
            EOUT(str)
            return false;
        }

        ::fcntl(client_socket_id, F_SETFL, O_NONBLOCK);

        {
            std::scoped_lock lock{ socket_guard_ };
            client_sockets_[client_socket_id] = client_address;
        }
    
        //
        //  Asks node to perform some actions on client connection (usualy send welcome message)
        //
        if (auto const connect_handler = process_connect_.load(); connect_handler != nullptr) 
        {
            connect_handler(client_address);
        }

        auto const str = fmt::format("Established connection with the client socket id = {} on '{}:{}'.", 
            client_socket_id, ::inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
        LNOTICE(str)
        NOUT(str)
    

        return true;
    }

}