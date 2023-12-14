#ifndef H_TEGIA_DB_MYSQL
#define H_TEGIA_DB_MYSQL

////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// INCLUDE                                                                                //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

// #include <tegia/dao/entity.h>

#include "const.h"
//#include "mysql_result.h"
#include "records.h"

////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// namespace tegia::mysql                                                                 //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

/**
 * 
 * 
*/

namespace tegia{
namespace mysql{

	/**
	 *	\brief		Выполнение SQL-запроса
	 *	\authors	Горячев Игорь 
	 *	\details
	 */

	tegia::mysql::records *query(const std::string &context, const std::string &query, bool trace = false);

	/**
	 *	\brief		Выполняет экранирование строки для SQL-запроса
	 *	\authors	Горячев Игорь 
	 *	\details
	 */

	std::string strip(const std::string &query);

} // END namespace mysql
} // END namespace tegia

#endif