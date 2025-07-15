#ifndef H_TEGIA_TASKS_BUCKET_
#define H_TEGIA_TASKS_BUCKET_

#include <tegia/tegia.h>

//
//
//

namespace tegia {
namespace node {

class node;

}  // END namespace node
}  // END namespace tegia

//
//
//

namespace tegia {
namespace actors {

class worker_t;

}  // END namespace actors
}  // END namespace tegia

//
//
//

namespace tegia {
namespace tasks {

class bucket_t
{
	friend class tegia::actors::worker_t;
	friend class tegia::node::node;

	private:
		bucket_t();
		~bucket_t();

		int add();

}; // END class bucket_t

}  // END namespace tasks
}  // END namespace tegia

#endif