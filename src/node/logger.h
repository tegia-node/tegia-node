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

/*
	# Уровни логирования

	- debug   - нужен разработчикам, отключается в продакшене.
	- info    - логирует нормальную работу приложения.
	- warning - предупреждения о потенциальных проблемах.
	- error   - ошибки, но приложение продолжает работать.
	- critical - фатальные ошибки, угрожающие работе системы.

	# Почему принято решение отказаться от логирования функции / файла / строки

	1. Любой из этих параметров может поменяться. Особенно срока. Из-за этого 
	для ранее зарегистрированных событий информация об источнике события может быть неактуальна
	2. Использованием макросов __FILE__, __FUNCTION__, __LINE__ либо порождает использование 
	макросов для вызова функции логирования, либо указание этих параметров явням образом. Это 
	тоже не является удобным 

	Вместо этого предлагается ввести "тип события" в виде строки. Это может быть как понятная 
	человеку строка, так и просто случайным образом сгенерированная строка, по которой можно 
	легко найти место возникновения ошибки

*/

#define _LOG_DEBUG_     0
#define _LOG_NOTICE_    1
#define _LOG_WARNING_   2
#define _LOG_ERROR_     3
#define _LOG_CRITICAL_  4


namespace tegia {
namespace node {

class logger
{
	public:
		logger(const std::string &logdir); 
		~logger();

		logger(logger &other) = delete;
		void operator=(const logger &) = delete;

		void write(
			const std::string &level,
			const std::string &filename, 
			const std::string &function, 
			const int line,
			const std::chrono::high_resolution_clock::time_point now,
			const std::string &thread,
			int code, 
			const std::string &message);
			
	private:
		std::ofstream f_main_log;     // Файл для всего журнала
		std::mutex  log_lock;
};

} // END namespace node
} // END namespace tegia


//
//
//



#endif