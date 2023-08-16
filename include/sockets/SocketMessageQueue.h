#pragma once

#include <mutex>
#include <deque>
#include <unordered_map>
#include <string>
#include <optional>

namespace tegia::sockets
{
    class SocketMessageQueue final
    {
        public: 

            SocketMessageQueue() = default;
            ~SocketMessageQueue() noexcept { clear(); }

            SocketMessageQueue(SocketMessageQueue const& other) = delete;
            SocketMessageQueue(SocketMessageQueue&& other) noexcept = delete;
            SocketMessageQueue& operator = (SocketMessageQueue const& other) = delete;
            SocketMessageQueue& operator = (SocketMessageQueue& other) noexcept = delete;

            template <typename T>
            void add_message(std::string const& node_uuid, T&& message)
            {
                static_assert(std::is_constructible_v<std::string, T>, "Cannot create string from the specified argument.");
                std::string const& key = !node_uuid.empty() ? node_uuid : NO_UUID;
                std::scoped_lock lock { message_guards_[key] };
                socket_messages_[key].emplace_back(std::forward<T>(message));
                if (socket_messages_[key].size() >= MAX_MESSAGES) socket_messages_[key].pop_front();
			    //std::cout << "==>\tAdded message '" << socket_messages_[key].back() << "' to the UUID = '" << key << "'" << std::endl;
            }

            std::optional<std::string> get_message(std::string const& node_uuid)
            {
                std::optional<std::string> rc = std::nullopt;
                std::string const& key = !node_uuid.empty() ? node_uuid : NO_UUID;
                std::scoped_lock lock { message_guards_[key] };
                if (!socket_messages_[key].empty())
                {
                    rc = std::move(socket_messages_[key].front());
                    socket_messages_[key].pop_front();
                }
			    return rc;
            }

        private:

            static inline std::string const NO_UUID { "no_uuid" }; 
            static inline constexpr std::size_t const MAX_MESSAGES = 16UL * 1024UL; 

            std::unordered_map<std::string, std::deque<std::string>> socket_messages_{};
            std::unordered_map<std::string, std::mutex> message_guards_{};

            void clear() noexcept
            {
                for (auto& [uuid, guard] : message_guards_)
                {
                    std::scoped_lock lock { guard };
                    socket_messages_[uuid].clear();
                    socket_messages_.erase(uuid);
                }
                message_guards_.clear();
            }
    };
}