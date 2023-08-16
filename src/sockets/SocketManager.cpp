#include <tegia/sockets/SocketManager.h>

namespace tegia::sockets
{
    ///
    /// Creates and return a singleton 'SocketManager' object
    ///
    std::unique_ptr<SocketManager>& SocketManager::instance() noexcept
    {
        if (!socket_manager_)
        {
            socket_manager_ = std::make_unique<SocketManager>(class_tag{});
        }
        return socket_manager_;
    }


    ///
    /// Reads configuration for TCP/IP socket, creates and starts the required object
    ///
    template <typename T, typename... Args>
    error_data SocketManager::start_socket(nlohmann::json const& config_data, Args&&... args)
    {
        if (auto const address = get_socket_address(config_data); !address.second.has_value())
        {
            return {BAD_REQUEST_ERROR, std::string("Cannot create TCP/IP socket: no port number is specified in the input data or it's invalid.")};
        }
        else
        {
            std::scoped_lock lock { socket_guard_ };
            socket_ = std::make_unique<T>(address.first.has_value() ? *address.first : "", *address.second, std::forward<Args>(args)...);
            message_queue_ = std::make_unique<SocketMessageQueue>();
 
            auto const ip_address = socket_->get_socket_address();
            constexpr char const* socket_type = std::is_same_v<T, ServerSocket> ? "server" : "client";
            constexpr char const* socket_stat = std::is_same_v<T, ServerSocket> ? "listening on" : "connected to";
            std::this_thread::sleep_for(WAIT_FOR_ERROR);

            if (!socket_->is_connected())
            {
                return {SERVICE_UNAVAILABLE_ERROR, fmt::format("TCP/IP {} socket is not {} '{}:{}'.", 
                    socket_type, socket_stat, ::inet_ntoa(ip_address.sin_addr), ntohs(ip_address.sin_port))};
            }
            else
            {
                return {SUCCESS, fmt::format("TCP/IP {} socket is {} '{}:{}'.", 
                    socket_type, socket_stat, ::inet_ntoa(ip_address.sin_addr), ntohs(ip_address.sin_port))};
           }
        }
    }


    ///
    /// Reads socket configuration data obtained from configuration file at startup,
    /// creates and starts corresponding socket defined in configuration.
    ///
    NodeType SocketManager::init_socket(nlohmann::json const& config_data, message_handler_func const process_received_message, 
        connect_handler_func const process_connection, close_handler_func const process_client_close) 
    {
        NodeType rc = NodeType::STANDALONE;
        error_data error = std::make_pair(BAD_REQUEST_ERROR, 
                           std::make_optional(std::string("Cannot create TCP/IP socket: no address is specified in the input data.")));

        //
        //  First try to create server socket if it is configured
        //
        if (config_data.contains(SERVER_SECTION) && !config_data[SERVER_SECTION].is_null())
        {
            rc = NodeType::SERVER;
            error = start_socket<ServerSocket>(config_data.at(SERVER_SECTION), 
                process_received_message != nullptr ? process_received_message : &SocketManager::process_received_message, 
                &SocketManager::process_close_socket, process_connection, process_client_close);
        }
        //
        //  Then try to create client socket
        //
        else if (config_data.contains(CLIENT_SECTION) && !config_data[CLIENT_SECTION].is_null())
        {
            rc = NodeType::CLIENT;
            auto client_data = nlohmann::json::object();
            if (config_data[CLIENT_SECTION].is_array() && !config_data[CLIENT_SECTION].empty()) client_data = config_data[CLIENT_SECTION][0];
            else if (config_data[CLIENT_SECTION].is_object()) client_data = config_data[CLIENT_SECTION];

            error = start_socket<ClientSocket>(client_data, 
                process_received_message != nullptr ? process_received_message : &SocketManager::process_received_message, 
                &SocketManager::process_close_socket, process_connection);
        }

        if (error.first != SUCCESS)
        {
            LERROR(*error.second)
            EOUT(*error.second)
        } 
        else
        {
            LNOTICE(*error.second)
            NOUT(*error.second)
        }

       return rc;
    }


    ///
    /// Closes opened socket and frees resources
    ///
    void SocketManager::close_socket()  noexcept
    {
        try
        {
            is_closing_.store(true);
            std::scoped_lock lock { socket_guard_ };
            if (message_queue_)
            {
                 message_queue_.reset(nullptr);
            }
            if (socket_)
            {
                socket_->close_socket();
                socket_.reset(nullptr);
            }
        }
        catch(const std::exception& e)
        {
            LERROR("Exception happened when closing socket: " << e.what())
            EOUT("Exception happened when closing socket: " << e.what())
        }
        
        is_closing_.store(false);
    }


    ///
    /// Sends message to server socket (if client socket is opened and connected)
    ///
    error_data SocketManager::send_message_to_server_socket(std::string_view const message_body) noexcept
    {
        std::shared_lock lock { socket_guard_ };
        return socket_ ? socket_->send_message(message_body) : error_data{SERVICE_UNAVAILABLE_ERROR, "TCP/IP socket is not created!"};
    }


    ///
    /// Sends message to the specified client TCP/IP socket (if server socket is opened)
    ///
    error_data SocketManager::send_message_to_client_socket(sockaddr_in const socket_address, std::string_view const message_body) noexcept
    {
        std::shared_lock lock { socket_guard_ };
        return socket_ ? socket_->send_message(socket_address, message_body) : error_data{SERVICE_UNAVAILABLE_ERROR, "TCP/IP socket is not created!"};
    }


    ///
    /// Sends message to the specified  TCP/IP socket
    ///
    /*
    error_data SocketManager::send_message(std::string_view const ip_address, in_port_t const port_number, std::string_view const message_body) noexcept
    {
        error_data rc;
        std::shared_lock lock { socket_guard_ };

        for (auto& socket : sockets_)
        {
            rc = socket->send_message(ip_address, port_number, message_body);
            if (rc.first != NOT_FOUND_ERROR) break;
        }

       return rc;
    }
    */

    ///
    /// Default handler for obtained TCP/IP messages - just print the for std::cout in developer's mode.
    ///
    void SocketManager::process_received_message(sockaddr_in const ip_address, std::string_view const message) noexcept
    {
        NOUT(fmt::format("Message received from '{}:{}': '{}'", ::inet_ntoa(ip_address.sin_addr), ntohs(ip_address.sin_port), message))
    }


    ///
    /// Handler for the case when client serer connection is lost or any other severe error happened.
    /// Try to restart the socket (note: wee need to do it in another thread!)
    ///
    void SocketManager::process_close_socket_(sockaddr_in ip_address) noexcept
    {
        if (!is_closing_.load())
        {
            std::thread restart(&SocketManager::restart_socket, ip_address);
            restart.detach();
        }
    }


    void SocketManager::restart_socket_(sockaddr_in const ip_address) noexcept
    {
        if (!is_closing_.load())
        {
            std::shared_lock lock { socket_guard_ };
            if (socket_)
            {
                auto const address = socket_->get_socket_address();
                if (address.sin_port == ip_address.sin_port && address.sin_addr.s_addr == ip_address.sin_addr.s_addr)
                {
                    socket_->start_socket();
                    NOUT(fmt::format("Restarting socket on '{}:{}'.", ::inet_ntoa(ip_address.sin_addr), ntohs(ip_address.sin_port)))
                }
            }
        }
    }


    ///
    /// Gets IP address and port number from the input JSON data.
    ///
    socket_address SocketManager::get_socket_address(nlohmann::json const& input_data) noexcept
    {
        socket_address rc { std::nullopt, std::nullopt };

        if (input_data.contains(HOST_ADDRESS) && input_data[HOST_ADDRESS].is_string())
        {
            auto host_address = input_data[HOST_ADDRESS].get<std::string>();
            if (!host_address.empty())
            {
                rc.first = std::move(host_address);
            }
        }

        if (input_data.contains(PORT_NUMBER) && !input_data[PORT_NUMBER].is_null()) 
        {
            if (input_data[PORT_NUMBER].is_number_unsigned())
            {
                rc.second = input_data[PORT_NUMBER].get<in_port_t>();
            }
            else if (input_data[PORT_NUMBER].is_string())
            {
                rc.second = tegia::common::try_parse_remove_leading_blanks<in_port_t>(input_data[PORT_NUMBER].get<std::string>());
            }
        }

        return rc;
    }


    ///
    /// Sends all queued message (if any exists) to the server when (if) the connection was [re]established.
    ///
    void SocketManager::send_queued_server_messages() noexcept
    {
        auto message = message_queue_->get_message(""s);
        while (message.has_value())
        {
            if (auto const rc = send_message_to_server_socket(*message); rc.first != SUCCESS)
            {
                 message_queue_->add_message(""s, std::move(*message));
                 break;
            }
            message = message_queue_->get_message(""s);
        }
    }

    ///
    /// Sends all queued message (if any exists) to the specified client when (if) the connection was [re]established.
    ///
    void SocketManager::send_queued_client_messages(std::string&& node_uuid, sockaddr_in socket_address) noexcept
    {
        auto message = message_queue_->get_message(node_uuid);
        while (message.has_value())
        {
            //std::cout << "==>\tSending queued message '" << *message << "' to node uuid = '" << node_uuid << "'" << std::endl;
            if (auto const rc = send_message_to_client_socket(socket_address, *message); rc.first != SUCCESS)
            {
                 message_queue_->add_message(node_uuid, std::move(*message));
                 break;
            }
            message = message_queue_->get_message(node_uuid);
        }
    }



}