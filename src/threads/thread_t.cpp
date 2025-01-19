#include <tegia/core/cast.h>

#include "thread_t.h"
#include "manager_t.h"

namespace tegia {
namespace threads {


//////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
//////////////////////////////////////////////////////////////////////////////////////////////////


thread_t::thread_t(): _mysql_provider(new ::tegia::mysql::provider())
{
	this->_user = std::make_shared<::tegia::user>();
	this->_user->_roles.set(ROLES::SESSION::SYSTEM);
};


//////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
//////////////////////////////////////////////////////////////////////////////////////////////////


::tegia::node::node * const thread_t::node()
{
	return this->_node;
};

//
//
//

::tegia::mysql::provider * const thread_t::mysql_provider()
{
	return this->_mysql_provider;
};

//
//
//

::tegia::actors::map_t * const thread_t::actor_map()
{
	return this->_actor_map;
};

//
//
//

const std::shared_ptr<tegia::user> thread_t::user()
{
    return tegia::threads::thread->_user;
};


//////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
//////////////////////////////////////////////////////////////////////////////////////////////////


std::string thread_t::tid()
{
	return core::cast<std::string>(this->_tid);
};

//
//
//

const nlohmann::json * const thread_t::config(const std::string &name)
{
	return this->_config->get(name);
};

//
//
//

int thread_t::thread(std::function<void()> _fn)
{
	return this->_manager->thread(_fn);
};


//////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
//////////////////////////////////////////////////////////////////////////////////////////////////


thread_local const std::unique_ptr<tegia::threads::thread_t> thread = std::make_unique<tegia::threads::thread_t>();


} // namespace threads
} // namespace tegia