#ifndef H_TEGIA_PLATFORM_ACTORS_ADD_TASK_
#define H_TEGIA_PLATFORM_ACTORS_ADD_TASK_

#include <tegia/actors/add_task.h>
#include "../threads/thread_pool.h"

namespace tegia {
namespace threads {

int add_task(std::function<void()> fn, int priority)
{
	tegia::threads::pool::instance()->add_task(fn, priority);
	return 0;
};

}  // namespace threads	
}  // namespace tegia



#endif