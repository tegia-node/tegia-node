#include <tegia/context/context.h>

#undef __LOCATION__
#undef LDEBUG
#undef LSDEBUG
#undef LNOTICE
#undef LSNOTICE
#undef LWARNING
#undef LSWARNING
#undef LERROR
#undef LSERROR
#undef LCRITICAL
#undef LSCRITICAL

////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// DEFINES                                                                                //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////


//
// Макросы логирвоания для разных уровней логирования
//

#define __LOCATION__   __FILE__,__FUNCTION__,__LINE__

#ifndef _LOG_PRINT_
#define _LOG_PRINT_ false
#endif

//
//
//

#if _LOG_LEVEL_ == _LOG_DEBUG_ && _LOG_PRINT_ == true
#define LDEBUG(text)      ::tegia::context::log( _LOG_DEBUG_,    std::chrono::high_resolution_clock::now(), __LOCATION__, text ); std::cout << text << std::endl;
#define LSDEBUG(text)     ::tegia::context::log( _LOG_DEBUG_,    std::chrono::high_resolution_clock::now(), __LOCATION__, (std::ostringstream{} << text).str() ); std::cout << text << std::endl;
#define LNOTICE(text)     ::tegia::context::log( _LOG_NOTICE_,   std::chrono::high_resolution_clock::now(), __LOCATION__, text ); std::cout << text << std::endl;    
#define LSNOTICE(text)    ::tegia::context::log( _LOG_NOTICE_,   std::chrono::high_resolution_clock::now(), __LOCATION__, (std::ostringstream{} << text).str() ); std::cout << text << std::endl;
#define LWARNING(text)    ::tegia::context::log( _LOG_WARNING_,  std::chrono::high_resolution_clock::now(), __LOCATION__, text ); std::cout << text << std::endl;
#define LSWARNING(text)   ::tegia::context::log( _LOG_WARNING_,  std::chrono::high_resolution_clock::now(), __LOCATION__, (std::ostringstream{} << text).str() ); std::cout << text << std::endl;
#define LERROR(text)      ::tegia::context::log( _LOG_ERROR_,    std::chrono::high_resolution_clock::now(), __LOCATION__, text ); std::cout << text << std::endl; 
#define LSERROR(text)     ::tegia::context::log( _LOG_ERROR_,    std::chrono::high_resolution_clock::now(), __LOCATION__, (std::ostringstream{} << text).str() ); std::cout << text << std::endl; 
#define LCRITICAL(text)   ::tegia::context::log( _LOG_CRITICAL_, std::chrono::high_resolution_clock::now(), __LOCATION__, text ); std::cout << text << std::endl; 
#define LSCRITICAL(text)  ::tegia::context::log( _LOG_CRITICAL_, std::chrono::high_resolution_clock::now(), __LOCATION__, (std::ostringstream{} << text).str() ); std::cout << text << std::endl; 
#endif

#if _LOG_LEVEL_ == _LOG_DEBUG_ && _LOG_PRINT_ == false
#define LDEBUG(text)      ::tegia::context::log( _LOG_DEBUG_,    std::chrono::high_resolution_clock::now(), __LOCATION__, text );    
#define LSDEBUG(text)     ::tegia::context::log( _LOG_DEBUG_,    std::chrono::high_resolution_clock::now(), __LOCATION__, (std::ostringstream{} << text).str() );    
#define LNOTICE(text)     ::tegia::context::log( _LOG_NOTICE_,   std::chrono::high_resolution_clock::now(), __LOCATION__, text );    
#define LSNOTICE(text)    ::tegia::context::log( _LOG_NOTICE_,   std::chrono::high_resolution_clock::now(), __LOCATION__, (std::ostringstream{} << text).str() );    
#define LWARNING(text)    ::tegia::context::log( _LOG_WARNING_,  std::chrono::high_resolution_clock::now(), __LOCATION__, text ); 
#define LSWARNING(text)   ::tegia::context::log( _LOG_WARNING_,  std::chrono::high_resolution_clock::now(), __LOCATION__, (std::ostringstream{} << text).str() ); 
#define LERROR(text)      ::tegia::context::log( _LOG_ERROR_,    std::chrono::high_resolution_clock::now(), __LOCATION__, text );    
#define LSERROR(text)     ::tegia::context::log( _LOG_ERROR_,    std::chrono::high_resolution_clock::now(), __LOCATION__, (std::ostringstream{} << text).str() );    
#define LCRITICAL(text)   ::tegia::context::log( _LOG_CRITICAL_, std::chrono::high_resolution_clock::now(), __LOCATION__, text );  
#define LSCRITICAL(text)  ::tegia::context::log( _LOG_CRITICAL_, std::chrono::high_resolution_clock::now(), __LOCATION__, (std::ostringstream{} << text).str() );  
#endif

//
//
//

#if _LOG_LEVEL_ == _LOG_NOTICE_ && _LOG_PRINT_ == true
#define LDEBUG(text)     
#define LSDEBUG(text)     
#define LNOTICE(text)     ::tegia::context::log( _LOG_NOTICE_,   std::chrono::high_resolution_clock::now(), __LOCATION__, text ); std::cout << text << std::endl;    
#define LSNOTICE(text)    ::tegia::context::log( _LOG_NOTICE_,   std::chrono::high_resolution_clock::now(), __LOCATION__, (std::ostringstream{} << text).str() ); std::cout << text << std::endl;
#define LWARNING(text)    ::tegia::context::log( _LOG_WARNING_,  std::chrono::high_resolution_clock::now(), __LOCATION__, text ); std::cout << text << std::endl;
#define LSWARNING(text)   ::tegia::context::log( _LOG_WARNING_,  std::chrono::high_resolution_clock::now(), __LOCATION__, (std::ostringstream{} << text).str() ); std::cout << text << std::endl;
#define LERROR(text)      ::tegia::context::log( _LOG_ERROR_,    std::chrono::high_resolution_clock::now(), __LOCATION__, text ); std::cout << text << std::endl; 
#define LSERROR(text)     ::tegia::context::log( _LOG_ERROR_,    std::chrono::high_resolution_clock::now(), __LOCATION__, (std::ostringstream{} << text).str() ); std::cout << text << std::endl; 
#define LCRITICAL(text)   ::tegia::context::log( _LOG_CRITICAL_, std::chrono::high_resolution_clock::now(), __LOCATION__, text ); std::cout << text << std::endl; 
#define LSCRITICAL(text)  ::tegia::context::log( _LOG_CRITICAL_, std::chrono::high_resolution_clock::now(), __LOCATION__, (std::ostringstream{} << text).str() ); std::cout << text << std::endl; 
#endif

#if _LOG_LEVEL_ == _LOG_NOTICE_ && _LOG_PRINT_ == false
#define LDEBUG(text)     
#define LSDEBUG(text)     
#define LNOTICE(text)     ::tegia::context::log( _LOG_NOTICE_,   std::chrono::high_resolution_clock::now(), __LOCATION__, text );    
#define LSNOTICE(text)    ::tegia::context::log( _LOG_NOTICE_,   std::chrono::high_resolution_clock::now(), __LOCATION__, (std::ostringstream{} << text).str() );    
#define LWARNING(text)    ::tegia::context::log( _LOG_WARNING_,  std::chrono::high_resolution_clock::now(), __LOCATION__, text ); 
#define LSWARNING(text)   ::tegia::context::log( _LOG_WARNING_,  std::chrono::high_resolution_clock::now(), __LOCATION__, (std::ostringstream{} << text).str() ); 
#define LERROR(text)      ::tegia::context::log( _LOG_ERROR_,    std::chrono::high_resolution_clock::now(), __LOCATION__, text );    
#define LSERROR(text)     ::tegia::context::log( _LOG_ERROR_,    std::chrono::high_resolution_clock::now(), __LOCATION__, (std::ostringstream{} << text).str() );    
#define LCRITICAL(text)   ::tegia::context::log( _LOG_CRITICAL_, std::chrono::high_resolution_clock::now(), __LOCATION__, text );  
#define LSCRITICAL(text)  ::tegia::context::log( _LOG_CRITICAL_, std::chrono::high_resolution_clock::now(), __LOCATION__, (std::ostringstream{} << text).str() );  
#endif

//
//
//

#if _LOG_LEVEL_ == _LOG_WARNING_ && _LOG_PRINT_ == true
#define LDEBUG(text)     
#define LSDEBUG(text)     
#define LNOTICE(text)
#define LSNOTICE(text)
#define LWARNING(text)    ::tegia::context::log( _LOG_WARNING_,  std::chrono::high_resolution_clock::now(), __LOCATION__, text ); std::cout << text << std::endl;
#define LSWARNING(text)   ::tegia::context::log( _LOG_WARNING_,  std::chrono::high_resolution_clock::now(), __LOCATION__, (std::ostringstream{} << text).str() ); std::cout << text << std::endl;
#define LERROR(text)      ::tegia::context::log( _LOG_ERROR_,    std::chrono::high_resolution_clock::now(), __LOCATION__, text ); std::cout << text << std::endl; 
#define LSERROR(text)     ::tegia::context::log( _LOG_ERROR_,    std::chrono::high_resolution_clock::now(), __LOCATION__, (std::ostringstream{} << text).str() ); std::cout << text << std::endl; 
#define LCRITICAL(text)   ::tegia::context::log( _LOG_CRITICAL_, std::chrono::high_resolution_clock::now(), __LOCATION__, text ); std::cout << text << std::endl; 
#define LSCRITICAL(text)  ::tegia::context::log( _LOG_CRITICAL_, std::chrono::high_resolution_clock::now(), __LOCATION__, (std::ostringstream{} << text).str() ); std::cout << text << std::endl; 
#endif

#if _LOG_LEVEL_ == _LOG_WARNING_ && _LOG_PRINT_ == false
#define LDEBUG(text)     
#define LSDEBUG(text)     
#define LNOTICE(text)
#define LSNOTICE(text)
#define LWARNING(text)    ::tegia::context::log( _LOG_WARNING_,  std::chrono::high_resolution_clock::now(), __LOCATION__, text ); 
#define LSWARNING(text)   ::tegia::context::log( _LOG_WARNING_,  std::chrono::high_resolution_clock::now(), __LOCATION__, (std::ostringstream{} << text).str() ); 
#define LERROR(text)      ::tegia::context::log( _LOG_ERROR_,    std::chrono::high_resolution_clock::now(), __LOCATION__, text );    
#define LSERROR(text)     ::tegia::context::log( _LOG_ERROR_,    std::chrono::high_resolution_clock::now(), __LOCATION__, (std::ostringstream{} << text).str() );    
#define LCRITICAL(text)   ::tegia::context::log( _LOG_CRITICAL_, std::chrono::high_resolution_clock::now(), __LOCATION__, text );  
#define LSCRITICAL(text)  ::tegia::context::log( _LOG_CRITICAL_, std::chrono::high_resolution_clock::now(), __LOCATION__, (std::ostringstream{} << text).str() );  
#endif

//
//
//

#if _LOG_LEVEL_ == _LOG_ERROR_ && _LOG_PRINT_ == true
#define LDEBUG(text)     
#define LSDEBUG(text)     
#define LNOTICE(text)
#define LSNOTICE(text)
#define LWARNING(text)
#define LSWARNING(text)
#define LERROR(text)      ::tegia::context::log( _LOG_ERROR_,    std::chrono::high_resolution_clock::now(), __LOCATION__, text ); std::cout << text << std::endl;   
#define LSERROR(text)     ::tegia::context::log( _LOG_ERROR_,    std::chrono::high_resolution_clock::now(), __LOCATION__, (std::ostringstream{} << text).str() ); std::cout << text << std::endl;   
#define LCRITICAL(text)   ::tegia::context::log( _LOG_CRITICAL_, std::chrono::high_resolution_clock::now(), __LOCATION__, text ); std::cout << text << std::endl; 
#define LSCRITICAL(text)  ::tegia::context::log( _LOG_CRITICAL_, std::chrono::high_resolution_clock::now(), __LOCATION__, (std::ostringstream{} << text).str() ); std::cout << text << std::endl; 
#endif

#if _LOG_LEVEL_ == _LOG_ERROR_ && _LOG_PRINT_ == false
#define LDEBUG(text)     
#define LSDEBUG(text)     
#define LNOTICE(text)
#define LSNOTICE(text)
#define LWARNING(text)
#define LSWARNING(text)
#define LERROR(text)      ::tegia::context::log( _LOG_ERROR_,    std::chrono::high_resolution_clock::now(), __LOCATION__, text );    
#define LSERROR(text)     ::tegia::context::log( _LOG_ERROR_,    std::chrono::high_resolution_clock::now(), __LOCATION__, (std::ostringstream{} << text).str() );    
#define LCRITICAL(text)   ::tegia::context::log( _LOG_CRITICAL_, std::chrono::high_resolution_clock::now(), __LOCATION__, text );  
#define LSCRITICAL(text)  ::tegia::context::log( _LOG_CRITICAL_, std::chrono::high_resolution_clock::now(), __LOCATION__, (std::ostringstream{} << text).str() );  
#endif

//
//
//

#if _LOG_LEVEL_ == _LOG_CRITICAL_ && _LOG_PRINT_ == true
#define LDEBUG(text)     
#define LSDEBUG(text)     
#define LNOTICE(text)
#define LSNOTICE(text)
#define LWARNING(text)
#define LSWARNING(text)
#define LERROR(text)
#define LSERROR(text)
#define LCRITICAL(text)   ::tegia::context::log( _LOG_CRITICAL_, std::chrono::high_resolution_clock::now(), __LOCATION__, text ); std::cout << text << std::endl; 
#define LSCRITICAL(text)  ::tegia::context::log( _LOG_CRITICAL_, std::chrono::high_resolution_clock::now(), __LOCATION__, (std::ostringstream{} << text).str() ); std::cout << text << std::endl; 
#endif

#if _LOG_LEVEL_ == _LOG_CRITICAL_ && _LOG_PRINT_ == false
#define LDEBUG(text)     
#define LSDEBUG(text)     
#define LNOTICE(text)
#define LSNOTICE(text)
#define LWARNING(text)
#define LSWARNING(text)
#define LERROR(text)
#define LSERROR(text)
#define LCRITICAL(text)   ::tegia::context::log( _LOG_CRITICAL_, std::chrono::high_resolution_clock::now(), __LOCATION__, text );  
#define LSCRITICAL(text)  ::tegia::context::log( _LOG_CRITICAL_, std::chrono::high_resolution_clock::now(), __LOCATION__, (std::ostringstream{} << text).str() );  
#endif
