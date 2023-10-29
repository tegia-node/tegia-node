#ifndef H_TEGIA_PLATFORM_ACTORS_MESSAGE_T
#define H_TEGIA_PLATFORM_ACTORS_MESSAGE_T

#include <mutex>
#include <stack>

#include <tegia/core/crypt.h>
#include <tegia/core/json.h>


struct _addr
{
	std::string actor;
	std::string action;
};


class HTTP_REQUEST_DATA
{
	friend class A2FCGI;
	friend class A2Session;
	friend class A2Sessions;

	public:

		std::string request_method{};
		std::string script_name{};
		std::string content_type{};
		std::string content_length{};
		std::string redirect_status{};

		std::string server_software{};
		std::string server_name{};
		std::string gateway_interface{};
		std::string server_protocol{};
		std::string server_port{};

		std::string remote_addr{};
		std::string http_accept{};
		std::string http_user_agent{};
		std::string http_referer{};
		std::string http_origin{};
		std::string http_authorization{};

		nlohmann::json post = nlohmann::json();
		nlohmann::json query_param = nlohmann::json();

		nlohmann::json json() const noexcept;

		void from_json(nlohmann::json& json_data) noexcept;

		std::string get_cookie(const std::string &name)
		{
			return {};	
		};

	private:

		std::map<std::string,std::string> cookie{};
};

struct HTTP
{
	HTTP_REQUEST_DATA request;
};

class A2FCGI;

class message_t
{
	friend class A2FCGI;

	protected:
		std::mutex callback_mutex{};
		void* request = nullptr;
		bool is_local_ = true;

		int priority = 2;

	public:
		
		nlohmann::json data = nlohmann::json::object();

		nlohmann::json pipeline = nlohmann::json{};

		std::stack<_addr> callback{};
		HTTP* http = nullptr;

		message_t() = default;

		message_t(message_t const&) = delete;
		message_t(message_t&&) noexcept = delete;
		message_t& operator = (message_t const &) = delete;
		message_t& operator = (message_t&&) noexcept = delete;

		message_t(nlohmann::json json_data) : data(std::move(json_data)) {}

		virtual ~message_t()
		{
			if(this->http != nullptr)
			{
				delete this->http;
				http = nullptr;
			}
		};

		/*
		std::string& getUUID()
		{
			return this->guid; 
		};
		*/

		/*
		_addr get_pipeline()
		{

		};
		*/

		_addr get_callback()
		{
			this->callback_mutex.lock();
			
			if(this->callback.size() > 0)
			{
				auto cb = this->callback.top();
				this->callback.pop();		
				this->callback_mutex.unlock();
				return cb;
			}
			else
			{
				_addr cb {"", ""};
				this->callback_mutex.unlock();
				return cb;
			}
		};


		nlohmann::json serialize() noexcept;
		void deserialize(nlohmann::json& json_data) noexcept;
		bool is_local() const noexcept { return is_local_; };

		void set_priority(int _priority)
		{
			this->priority = _priority;
		};

		void priority_up()
		{
			this->priority = this->priority + 1;
		};

		int get_priority()
		{
			return this->priority;
		};

};



#endif