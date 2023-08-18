#include <tegia/sockets/AbstractSocket.h>
#include <tegia/core/string.h>

namespace tegia::sockets
{

    ////////////////////////////////////////////////////////////////////////////////////////////
    //                                                                                        //
    // CONSTRUCTOR / DESTRUCTOR                                                               //
    //                                                                                        //
    ////////////////////////////////////////////////////////////////////////////////////////////

    AbstractSocket::AbstractSocket() 
    {
        if (void* const new_buffer = std::malloc(MESSAGE_BUFFER_SIZE); new_buffer != nullptr)
        {
            read_buffer = new_buffer;
            read_buffer_size = MESSAGE_BUFFER_SIZE;
        } 
        if (void* const new_buffer = std::malloc(MESSAGE_BUFFER_SIZE); new_buffer != nullptr)
        {
            send_buffer = new_buffer;
            send_buffer_size = MESSAGE_BUFFER_SIZE;
        }
    }

    AbstractSocket::~AbstractSocket() noexcept
    {
        stop_listener();
        std::scoped_lock lock { socket_guard_ };
        std::free(send_buffer); 
        send_buffer = nullptr;
        send_buffer_size = 0U;
        std::free(read_buffer); 
        read_buffer = nullptr;
        read_buffer_size = 0U;
        std::memset(reinterpret_cast<void*>(&socket_address_), 0, sizeof socket_address_);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////
    //                                                                                        //
    // SUPPORT FUNCTIONS                                                                      //
    //                                                                                        //
    ////////////////////////////////////////////////////////////////////////////////////////////

    ///
    /// Fills-in the data structure containing IP Address and port number for socket connection.
    ///
    void AbstractSocket::set_ip_address(std::string_view const ip_address, in_port_t const port_number) noexcept
    {
        std::memset(reinterpret_cast<void*>(&socket_address_), 0, sizeof socket_address_);
        socket_address_.sin_family = AF_INET;
        socket_address_.sin_addr.s_addr = ::htonl(INADDR_LOOPBACK);
        ::inet_aton(ip_address.data(), &socket_address_.sin_addr);
        socket_address_.sin_port = htons(port_number);
    }


    ///
    /// Reads first 'N' bytes from the socket stream
    ///
    ssize_t AbstractSocket::read_n_bytes(int const socket_id, void* const buf, std::size_t const N, int const flags) const
    {
        std::size_t offset = 0U;
        while (is_connected()) 
        {
            ssize_t const rc = ::recv(socket_id, reinterpret_cast<void*>(reinterpret_cast<char*>(buf) + offset), N - offset, flags);
            if (rc < 0L) 
            {
                // Error occurred
                if (errno != EINTR && errno != EWOULDBLOCK)
                {
                    NOUT(fmt::format("Error #{} happened while receiving TCP/IP message: {}.", errno, ::strerror(errno)))
                    return rc;
                }
            } 
            else if (rc == 0L)
            {
                return -1;
            }
            else
            {
                offset += rc;
                //NOUT(fmt::format("\t{} bytes of {} were read.", offset, N))
                // All N bytes read
                if (offset >= N) break;
                // Continue reading
            }
        }
        return static_cast<ssize_t>(offset);
    }


    ///
    /// Sends message to the TCP/IP sever socket to which the client is connected.
    ///
    error_data AbstractSocket::send_message(std::string_view const ip_address, in_port_t const port_number, std::string_view const message_body) noexcept
    {
        sockaddr_in socket_address{};

        socket_address.sin_family = AF_INET;
        socket_address.sin_addr.s_addr = ::htonl(INADDR_LOOPBACK);
        ::inet_aton(ip_address.data(), &socket_address.sin_addr);
        socket_address.sin_port = htons(port_number);

        return send_message(socket_address, message_body);
    }


    ///
    /// Sends message to the TCP/IP sever socket to which the client is connected.
    ///
    error_data AbstractSocket::send_message(int const socket_id, sockaddr_in const ip_address, std::string_view message_body) noexcept
    {
        //
        // Check if socket is already initialized and running:
        //
        if (!is_connected())
        {
            return std::make_pair(BAD_REQUEST_ERROR, std::make_optional(std::string("Cannot send TCP/IP message: socket is not connected.")));
        }

        auto compress_signature = NO_COMPRESSION;
        std::string compressed_message{};
        std::size_t message_size = message_body.size();
        if (message_size >= SIZE_TO_COMPRESS)
        {
           compressed_message = core::string::compress_string(message_body, Z_DEFAULT_COMPRESSION);  
           message_body = std::string_view{compressed_message};
           NOUT("Message was compressed with ratio = " << double(message_size) / double(message_body.size()))
           message_size = message_body.size();
           compress_signature = YES_COMPRESSION;
        }

        // 
        // Calculate the size of TCP/IP message to send:
        //
        constexpr std::size_t sig_len = sizeof(TEGIA_SIGNATURE);
        constexpr std::size_t siz_len = sizeof(std::uint64_t);
        std::uint64_t const  msg_len = static_cast<std::uint64_t>(message_size);
        message_size = sig_len + siz_len + msg_len;

        try
        {
            //
            // Note! We need exclusive access to the buffer.
            //
            std::scoped_lock lock { socket_guard_ };

            // 
            // Allocate the new buffer of required size or use existing if it's enough:
            //
            if (message_size > send_buffer_size)
            {
                //
                //  Don't allow to allocate memory exceeding 'MAXIMAL_BUFFER_SIZE'!
                //
                if (message_size <= MAXIMAL_BUFFER_SIZE)
                {  
                    auto const buffer_size = (message_size / MESSAGE_BUFFER_SIZE + 1L) * MESSAGE_BUFFER_SIZE;
                    if (void* const new_buffer = std::realloc(send_buffer, buffer_size); new_buffer != nullptr)
                    {
                        send_buffer = new_buffer;
                        send_buffer_size = buffer_size;
                    } 
                }

                if (message_size > send_buffer_size)
                {
                    return std::make_pair(ENOMEM, std::make_optional(std::string("Cannot send TCP/IP message: not enough memory is available.")));
                }
            }

            // 
            // Create the correct Tegia TCP/IP message - with the header containing signature and body size:
            //
            ::memcpy(send_buffer, reinterpret_cast<const void*>(TEGIA_SIGNATURE), sig_len - 1L);
            reinterpret_cast<char*>(send_buffer)[sig_len - 1L] = compress_signature;
            ::memcpy(reinterpret_cast<void*>(reinterpret_cast<char*>(send_buffer) + sig_len), reinterpret_cast<const void*>(&msg_len), siz_len);
            ::memcpy(reinterpret_cast<void*>(reinterpret_cast<char*>(send_buffer) + sig_len + siz_len), reinterpret_cast<const void*>(message_body.data()), msg_len);
            compressed_message.clear();
            
            //
            //  Send all message data:
            //
            NOUT(fmt::format("==> Sending TCP/IP message to '{}:{}'.", ::inet_ntoa(ip_address.sin_addr), htons(ip_address.sin_port)))
            ssize_t total_len = 0L;
            while (static_cast<std::size_t>(total_len) < message_size)
            {
                ssize_t const bytes_sent = 
                    ::send(socket_id, reinterpret_cast<void*>(reinterpret_cast<char*>(send_buffer) + total_len), message_size - total_len, MSG_DONTWAIT);
                if (bytes_sent == -1)
                {
                    NOUT(fmt::format("Error #{} happened while sending TCP/IP message to '{}:{}': {}.", 
                            errno, ::inet_ntoa(ip_address.sin_addr), htons(ip_address.sin_port), ::strerror(errno)))
                    if (errno != EINTR && errno != EWOULDBLOCK) 
                    {
                         return std::make_pair(errno, std::make_optional(fmt::format("Error #{} happened while sending TCP/IP message to '{}:{}': {}.", 
                            errno, ::inet_ntoa(ip_address.sin_addr), htons(ip_address.sin_port), ::strerror(errno))));
                    }
                }
                else
                {
                    total_len += bytes_sent;
                    //NOUT(fmt::format("\t{} bytes of {} were sent.", total_len, message_size))
                }
            }
        }
        catch (const std::exception& e)
        {
            return std::make_pair(INTERNAL_SERVER_ERROR, std::make_optional(fmt::format("Error happened while sending TCP/IP message to '{}:{}': {}.", 
                ::inet_ntoa(ip_address.sin_addr), htons(ip_address.sin_port), e.what())));
        }
    
        return std::make_pair(SUCCESS, 
            std::make_optional(fmt::format("TCP/IP message was sent to '{}:{}'.", ::inet_ntoa(ip_address.sin_addr), htons(ip_address.sin_port))));
    }


    ///
    /// Stops socket listening and close file descriptor(s)
    //
    void AbstractSocket::close_socket()
    {
        if (is_connected() || is_connecting()) 
        {
            stop_listener();
        }
    }

    ///
    /// Sets flag to stop socket data listening and waits until thread terminates.
    ///
    void AbstractSocket::stop_listener()
    {
        is_connecting_.store(false);
        is_connected_.store(false);

        if (listener_)
        {
            if (listener_->joinable()) listener_->join();
            listener_.reset(nullptr);
        }
    }


    ///
    /// Starts listener thread for server messages and set the flag.
    ///
    bool AbstractSocket::start_listener() 
    {
        stop_listener();
        is_connecting_.store(true);
        listener_ = std::make_unique<std::thread>(&AbstractSocket::event_loop, this);
        return static_cast<bool>(listener_);
    }


    ///
    ///  Creates and initializes TCP/IP socket and start listening.
    ///
    bool AbstractSocket::start_socket(std::string_view const ip_address, in_port_t const port_number)
    {
        //
        // Check if socket is already initialized and running:
        //
        if (is_connected() || is_connecting())
        {
            //
            // Convert server address data to the network format:
            //
            in_addr address{ ::htonl(INADDR_LOOPBACK) };
            ::inet_aton(ip_address.data(), &address);
            in_port_t const port = htons(port_number);

            //
            //  If it has the same IP address and port then it's nothing to do, otherwise it cannot be done! :)
            //
            return address.s_addr == socket_address_.sin_addr.s_addr && port == socket_address_.sin_port;
        }

        set_ip_address(ip_address, port_number);
        return start_socket();
    }


    ///
    ///  Creates and initializes TCP/IP socket and start listening.
    ///
    bool AbstractSocket::start_socket()
    {
        return is_connected() || is_connecting() || start_listener();
    }


    ///
    /// Waits until any event happens on any socket and process it:
    ///
    void AbstractSocket::event_loop() 
    {
        //
        //  Create and initialize the socket:
        //
        while (is_connecting())
        {
            if (initialize_socket())
            {
                auto const str = fmt::format("TCP/IP socket is created on '{}:{}'.", ::inet_ntoa(socket_address_.sin_addr), ntohs(socket_address_.sin_port));
                LNOTICE(str)
                NOUT(str)
                is_connecting_.store(false);
                is_connected_.store(true);
                break;
            }
            std::this_thread::sleep_for(SECONDS_TO_REPEAT);
        }


        while (is_connected())
        {
            fd_set sockets{};
            if (int const  events_number = wait_for_events(sockets); events_number > 0)
            {
                //
                // Obtained event(s) from the socket(s). Process them.
                //
                if (!process_events(events_number, sockets))
                {
                    //
                    //  Error happened - close connection.
                    //
                    is_connected_.store(false);
                }
            }
            else if (events_number < 0)
            {
                //
                //  Error happened - close connection.
                //
                is_connected_.store(false);
            }
        }

        close_socket_descriptors(); 
        if (auto const close_handler = process_close_.load(); close_handler != nullptr) 
        {
            close_handler(socket_address_);
        }
    }


    ///
    /// Closes socket file descriptor if required:
    ///
    void AbstractSocket::close_socket_descriptors()
    {
        std::scoped_lock lock { socket_guard_ };

        if (socket_id_ != INVALID_SOCKET) 
        {
            if (::close(socket_id_) != SUCCESS)
            {
                auto const str = fmt::format("Error happened while closing socket: {}.", strerror(errno));
                LWARNING(str)
                EOUT(str)
            }
            else
            {
                auto const str = fmt::format("TCP/IP socket on '{}:{}' is closed.", ::inet_ntoa(socket_address_.sin_addr), ntohs(socket_address_.sin_port));
                LNOTICE(str)
                NOUT(str)
            }
            socket_id_ = INVALID_SOCKET;
        }
    }


    //
    // Waits for events on the socket(s): new client connected and/or data obtained to read.
    //
    int AbstractSocket::wait_for_events(fd_set& sockets)  
    {
        timeval timeout{};
        timeout.tv_sec = SECONDS_TO_WAIT;
        timeout.tv_usec = 0L;

        int const max_id = fill_in_fd(sockets);

        if (!is_connected()) return 0;

        auto const rc = ::select(max_id + 1, &sockets, nullptr, nullptr, &timeout);
        
        if (rc == -1)
        {
            auto const str = fmt::format("Error happened while waiting data on the socket with id = {}: {}.", socket_id_.load(), ::strerror(errno));
            LERROR(str)
            EOUT(str)
        }
#if defined (DEVELOPER_VERSION)        
        else if (rc == 0)
        {
            NOUT(fmt::format("Timeout happened while waiting data on the socket with id = {}.", socket_id_.load()));
        }
#endif        
        
        return rc;
    }


    ///
    /// Reads message data from the client socket, parses them and perform the required action:
    ///
    bool AbstractSocket::read_data(int const socket_id, sockaddr_in const ip_address, int const flags) noexcept
    {
        constexpr auto const sign_len = sizeof TEGIA_SIGNATURE;
        char signature[sign_len]{};

        try
        {
            NOUT(fmt::format("\t==> Reading TCP/IP message from '{}:{}'.", ::inet_ntoa(ip_address.sin_addr), htons(ip_address.sin_port)))
            //
            //  Get first 'sig_len' bytes to read message signature:
            //
            auto len = read_n_bytes(socket_id, reinterpret_cast<void*>(signature), sign_len, flags);
            if (len > 0L)
            {
                if (static_cast<std::size_t>(len) == sign_len)
                {
                    //
                    //  Check that is the correct TEGIA message format
                    //  
                    constexpr auto const sign_len1 = sign_len - 1L;
                    if (tegia::common::equal_ic(TEGIA_SIGNATURE, std::string_view(signature, sign_len1)) && 
                        (signature[sign_len1] == NO_COMPRESSION || signature[sign_len1] == YES_COMPRESSION))
                    {
                        //
                        //  Get the length of message body
                        //
                        std::uint64_t msg_len{};
                        len = read_n_bytes(socket_id, reinterpret_cast<void*>(&msg_len), sizeof msg_len, flags);
                        if (static_cast<std::size_t>(len) == sizeof(msg_len))
                        {
                            // 
                            // Allocate the required buffer (we don't need lock here because it's always used only from a single listener thread!):
                            //
                            if (msg_len > read_buffer_size)
                            {
                                if (msg_len <= MAXIMAL_BUFFER_SIZE)
                                {
                                    auto const buffer_size = (msg_len / MESSAGE_BUFFER_SIZE + 1L) * MESSAGE_BUFFER_SIZE;
                                    if (void* const new_buffer = ::realloc(read_buffer, buffer_size); new_buffer != nullptr)
                                    {
                                        read_buffer = new_buffer;
                                        read_buffer_size = buffer_size;
                                    }
                                } 
                                if (msg_len > read_buffer_size)
                                {
                                    do
                                    {
                                        len = ::recv(socket_id, read_buffer, read_buffer_size, MSG_DONTWAIT);
                                    } 
                                    while (static_cast<std::size_t>(len) == read_buffer_size);
                                    auto const str = fmt::format("Cannot read TCP/IP message from the socket on '{}:{}'. Not enough memory is available.", 
                                        ::inet_ntoa(ip_address.sin_addr), ntohs(ip_address.sin_port));
                                    LWARNING(str)
                                    EOUT(str)
                                    NOUT(std::string_view(reinterpret_cast<char*>(read_buffer), len))
                                    return true;
                                }
                            }

                            // 
                            // Read the message itself:
                            //
                            len = read_n_bytes(socket_id, read_buffer, msg_len, flags);
                            if (static_cast<std::size_t>(len) != msg_len)
                            {
                                auto const str = fmt::format("Cannot read the entire TCP/IP Tegia message from the socket on '{}:{}'. Ignored.", 
                                    ::inet_ntoa(ip_address.sin_addr), ntohs(ip_address.sin_port));
                                LWARNING(str)
                                NOUT(std::string_view(reinterpret_cast<char*>(read_buffer), len))
                            }
                            else
                            {
                                auto message_body = std::string_view(reinterpret_cast<char*>(read_buffer), msg_len);
                                std::string decompressed_message{};
                                if (signature[sign_len1] == YES_COMPRESSION)
                                {
                                    decompressed_message = core::string::decompress_string(message_body);
                                    message_body = std::string_view(decompressed_message);
                                }

                                //
                                // Deserialize the message and perform further actions if registered
                                //
                                if (auto const message_handler = process_message_.load(); message_handler != nullptr)
                                {
                                    message_handler(ip_address, message_body);
                                }
#if defined (DEVELOPER_VERSION)                
                                else
                                {
                                    NOUT("Received message: " << message_body)
                                }                                
#endif                
                            }
                        }
                        else
                        {
                            auto const str = fmt::format("Invalid format of TCP/IP Tegia message was obtained from the socket on '{}:{}'. Ignored.", 
                                ::inet_ntoa(ip_address.sin_addr), ntohs(ip_address.sin_port));
                            LWARNING(str)
                            EOUT(str)
                        }
                    }
                    else
                    {
                        //
                        // Just read unknown message to remove data from socket buffer.
                        //
                        do
                        {
                            len = ::recv(socket_id, read_buffer, read_buffer_size, MSG_DONTWAIT);
                        } 
                        while (static_cast<std::size_t>(len) == read_buffer_size);
                        NOUT("Unknown message was obtained from the socket on '" << ::inet_ntoa(ip_address.sin_addr) << ':' 
                            << ntohs(ip_address.sin_port) << "': " << std::string_view(signature, sign_len) 
                            << (len > 0L ? std::string_view(reinterpret_cast<char*>(read_buffer), len) : std::string_view(""))) 
                    }
                }
#if defined (DEVELOPER_VERSION)                
                else
                {
                    NOUT("Unknown message was obtained from the socket on '" << ::inet_ntoa(ip_address.sin_addr) << ':' 
                        << ntohs(ip_address.sin_port) << "': " << std::string_view(signature, len)) 
                }
#endif                
                return true;
            }
        }
        catch (const std::exception& e)
        {
            auto const str = fmt::format("Error happened while reading data from the socket on '{}:{}'. {}.",
                ::inet_ntoa(ip_address.sin_addr), ntohs(ip_address.sin_port), e.what());
            LERROR(str)
            EOUT(str)
        }   

        return false;
    }

}