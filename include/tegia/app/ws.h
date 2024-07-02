#ifndef H_WS_BASE
#define H_WS_BASE

#include <tegia/tegia.h>
#include <tegia/app/router.h>
#include <tegia/db/mysql/mysql.h>


namespace tegia {
namespace ws {

class base: public tegia::actors::actor_base
{
	public:

		base(
			const std::string &_name, 
			nlohmann::json data);

		~base();

		//
		// ACTOR ACTIONS
		//
		
		int router(const std::shared_ptr<message_t> &message);

	private:
		tegia::app::router_t * _router = nullptr;
};

}	// END namespace ws
}	// END namespace tegia


#endif