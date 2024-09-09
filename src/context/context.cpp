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

/*
::tegia::user * const context::user()
{
	return const_cast<::tegia::user * const>(tegia::threads::data->user.get());
};
*/

const std::shared_ptr<tegia::user> context::user()
{
    return tegia::threads::data->user;
};


/*
void context::user(const std::shared_ptr<message_t> &message)
{
	tegia::threads::data->user = message->user;	
};
*/

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