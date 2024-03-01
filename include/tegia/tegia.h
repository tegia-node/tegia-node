#ifndef H_TEGIA
#define H_TEGIA


#include <iostream>

#include <tegia/core/const.h>
#include <tegia/core/cast.h>
#include <tegia/core/json.h>
#include <tegia/actors/types.h>
#include <tegia/context/context.h>


/////////////////////////////////////////////////////////////////////////////////////////////////////
/*

	tegia::conf::*

	Доступ к конфигурационным файлам кластера

*/
/////////////////////////////////////////////////////////////////////////////////////////////////////


namespace tegia {
namespace conf {

std::string path(const std::string &name);
const nlohmann::json * const get(const std::string &name);

} // namespace conf
} // namespace tegia


/////////////////////////////////////////////////////////////////////////////////////////////////////
/*

	tegia::message::*

	Создание и отправка сообщений акторам
*/
/////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tegia {
struct message {

static int send(
	const std::string &name, 
	const std::string &action, 
	nlohmann::json data,
	int priority = 0);

static int send(
	const std::string &name, 
	const std::string &action, 
	const std::shared_ptr<message_t> &message,
	int priority = 0);


static inline const std::shared_ptr<message_t> init(nlohmann::json data = nlohmann::json())
{
	std::shared_ptr<message_t> message(new message_t( data ));
	return message;
};

template<class T>
static inline const std::shared_ptr<T> init(nlohmann::json data = nlohmann::json())
{
	std::shared_ptr<T> message(new T( data ));
	return message;
}

}; // struct message
}  // namespace tegia

/*
namespace tegia {
namespace message {

int send(
	const std::string &name, 
	const std::string &action, 
	nlohmann::json data,
	int priority = 0);

int send(
	const std::string &name, 
	const std::string &action, 
	const std::shared_ptr<message_t> &message,
	int priority = 0);


inline const std::shared_ptr<message_t> init(nlohmann::json data = nlohmann::json())
{
	std::shared_ptr<message_t> message(new message_t( data ));
	return message;
};

template<class T>
const std::shared_ptr<T> init(nlohmann::json data = nlohmann::json())
{
	std::shared_ptr<T> message(new T( data ));
	return message;
};

} // namespace message
} // namespace tegia
*/



#endif