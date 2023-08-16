#ifndef H_TEGIA_PLATFORM_LOGGER
#define H_TEGIA_PLATFORM_LOGGER

////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// INCLUDES                                                                               //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

#include <string>
#include <stdio.h>
#include <stdlib.h>

#include <fstream>
#include <iostream>
#include <sstream>

#include <mutex>
#include <chrono>


////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// DEFINES                                                                                //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////


#define __LOCATION__   __FILE__,__FUNCTION__,__LINE__

//
// Определяем типы сообщений
//

#define _LOG_DEBUG_     0  // Отладочные сообщения
#define _LOG_NOTICE_    1  // Уведомления о возможных проблемах
#define _LOG_WARNING_   2  // Важные уведомления
#define _LOG_ERROR_     3  // Серьезные ошибки в коде или логике работы Платформы
#define _LOG_CRITICAL_  4  // Совсем беда!

//
// Текущий уровень логирования 
//

#define _LOG_LEVEL_ _LOG_NOTICE_

//
// Макросы логирвоания для разных уровней логирования
//

#if _LOG_LEVEL_ == _LOG_DEBUG_
#define LDEBUG(text)     tegia::logger::instance()->writer( _LOG_DEBUG_,    std::chrono::high_resolution_clock::now(), __LOCATION__, core::string::format() << text );    
#define LNOTICE(text)    tegia::logger::instance()->writer( _LOG_NOTICE_,   std::chrono::high_resolution_clock::now(), __LOCATION__, core::string::format() << text );    
#define LWARNING(text)   tegia::logger::instance()->writer( _LOG_WARNING_,  std::chrono::high_resolution_clock::now(), __LOCATION__, core::string::format() << text ); 
#define LERROR(text)     tegia::logger::instance()->writer( _LOG_ERROR_,    std::chrono::high_resolution_clock::now(), __LOCATION__, core::string::format() << text );    
#define LCRITICAL(text)  tegia::logger::instance()->writer( _LOG_CRITICAL_, std::chrono::high_resolution_clock::now(), __LOCATION__, core::string::format() << text );  
#endif

#if _LOG_LEVEL_ == _LOG_NOTICE_
#define LDEBUG(text)     
#define LNOTICE(text)    tegia::logger::instance()->writer( _LOG_NOTICE_,   std::chrono::high_resolution_clock::now(), __LOCATION__, core::string::format() << text );    
#define LWARNING(text)   tegia::logger::instance()->writer( _LOG_WARNING_,  std::chrono::high_resolution_clock::now(), __LOCATION__, core::string::format() << text ); 
#define LERROR(text)     tegia::logger::instance()->writer( _LOG_ERROR_,    std::chrono::high_resolution_clock::now(), __LOCATION__, core::string::format() << text );    
#define LCRITICAL(text)  tegia::logger::instance()->writer( _LOG_CRITICAL_, std::chrono::high_resolution_clock::now(), __LOCATION__, core::string::format() << text );  
#endif

#if _LOG_LEVEL_ == _LOG_WARNING_
#define LDEBUG(text)     
#define LNOTICE(text)    
#define LWARNING(text)   tegia::logger::instance()->writer( _LOG_WARNING_,  std::chrono::high_resolution_clock::now(), __LOCATION__, core::string::format() << text ); 
#define LERROR(text)     tegia::logger::instance()->writer( _LOG_ERROR_,    std::chrono::high_resolution_clock::now(), __LOCATION__, core::string::format() << text );    
#define LCRITICAL(text)  tegia::logger::instance()->writer( _LOG_CRITICAL_, std::chrono::high_resolution_clock::now(), __LOCATION__, core::string::format() << text );  
#endif

#if _LOG_LEVEL_ == _LOG_ERROR_
#define LDEBUG(text)     
#define LNOTICE(text)    
#define LWARNING(text)   
#define LERROR(text)     tegia::logger::instance()->writer( _LOG_ERROR_,    std::chrono::high_resolution_clock::now(), __LOCATION__, core::string::format() << text );    
#define LCRITICAL(text)  tegia::logger::instance()->writer( _LOG_CRITICAL_, std::chrono::high_resolution_clock::now(), __LOCATION__, core::string::format() << text );  
#endif

#if _LOG_LEVEL_ == _LOG_CRITICAL_
#define LDEBUG(text)     
#define LNOTICE(text)    
#define LWARNING(text)   
#define LERROR(text)     
#define LCRITICAL(text)  tegia::logger::instance()->writer( _LOG_CRITICAL_, std::chrono::high_resolution_clock::now(), __LOCATION__, core::string::format() << text );  
#endif

////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// LOGGER CLASS                                                                           //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////


namespace tegia {

class logger
{
	protected:  

		std::ofstream flog_all;     // Файл для всего журнала
		std::mutex  log_lock; 

		static logger* _self;

	public: 

		logger(const std::string &filename, int log_level); 
		~logger();

		void writer(
			const int level,
			const std::chrono::high_resolution_clock::time_point now,
			const std::string &filename, 
			const std::string &function, 
			const int line,			
			const std::string &message
		);
		
		static logger* instance(const std::string &filename = "", int log_level = 0);

		logger(logger &other) = delete;

		void operator=(const logger &) = delete;

}; // end class logger

}  // end namespace tegia


#endif