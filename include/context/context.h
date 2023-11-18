#ifndef H_TEGIA_CONTEXT
#define H_TEGIA_CONTEXT
// --------------------------------------------------------------------


#include <tegia2/context/user.h>
#include <tegia2/core/json.h>

#define _LOG_DEBUG_     0  // Отладочные сообщения
#define _LOG_NOTICE_    1  // Уведомления о возможных проблемах
#define _LOG_WARNING_   2  // Важные уведомления
#define _LOG_ERROR_     3  // Серьезные ошибки в коде или логике работы Платформы
#define _LOG_CRITICAL_  4  // Совсем беда!

namespace tegia {
class context
{
	public:
		static std::string tid();
		static ::tegia::user * const user();

		static const nlohmann::json * const config(const std::string &name);

		static void log(
			const int level,
			const std::chrono::high_resolution_clock::time_point now,
			const std::string &filename, 
			const std::string &function, 
			const int line,			
			const std::string &message			
		);

};	// END CLASS context
}	// END NAMESPACE tegia 


// --------------------------------------------------------------------
#endif