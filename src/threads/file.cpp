
#include <tegia/context/thread_context.h>


namespace tegia {
namespace threads {

extern thread_local int count;
extern thread_local context_ptr thread_context;

}
}