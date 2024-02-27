#include <tegia/actors/message_t.h>



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



bool callback_t::add(const std::string &_actor, const std::string &_action)
{
	this->callback.push({true,_actor,_action});
	return true;
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
