#include "manager_t.h"

namespace tegia {
namespace threads {


//////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
//////////////////////////////////////////////////////////////////////////////////////////////////


void manager_t::init(
	::tegia::node::node     * _node, 
	::tegia::node::config   * _config,
	::tegia::node::logger   * _logger,
	::tegia::actors::map_t  * _actor_map)
{
	this->_node = _node;
	this->_config = _config;
	this->_logger = _logger;
	this->_actor_map = _actor_map;

	auto tid = std::this_thread::get_id();
	tegia::threads::thread->_tid = tid;

	tegia::threads::thread->_config    = _config;
	tegia::threads::thread->_logger    = _logger;
	tegia::threads::thread->_manager   = this;	
	tegia::threads::thread->_node      = _node;
	tegia::threads::thread->_actor_map = _actor_map;
};


//////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
//////////////////////////////////////////////////////////////////////////////////////////////////


int manager_t::thread(std::function<void()> _fn)
{
    std::thread t( 
	[
		config = this->_config,
		logger = this->_logger,
		manager = this,
		node = this->_node,
		actor_map = this->_actor_map,
		_fn
	] () 
	{
		//
		// INIT THREAD
		//

		auto tid = std::this_thread::get_id();

		tegia::threads::thread->_tid       = tid;
		tegia::threads::thread->_config    = config;
		tegia::threads::thread->_logger    = logger;
		tegia::threads::thread->_manager   = manager;
		tegia::threads::thread->_node      = node;
		tegia::threads::thread->_actor_map = actor_map;

		//
		// Соединение с БД
		//
		
		auto dbc = config->get("dbc");

		if(dbc->contains("connections") == true)
		{
			for (auto it = (*dbc)["connections"].begin(); it != (*dbc)["connections"].end(); ++it)
			{
				tegia::threads::thread->_mysql_provider->add_connection(it.key(),it.value());
			}
		}

		if(dbc->contains("databases") == true)
		{
			for (auto it = (*dbc)["databases"].begin(); it != (*dbc)["databases"].end(); ++it)
			{
				tegia::threads::thread->_mysql_provider->add_database(it.key(),it.value());
			}
		}		

		//
		// RUN THREAD
		//

		std::cout << _OK_TEXT_ << "run thread " << tid << std::endl;
        _fn();
    });

    t.detach(); // Позволяет потоку работать независимо
	return 0;
};



} // namespace threads
} // namespace tegia