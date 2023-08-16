#ifndef H_TEGIA_NODE_LOGGER
#define H_TEGIA_NODE_LOGGER

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
// LOGGER CLASS                                                                           //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

#define _LOG_DEBUG_     0  // Отладочные сообщения
#define _LOG_NOTICE_    1  // Уведомления о возможных проблемах
#define _LOG_WARNING_   2  // Важные уведомления
#define _LOG_ERROR_     3  // Серьезные ошибки в коде или логике работы Платформы
#define _LOG_CRITICAL_  4  // Совсем беда!



namespace tegia {

class logger
{
	protected:  

		std::ofstream flog_all;     // Файл для всего журнала
		std::mutex  log_lock; 

		static logger* _self;

	public: 

		logger(const std::string &logdir, int log_level); 
		~logger();

		void writer(
			const int level,
			const std::chrono::high_resolution_clock::time_point now,
			const std::string &filename, 
			const std::string &function, 
			const int line,			
			const std::string &message
		);
		
		static logger* instance(const std::string &logdir = "", int log_level = 0);

		logger(logger &other) = delete;

		void operator=(const logger &) = delete;

}; // end class logger

}  // end namespace tegia


#endif