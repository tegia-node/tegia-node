#ifndef H_TEGIA_ACTOR_WS_SYSTEM
#define H_TEGIA_ACTOR_WS_SYSTEM

#include <tegia/tegia.h>

using namespace std::chrono_literals;

namespace tegia {
namespace actors {

class ws_t;

}  // END namespace actors
}  // END namespace tegia


//
//
//

namespace tegia {
namespace ws {


struct member_t
{
	int status = 0;
	std::string uuid;
	unsigned long long int roles;
	std::string key;
	std::string t_add;
};


}  // END namespace ws
}  // END namespace tegia

//
//
//

namespace tegia {
namespace ws {

class system_t
{
	friend class tegia::actors::ws_t;

	private:
		system_t(const std::string &connection, const std::string &table, const std::string &name);
		~system_t();

		//
		// functions
		//

		int init(nlohmann::json &data);
		int commit();
		std::string dump();

		//
		// data
		//

		int version = 1;
		std::string connection;
		std::string table;
		std::string name;

		std::string title;
		std::string prefix;
		std::string wsid;

		std::unordered_map<std::string,member_t> _members;

		std::mutex _mutex;
};


}  // END namespace ws
}  // END namespace tegia




#endif
