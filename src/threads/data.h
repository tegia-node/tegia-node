#ifndef H_TEGIA_THREADS_DATA
#define H_TEGIA_THREADS_DATA
// --------------------------------------------------------------------

#include <string>
#include <iostream>
#include <thread>

#include <tegia/core/cast.h>
#include <tegia2/context/user.h>

#include "../db/mysql/provider.h"

namespace tegia 
{
	class context;
}

namespace tegia {
namespace threads {

class pool;
class worker;

class _data
{
	friend class ::tegia::context;
	friend class ::tegia::threads::pool;
	friend class ::tegia::threads::worker;

	public:
		_data();
		~_data();

		tegia::mysql::records * query(const std::string &name,const std::string &query, bool trace = false);
	
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