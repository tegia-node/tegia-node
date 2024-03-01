#ifndef H_TEGIA_THREADS_DATA
#define H_TEGIA_THREADS_DATA
// --------------------------------------------------------------------

#include <string>
#include <iostream>
#include <thread>

#include <tegia/core/cast.h>
#include <tegia/context/user.h>

#include "../db/mysql/provider.h"


class message_t;

namespace tegia {
namespace node {

class node;

}
}

namespace tegia 
{
	class auth;
	class context;
}

namespace tegia {
namespace threads {

/*
	DOCUMENTATION

	Класс _data реализует инкапсуляцию данных в контексте потока 

*/


class _data
{
	friend class ::tegia::auth;
	friend class ::message_t;
	friend class ::tegia::context;
	friend class ::tegia::node::node;

	public:
		_data();
		~_data();

		::tegia::mysql::provider * const mysql_provider;
		::tegia::node::node * const node();

		// tegia::mysql::records * query(const std::string &context, const std::string &query, bool trace = false);
		// std::string mysql_strip(const std::string &input);
	
	private:

		void init(const nlohmann::json &config);

		::tegia::node::node * _node;
		std::string tid;
		std::shared_ptr<::tegia::user> user;
		
};

extern thread_local tegia::threads::_data * const data;

}	// END NAMESPACE threads
}	// END NAMESPACE tegia 




// --------------------------------------------------------------------
#endif