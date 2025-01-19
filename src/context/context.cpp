#include <tegia/context/context.h>

#include "../threads/thread_t.h"
#include "../node/node.h"
#include "../node/logger.h"




// --------------------------------------------------------------------

//
// CONTEXT
//

namespace tegia {

std::string context::tid()
{
	return tegia::threads::thread->tid();
};

//
//
//

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