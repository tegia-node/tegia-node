#pragma once

#include <netinet/ip.h>
#include <string>
#include <optional>
#include <chrono>

namespace tegia::sockets
{

  	enum class NodeType: int
	{
		STANDALONE = 0,
		SERVER = 1,
		CLIENT = 2
	};


    using namespace std::literals::string_literals;
    using namespace std::literals::chrono_literals;

    using error_data = std::pair<int, std::optional<std::string>>;

    using message_handler_func = void (*) (sockaddr_in, std::string_view);
    using close_handler_func = void (*) (sockaddr_in);
    using connect_handler_func = void (*) (sockaddr_in);

    inline constexpr int INVALID_SOCKET = -1;
    inline constexpr int SUCCESS = 0;

    inline constexpr int BAD_REQUEST_ERROR = 400;
    inline constexpr int NOT_FOUND_ERROR = 404;
    inline constexpr int BAD_FORMAT_ERROR = 415;
    inline constexpr int INTERNAL_SERVER_ERROR = 500;
    inline constexpr int SERVICE_UNAVAILABLE_ERROR = 503;
    inline constexpr int UNKNOWN_ERROR = 520;

    inline constexpr int MAX_CLIENTS = 4;
    inline constexpr int MAX_TRIES = 4;
    inline constexpr __time_t SECONDS_TO_WAIT = 10L;
    inline constexpr std::chrono::seconds SECONDS_TO_REPEAT = 10s;
    inline constexpr std::chrono::seconds WAIT_FOR_ERROR = 1s;
    inline constexpr std::chrono::seconds WAIT_FOR_NEXT_TRY = 5s;
    inline constexpr std::size_t MESSAGE_BUFFER_SIZE = 4096U;
    inline constexpr std::size_t MAXIMAL_BUFFER_SIZE = MESSAGE_BUFFER_SIZE * MESSAGE_BUFFER_SIZE;
    inline constexpr std::size_t SIZE_TO_COMPRESS    = 2U * MESSAGE_BUFFER_SIZE;
    
    inline constexpr char TEGIA_SIGNATURE[] = "TEG_SIG";
    inline constexpr char NO_COMPRESSION    = '#';
    inline constexpr char YES_COMPRESSION   = '$';

    inline std::string const SOCKET_SECTION = "sockets"s;
    inline std::string const SERVER_SECTION = "server"s;
    inline std::string const CLIENT_SECTION = "clients"s;
    inline std::string const HOST_ADDRESS   = "host"s;
    inline std::string const PORT_NUMBER    = "port"s;

}