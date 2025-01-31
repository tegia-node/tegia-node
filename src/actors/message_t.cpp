#include "../threads/thread_t.h"

#include <tegia/tegia.h>
#include <tegia/actors/message_t.h>


/////////////////////////////////////////////////////////////////////////////////////
/*
	callback_t
*/
/////////////////////////////////////////////////////////////////////////////////////


callback_t& callback_t::operator= (const callback_t &callback)
{
	// Проверка на самоприсваивание
	if (this == &callback) return *this;

	// Выполняем копирование значений
	this->_pause = callback._pause;
	this->callback = callback.callback;

	// Возвращаем текущий объект
	return *this;
};



int callback_t::add(const std::string &_actor, const std::string &_action)
{
	this->callback.push({true,_actor,_action});
	return 200;
};


addr_t callback_t::get()
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



/////////////////////////////////////////////////////////////////////////////////////
/*
	message_t
*/
/////////////////////////////////////////////////////////////////////////////////////


message_t::message_t(nlohmann::json _data):
	data(_data),
	uuid(tegia::random::uuid()),
	user(tegia::threads::thread->_user)
{};


message_t::message_t():
	data(nlohmann::json::object()),
	uuid(tegia::random::uuid()),
	user(tegia::threads::thread->_user)
{};



void message_t::print_user()
{
	this->user->print();
};


std::string message_t::user_id()
{
	return this->user->uuid();
};