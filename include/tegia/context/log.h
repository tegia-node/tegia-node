#undef __LOCATION__
#undef LDEBUG
#undef LNOTICE
#undef LWARNING
#undef LERROR
#undef LCRITICAL

////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// DEFINES                                                                                //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////


//
// Макросы логирвоания для разных уровней логирования
//

#define __LOCATION__   __FILE__,__FUNCTION__,__LINE__

#if _LOG_LEVEL_ == _LOG_DEBUG_
#define LDEBUG(text)     tegia::context::log( _LOG_DEBUG_,    std::chrono::high_resolution_clock::now(), __LOCATION__, text );    
#define LNOTICE(text)    tegia::context::log( _LOG_NOTICE_,   std::chrono::high_resolution_clock::now(), __LOCATION__, text );    
#define LWARNING(text)   tegia::context::log( _LOG_WARNING_,  std::chrono::high_resolution_clock::now(), __LOCATION__, text ); 
#define LERROR(text)     tegia::context::log( _LOG_ERROR_,    std::chrono::high_resolution_clock::now(), __LOCATION__, text );    
#define LCRITICAL(text)  tegia::context::log( _LOG_CRITICAL_, std::chrono::high_resolution_clock::now(), __LOCATION__, text );  
#endif

#if _LOG_LEVEL_ == _LOG_NOTICE_
#define LDEBUG(text)     
#define LNOTICE(text)    tegia::context::log( _LOG_NOTICE_,   std::chrono::high_resolution_clock::now(), __LOCATION__, text );    
#define LWARNING(text)   tegia::context::log( _LOG_WARNING_,  std::chrono::high_resolution_clock::now(), __LOCATION__, text ); 
#define LERROR(text)     tegia::context::log( _LOG_ERROR_,    std::chrono::high_resolution_clock::now(), __LOCATION__, text );    
#define LCRITICAL(text)  tegia::context::log( _LOG_CRITICAL_, std::chrono::high_resolution_clock::now(), __LOCATION__, text );  
#endif

#if _LOG_LEVEL_ == _LOG_WARNING_
#define LDEBUG(text)     
#define LNOTICE(text)
#define LWARNING(text)   tegia::context::log( _LOG_WARNING_,  std::chrono::high_resolution_clock::now(), __LOCATION__, text ); 
#define LERROR(text)     tegia::context::log( _LOG_ERROR_,    std::chrono::high_resolution_clock::now(), __LOCATION__, text );    
#define LCRITICAL(text)  tegia::context::log( _LOG_CRITICAL_, std::chrono::high_resolution_clock::now(), __LOCATION__, text );  
#endif

#if _LOG_LEVEL_ == _LOG_ERROR_
#define LDEBUG(text)     
#define LNOTICE(text)
#define LWARNING(text)
#define LERROR(text)     tegia::context::log( _LOG_ERROR_,    std::chrono::high_resolution_clock::now(), __LOCATION__, text );    
#define LCRITICAL(text)  tegia::context::log( _LOG_CRITICAL_, std::chrono::high_resolution_clock::now(), __LOCATION__, text );  
#endif

#if _LOG_LEVEL_ == _LOG_CRITICAL_
#define LDEBUG(text)     
#define LNOTICE(text)
#define LWARNING(text)
#define LERROR(text)
#define LCRITICAL(text)  tegia::context::log( _LOG_CRITICAL_, std::chrono::high_resolution_clock::now(), __LOCATION__, text );  
#endif
