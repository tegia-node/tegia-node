#ifndef H_TEGIA_CONTEXT2
#define H_TEGIA_CONTEXT2
// --------------------------------------------------------------------

#include <thread>
#include <tegia/core/cast.h>

namespace tegia{
class context2 {

	public:
		context2()
		{
			this->_tid = std::this_thread::get_id();
		};

		~context2(){};

		std::string tid()
		{
			return core::cast<std::string>(this->_tid);
		};

	private:
		int test = 0;

		std::thread::id _tid;



};	// END class context2
}	// END namespace tegia


// --------------------------------------------------------------------
#endif