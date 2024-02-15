#ifndef H_TEGIA_ACTORS_MESSAGE_T
#define H_TEGIA_ACTORS_MESSAGE_T

#include <stack>
#include <mutex>
#include <tegia/core/json.h>
#include <tegia/core/crypt.h>


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


		callback_t& operator= (const callback_t &callback)
		{
			// Проверка на самоприсваивание
			if (this == &callback) return *this;
		
			// Выполняем копирование значений
			this->_pause = callback._pause;
			this->callback = callback.callback;
		
			// Возвращаем текущий объект
			return *this;
		};	


		bool add(const std::string &_actor, const std::string &_action)
		{
			this->callback.push({true,_actor,_action});
			return true;
		};
		

		addr_t get()
		{
			this->_mutex.lock();

			if(this->_pause == true)
			{
				addr_t _callback {false,"", ""};
				this->_mutex.unlock();
				return std::move(_callback);					
			}

			if(this->callback.size() > 0)
			{
				auto _callback = this->callback.top();
				this->callback.pop();		
				this->_mutex.unlock();
				return std::move(_callback);
			}
			else
			{
				addr_t _callback {false,"", ""};
				this->_mutex.unlock();
				return std::move(_callback);
			}

		};

		void pause()
		{
			this->_pause = true;
		};

		void resume()
		{
			this->_pause = false;
		};

	private:
		bool _pause = false;
		std::mutex _mutex;
		std::stack<addr_t> callback{};
};



class message_t
{
	public:
		nlohmann::json data;
		nlohmann::json http;
		callback_t callback;

		message_t(nlohmann::json _data):
			data(_data),
			uuid(tegia::random::uuid()){};

		message_t():
			data(nlohmann::json::object()),
			uuid(tegia::random::uuid()){};

		message_t(message_t const&) = delete;
		message_t(message_t&&) noexcept = delete;
		message_t& operator = (message_t const &) = delete;
		message_t& operator = (message_t&&) noexcept = delete;

		virtual ~message_t() = default;

	private:
		std::string uuid;
};


#endif

