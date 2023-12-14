#ifndef H_TEGIA_THREADS_DATA
#define H_TEGIA_THREADS_DATA
// --------------------------------------------------------------------

#include <string>
#include <iostream>
#include <thread>

#include <tegia2/core/cast.h>
#include <tegia2/context/user.h>

#include "../db/mysql/provider.h"


namespace tegia {
namespace node {

class node;

}
}

namespace tegia 
{
	class context;
}

namespace tegia {
namespace threads {

class pool;
class worker;


/*
	DOCUMENTATION

	Класс _data реализует инкапсуляцию данных в контексте потока 

*/


class _data
{
	friend class ::tegia::context;
	friend class ::tegia::threads::pool;
	friend class ::tegia::threads::worker;
	friend class ::tegia::node::node;

	public:
		_data();
		~_data();

		tegia::mysql::records * query(const std::string &context, const std::string &query, bool trace = false);
		std::string mysql_strip(const std::string &input);
	
	private:

		void init(const nlohmann::json &config);

		std::string tid;
		::tegia::user *user;
		::tegia::mysql::provider *mysql_provider;
};

extern thread_local tegia::threads::_data * const data;

}	// END NAMESPACE threads
}	// END NAMESPACE tegia 




// --------------------------------------------------------------------
#endif