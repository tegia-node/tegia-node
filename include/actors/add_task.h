#ifndef H_TEGIA_PLATFORM_ACTORS_ADD_TASK_
#define H_TEGIA_PLATFORM_ACTORS_ADD_TASK_

#include <functional>

namespace tegia {
namespace threads {

	int add_task(std::function<void()> fn, int priority);

}	
}



#endif