#ifndef H_TEGIA_NODE
#define H_TEGIA_NODE
// ---------------------------------------------------------------------------------------------------

#include <iostream>
#include <tegia2/core/json.h>

namespace tegia {
namespace threads {
	class pool;
}
}

namespace TEGIA{
class NODE
{
	public:
		NODE();
		~NODE();
		bool run();
		bool action();

		const nlohmann::json * const config();

	private:
		tegia::threads::pool * threads;

		nlohmann::json conf = nlohmann::json::object();

};	// END class NODE
}	// EMD namespace TEGIA


// ---------------------------------------------------------------------------------------------------
#endif