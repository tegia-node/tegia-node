#ifndef H_TEGIA_THREADS_THREAD
#define H_TEGIA_THREADS_THREAD

#include <thread>
#include <tegia/context/user.h>

#include "../db/mysql/provider.h"
#include "../node/config.h"


namespace tegia::node 
{
	class node;
}

namespace tegia::actors 
{
	class map_t;
}

namespace tegia 
{
	struct message;
	class auth;
	class context;
}

class message_t;

//////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
//////////////////////////////////////////////////////////////////////////////////////////////////


namespace tegia {
namespace threads {

class manager_t;

class thread_t
{
	friend class ::tegia::context; // TODO
	friend class ::tegia::auth;
	friend class ::message_t;
	friend class ::tegia::actors::map_t;
	friend class ::tegia::threads::manager_t;
	friend struct ::tegia::message;

	public:
		thread_t();
		~thread_t() = default;

		::tegia::node::node * const node();
		::tegia::mysql::provider * const mysql_provider();
		::tegia::actors::map_t * const actor_map();
		const std::shared_ptr<tegia::user> user();

		std::string tid();

		const nlohmann::json * const config(const std::string &name);
		int thread(std::function<void()> _fn);

	private:
		std::thread::id _tid;
		::tegia::node::node          * _node;
		::tegia::node::config        * _config;
		::tegia::threads::manager_t  * _manager;
		::tegia::actors::map_t       * _actor_map;
		::tegia::mysql::provider     * _mysql_provider;
		std::shared_ptr<::tegia::user> _user;
};

extern thread_local const std::unique_ptr<tegia::threads::thread_t> thread;

} // END namespace threads
} // END namespace tegia

#endif