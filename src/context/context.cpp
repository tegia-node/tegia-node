#include <tegia/context/context.h>

#include "../threads/data.h"
#include "../threads/worker.h"

#include "../node/node.h"
#include "../node/logger.h"




// --------------------------------------------------------------------

//
// CONTEXT
//

namespace tegia {

std::string context::tid()
{
	return tegia::threads::data->tid;
};

::tegia::user * const context::user()
{
	return const_cast<::tegia::user * const>(tegia::threads::data->user);
};


const nlohmann::json * const context::config(const std::string &name)
{
	return tegia::node::node::instance()->config(name);
};


void context::log(
	const int level,
	const std::chrono::high_resolution_clock::time_point now,
	const std::string &filename, 
	const std::string &function, 
	const int line,			
	const std::string &message)
{
	tegia::logger::instance()->writer(level,now,filename,function,line,message);
};

}	// END NAMESPACE tegia 



// --------------------------------------------------------------------