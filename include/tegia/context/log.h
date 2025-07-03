#include <tegia/core.h>
#include <tegia/core/error.h>

#undef __LOCATION__
#undef LDEBUG
#undef LLDEBUG
#undef L2DEBUG
#undef L3DEBUG

#undef LNOTICE
#undef LLNOTICE
#undef L2NOTICE
#undef L3NOTICE

#undef LWARNING
#undef LLWARNING
#undef L2WARNING
#undef L3WARNING

#undef LERROR
#undef LLERROR
#undef L2ERROR
#undef L3ERROR

#undef LCRITICAL
#undef LLCRITICAL
#undef L2CRITICAL
#undef L3CRITICAL

////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// DEFINES                                                                                //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

//
// Макросы логирвоания для разных уровней логирования
//

#define __LOCATION__   __FILE__,__PRETTY_FUNCTION__,__LINE__

#ifndef _LOG_PRINT_
#define _LOG_PRINT_ false
#endif


#define __L3_PARAMS__      __PRETTY_FUNCTION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), event.code, event.log()
#define __L3_PRINT_OK__    std::cout << _ERR_TEXT_ << "[" << event.code << "] " << event.log() << std::endl; 

//
//
//

#if _LOG_LEVEL_ == _LOG_DEBUG_ && _LOG_PRINT_ == true
#define LDEBUG(message)            ::tegia::logger::event("debug",__LOCATION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), 0, message); std::cout << message << std::endl;
#define LLDEBUG(code, message)     ::tegia::logger::event("debug",__LOCATION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), code, message); std::cout << message << std::endl;
#define L2DEBUG(code, message)     ::tegia::logger::event2("debug",__PRETTY_FUNCTION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), code, message); std::cout << message << std::endl;
#define L3DEBUG(event)             ::tegia::logger::event2("debug",__L3_PARAMS__); __L3_PRINT_OK__

#define LNOTICE(message)           ::tegia::logger::event("notice",__LOCATION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), 0, message); std::cout << message << std::endl;
#define LLNOTICE(code, message)    ::tegia::logger::event("notice",__LOCATION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), code, message); std::cout << message << std::endl;
#define L2NOTICE(code, message)    ::tegia::logger::event2("notice",__PRETTY_FUNCTION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), code, message); std::cout << message << std::endl;
#define L3NOTICE(event)            ::tegia::logger::event2("notice",__L3_PARAMS__); __L3_PRINT_OK__

#define LWARNING(message)          ::tegia::logger::event("warning",__LOCATION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), 0, message); std::cout << message << std::endl;
#define LLWARNING(code, message)   ::tegia::logger::event("warning",__LOCATION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), code, message); std::cout << message << std::endl;
#define L2WARNING(code, message)   ::tegia::logger::event2("warning",__PRETTY_FUNCTION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), code, message); std::cout << message << std::endl;
#define L3WARNING(event)           ::tegia::logger::event2("warning",__L3_PARAMS__); __L3_PRINT_OK__

#define LERROR(message)            ::tegia::logger::event("error",__LOCATION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), 0, message); std::cout << message << std::endl;
#define LLERROR(code, message)     ::tegia::logger::event("error",__LOCATION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), code, message); std::cout << message << std::endl;
#define L2ERROR(code, message)     ::tegia::logger::event2("error",__PRETTY_FUNCTION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), code, message); std::cout << message << std::endl;
#define L3ERROR(event)             ::tegia::logger::event2("error",__L3_PARAMS__); __L3_PRINT_OK__

#define LCRITICAL(message)         ::tegia::logger::event("critical",__LOCATION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), 0, message); std::cout << message << std::endl;
#define LLCRITICAL(code, message)  ::tegia::logger::event("critical",__LOCATION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), code, message); std::cout << message << std::endl;
#define L2CRITICAL(code, message)  ::tegia::logger::event2("critical",__PRETTY_FUNCTION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), code, message); std::cout << message << std::endl;
#define L3CRITICAL(event)          ::tegia::logger::event2("critical",__L3_PARAMS__); __L3_PRINT_OK__
#endif


#if _LOG_LEVEL_ == _LOG_DEBUG_ && _LOG_PRINT_ == false
#define LDEBUG(message)            ::tegia::logger::event("debug",__LOCATION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), 0, message);
#define LLDEBUG(code, message)     ::tegia::logger::event("debug",__LOCATION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), code, message);
#define L2DEBUG(code, message)     ::tegia::logger::event2("debug",__PRETTY_FUNCTION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), code, message);
#define L3DEBUG(event)             ::tegia::logger::event2("debug",__L3_PARAMS__);

#define LNOTICE(message)           ::tegia::logger::event("notice",__LOCATION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), 0, message);
#define LLNOTICE(code, message)    ::tegia::logger::event("notice",__LOCATION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), code, message);
#define L2NOTICE(code, message)    ::tegia::logger::event2("notice",__PRETTY_FUNCTION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), code, message);
#define L3NOTICE(event)            ::tegia::logger::event2("notice",__L3_PARAMS__);

#define LWARNING(message)          ::tegia::logger::event("warning",__LOCATION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), 0, message);
#define LLWARNING(code, message)   ::tegia::logger::event("warning",__LOCATION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), code, message);
#define L2WARNING(code, message)   ::tegia::logger::event2("warning",__PRETTY_FUNCTION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), code, message);
#define L3WARNING(event)           ::tegia::logger::event2("warning",__L3_PARAMS__);

#define LERROR(message)            ::tegia::logger::event("error",__LOCATION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), 0, message);
#define LLERROR(code, message)     ::tegia::logger::event("error",__LOCATION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), code, message);
#define L2ERROR(code, message)     ::tegia::logger::event2("error",__PRETTY_FUNCTION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), code, message);
#define L3ERROR(event)             ::tegia::logger::event2("error",__L3_PARAMS__);

#define LCRITICAL(message)         ::tegia::logger::event("critical",__LOCATION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), 0, message);
#define LLCRITICAL(code, message)  ::tegia::logger::event("critical",__LOCATION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), code, message);
#define L2CRITICAL(code, message)  ::tegia::logger::event2("critical",__PRETTY_FUNCTION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), code, message);
#define L3CRITICAL(event)          ::tegia::logger::event2("critical",__L3_PARAMS__);
#endif

//
//
//

#if _LOG_LEVEL_ == _LOG_NOTICE_ && _LOG_PRINT_ == true
#define LDEBUG(message)
#define LLDEBUG(code, message)
#define L2DEBUG(code, message)
#define L3DEBUG(event)

#define LNOTICE(message)           ::tegia::logger::event("notice",__LOCATION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), 0, message); std::cout << message << std::endl;
#define LLNOTICE(code, message)    ::tegia::logger::event("notice",__LOCATION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), code, message); std::cout << message << std::endl;
#define L2NOTICE(code, message)    ::tegia::logger::event2("notice",__PRETTY_FUNCTION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), code, message); std::cout << message << std::endl;
#define L3NOTICE(event)            ::tegia::logger::event2("notice",__L3_PARAMS__); __L3_PRINT_OK__

#define LWARNING(message)          ::tegia::logger::event("warning",__LOCATION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), 0, message); std::cout << message << std::endl;
#define LLWARNING(code, message)   ::tegia::logger::event("warning",__LOCATION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), code, message); std::cout << message << std::endl;
#define L2WARNING(code, message)   ::tegia::logger::event2("warning",__PRETTY_FUNCTION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), code, message); std::cout << message << std::endl;
#define L3WARNING(event)           ::tegia::logger::event2("warning",__L3_PARAMS__); __L3_PRINT_OK__

#define LERROR(message)            ::tegia::logger::event("error",__LOCATION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), 0, message); std::cout << message << std::endl;
#define LLERROR(code, message)     ::tegia::logger::event("error",__LOCATION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), code, message); std::cout << message << std::endl;
#define L2ERROR(code, message)     ::tegia::logger::event2("error",__PRETTY_FUNCTION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), code, message); std::cout << message << std::endl;
#define L3ERROR(event)             ::tegia::logger::event2("error",__L3_PARAMS__); __L3_PRINT_OK__

#define LCRITICAL(message)         ::tegia::logger::event("critical",__LOCATION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), 0, message); std::cout << message << std::endl;
#define LLCRITICAL(code, message)  ::tegia::logger::event("critical",__LOCATION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), code, message); std::cout << message << std::endl;
#define L2CRITICAL(code, message)  ::tegia::logger::event2("critical",__PRETTY_FUNCTION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), code, message); std::cout << message << std::endl;
#define L3CRITICAL(event)          ::tegia::logger::event2("critical",__L3_PARAMS__); __L3_PRINT_OK__
#endif

#if _LOG_LEVEL_ == _LOG_NOTICE_ && _LOG_PRINT_ == false
#define LDEBUG(message)
#define LLDEBUG(code, message)
#define L2DEBUG(code, message)
#define L3DEBUG(event)

#define LNOTICE(message)           ::tegia::logger::event("notice",__LOCATION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), 0, message);
#define LLNOTICE(code, message)    ::tegia::logger::event("notice",__LOCATION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), code, message);
#define L2NOTICE(code, message)    ::tegia::logger::event2("notice",__PRETTY_FUNCTION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), code, message);
#define L3NOTICE(event)            ::tegia::logger::event2("notice",__L3_PARAMS__);

#define LWARNING(message)          ::tegia::logger::event("warning",__LOCATION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), 0, message);
#define LLWARNING(code, message)   ::tegia::logger::event("warning",__LOCATION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), code, message);
#define L2WARNING(code, message)   ::tegia::logger::event2("warning",__PRETTY_FUNCTION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), code, message);
#define L3WARNING(event)           ::tegia::logger::event2("warning",__L3_PARAMS__);

#define LERROR(message)            ::tegia::logger::event("error",__LOCATION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), 0, message);
#define LLERROR(code, message)     ::tegia::logger::event("error",__LOCATION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), code, message);
#define L2ERROR(code, message)     ::tegia::logger::event2("error",__PRETTY_FUNCTION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), code, message);
#define L3ERROR(event)             ::tegia::logger::event2("error",__L3_PARAMS__);

#define LCRITICAL(message)         ::tegia::logger::event("critical",__LOCATION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), 0, message);
#define LLCRITICAL(code, message)  ::tegia::logger::event("critical",__LOCATION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), code, message);
#define L2CRITICAL(code, message)  ::tegia::logger::event2("critical",__PRETTY_FUNCTION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), code, message);
#define L3CRITICAL(event)          ::tegia::logger::event2("critical",__L3_PARAMS__);
#endif

//
//
//

#if _LOG_LEVEL_ == _LOG_WARNING_ && _LOG_PRINT_ == true
#define LDEBUG(message)
#define LLDEBUG(code, message)
#define L2DEBUG(code, message)
#define L3DEBUG(event)

#define LNOTICE(message)
#define LLNOTICE(code, message)
#define L2NOTICE(code, message)
#define L3NOTICE(event)

#define LWARNING(message)          ::tegia::logger::event("warning",__LOCATION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), 0, message); std::cout << message << std::endl;
#define LLWARNING(code, message)   ::tegia::logger::event("warning",__LOCATION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), code, message); std::cout << message << std::endl;
#define L2WARNING(code, message)   ::tegia::logger::event2("warning",__PRETTY_FUNCTION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), code, message); std::cout << message << std::endl;
#define L3WARNING(event)           ::tegia::logger::event2("warning",__L3_PARAMS__); __L3_PRINT_OK__

#define LERROR(message)            ::tegia::logger::event("error",__LOCATION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), 0, message); std::cout << message << std::endl;
#define LLERROR(code, message)     ::tegia::logger::event("error",__LOCATION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), code, message); std::cout << message << std::endl;
#define L2ERROR(code, message)     ::tegia::logger::event2("error",__PRETTY_FUNCTION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), code, message); std::cout << message << std::endl;
#define L3ERROR(event)             ::tegia::logger::event2("error",__L3_PARAMS__); __L3_PRINT_OK__

#define LCRITICAL(message)         ::tegia::logger::event("critical",__LOCATION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), 0, message); std::cout << message << std::endl;
#define LLCRITICAL(code, message)  ::tegia::logger::event("critical",__LOCATION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), code, message); std::cout << message << std::endl;
#define L2CRITICAL(code, message)  ::tegia::logger::event2("critical",__PRETTY_FUNCTION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), code, message); std::cout << message << std::endl;
#define L3CRITICAL(event)          ::tegia::logger::event2("critical",__L3_PARAMS__); __L3_PRINT_OK__
#endif

#if _LOG_LEVEL_ == _LOG_WARNING_ && _LOG_PRINT_ == false
#define LDEBUG(message)
#define LLDEBUG(code, message)
#define L2DEBUG(code, message)
#define L3DEBUG(event)

#define LNOTICE(message)
#define LLNOTICE(code, message)
#define L2NOTICE(code, message)
#define L3NOTICE(event)

#define LWARNING(message)          ::tegia::logger::event("warning",__LOCATION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), 0, message);
#define LLWARNING(code, message)   ::tegia::logger::event("warning",__LOCATION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), code, message);
#define L2WARNING(code, message)   ::tegia::logger::event2("warning",__PRETTY_FUNCTION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), code, message);
#define L3WARNING(event)           ::tegia::logger::event2("warning",__L3_PARAMS__);

#define LERROR(message)            ::tegia::logger::event("error",__LOCATION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), 0, message);
#define LLERROR(code, message)     ::tegia::logger::event("error",__LOCATION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), code, message);
#define L2ERROR(code, message)     ::tegia::logger::event2("error",__PRETTY_FUNCTION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), code, message);
#define L3ERROR(event)             ::tegia::logger::event2("error",__L3_PARAMS__);

#define LCRITICAL(message)         ::tegia::logger::event("critical",__LOCATION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), 0, message);
#define LLCRITICAL(code, message)  ::tegia::logger::event("critical",__LOCATION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), code, message);
#define L2CRITICAL(code, message)  ::tegia::logger::event2("critical",__PRETTY_FUNCTION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), code, message);
#define L3CRITICAL(event)          ::tegia::logger::event2("critical",__L3_PARAMS__);
#endif

//
//
//

#if _LOG_LEVEL_ == _LOG_ERROR_ && _LOG_PRINT_ == true
#define LDEBUG(message)
#define LLDEBUG(code, message)
#define L2DEBUG(code, message)
#define L3DEBUG(event)

#define LNOTICE(message)
#define LLNOTICE(code, message)
#define L2NOTICE(code, message)
#define L3NOTICE(event)

#define LWARNING(message)
#define LLWARNING(code, message)
#define L2WARNING(code, message)
#define L3WARNING(event)

#define LERROR(message)            ::tegia::logger::event("error",__LOCATION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), 0, message); std::cout << message << std::endl;
#define LLERROR(code, message)     ::tegia::logger::event("error",__LOCATION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), code, message); std::cout << message << std::endl;
#define L2ERROR(code, message)     ::tegia::logger::event2("error",__PRETTY_FUNCTION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), code, message); std::cout << message << std::endl;
#define L3ERROR(event)             ::tegia::logger::event2("error",__L3_PARAMS__); __L3_PRINT_OK__

#define LCRITICAL(message)         ::tegia::logger::event("critical",__LOCATION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), 0, message); std::cout << message << std::endl;
#define LLCRITICAL(code, message)  ::tegia::logger::event("critical",__LOCATION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), code, message); std::cout << message << std::endl;
#define L2CRITICAL(code, message)  ::tegia::logger::event2("critical",__PRETTY_FUNCTION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), code, message); std::cout << message << std::endl;
#define L3CRITICAL(event)          ::tegia::logger::event2("critical",__L3_PARAMS__); __L3_PRINT_OK__
#endif

#if _LOG_LEVEL_ == _LOG_ERROR_ && _LOG_PRINT_ == false
#define LDEBUG(message)
#define LLDEBUG(code, message)
#define L2DEBUG(code, message)
#define L3DEBUG(event)

#define LNOTICE(message)
#define LLNOTICE(code, message)
#define L2NOTICE(code, message)
#define L3NOTICE(event)

#define LWARNING(message)
#define LLWARNING(code, message)
#define L2WARNING(code, message)
#define L3WARNING(event)

#define LERROR(message)            ::tegia::logger::event("error",__LOCATION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), 0, message);
#define LLERROR(code, message)     ::tegia::logger::event("error",__LOCATION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), code, message);
#define L2ERROR(code, message)     ::tegia::logger::event2("error",__PRETTY_FUNCTION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), code, message);
#define L3ERROR(event)             ::tegia::logger::event2("error",__L3_PARAMS__);

#define LCRITICAL(message)         ::tegia::logger::event("critical",__LOCATION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), 0, message);
#define LLCRITICAL(code, message)  ::tegia::logger::event("critical",__LOCATION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), code, message);
#define L2CRITICAL(code, message)  ::tegia::logger::event2("critical",__PRETTY_FUNCTION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), code, message);
#define L3CRITICAL(event)          ::tegia::logger::event2("critical",__L3_PARAMS__);
#endif

//
//
//

#if _LOG_LEVEL_ == _LOG_CRITICAL_ && _LOG_PRINT_ == true
#define LDEBUG(message)
#define LLDEBUG(code, message)
#define L2DEBUG(code, message)
#define L3DEBUG(event)

#define LNOTICE(message)
#define LLNOTICE(code, message)
#define L2NOTICE(code, message)
#define L3NOTICE(event)

#define LWARNING(message)
#define LLWARNING(code, message)
#define L2WARNING(code, message)
#define L3WARNING(event)

#define LERROR(message)
#define LLERROR(code, message)
#define L2ERROR(code, message)
#define L3ERROR(event)

#define LCRITICAL(message)         ::tegia::logger::event("critical",__LOCATION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), 0, message); std::cout << message << std::endl;
#define LLCRITICAL(code, message)  ::tegia::logger::event("critical",__LOCATION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), code, message); std::cout << message << std::endl;
#define L2CRITICAL(code, message)  ::tegia::logger::event2("critical",__PRETTY_FUNCTION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), code, message); std::cout << message << std::endl;
#define L3CRITICAL(event)          ::tegia::logger::event2("critical",__L3_PARAMS__); __L3_PRINT_OK__
#endif

#if _LOG_LEVEL_ == _LOG_CRITICAL_ && _LOG_PRINT_ == false
#define LDEBUG(message)
#define LLDEBUG(code, message)
#define L2DEBUG(code, message)
#define L3DEBUG(event)

#define LNOTICE(message)
#define LLNOTICE(code, message)
#define L2NOTICE(code, message)
#define L3NOTICE(event)

#define LWARNING(message)
#define LLWARNING(code, message)
#define L2WARNING(code, message)
#define L3WARNING(event)

#define LERROR(message)
#define LLERROR(code, message)
#define L2ERROR(code, message)
#define L3ERROR(event)

#define LCRITICAL(message)         ::tegia::logger::event("critical",__LOCATION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), 0, message);
#define LLCRITICAL(code, message)  ::tegia::logger::event("critical",__LOCATION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), code, message);
#define L2CRITICAL(code, message)  ::tegia::logger::event2("critical",__PRETTY_FUNCTION__,std::chrono::high_resolution_clock::now(), tegia::threads::tid(), code, message);
#define L3CRITICAL(event)          ::tegia::logger::event2("critical",__L3_PARAMS__);
#endif
