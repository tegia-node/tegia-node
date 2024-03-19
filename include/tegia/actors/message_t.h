#ifndef H_TEGIA_ACTORS_MESSAGE_T
#define H_TEGIA_ACTORS_MESSAGE_T

#include <stack>
#include <mutex>
#include <tegia/core/json.h>
#include <tegia/core/crypt.h>
#include <tegia/context/user.h>


struct addr_t
{
	bool is_addr = true;
	std::string actor;
	std::string action;
};


class callback_t
{
	public:
		callback_t() = default;
		~callback_t() = default;

		callback_t& operator= (const callback_t &callback);	
		int add(const std::string &_actor, const std::string &_action);
		addr_t get();

		inline void pause()
		{
			this->_pause = true;
		};

		inline void resume()
		{
			this->_pause = false;
		};

	private:
		bool _pause = false;
		std::mutex _mutex;
		std::stack<addr_t> callback{};
};


namespace tegia {
	struct message;
}

class message_t
{
	public:
		nlohmann::json data;
		nlohmann::json http;
		callback_t callback;

		message_t();
		message_t(nlohmann::json _data);

		message_t(message_t const&) = delete;
		message_t(message_t&&) noexcept = delete;
		message_t& operator = (message_t const &) = delete;
		message_t& operator = (message_t&&) noexcept = delete;

		virtual ~message_t() = default;

		void print_user();

	private:
		std::string uuid;
		std::shared_ptr<::tegia::user> user;
};


#endif

