#ifndef H_TEGIA_DB_SPHINX
#define H_TEGIA_DB_SPHINX

////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// INCLUDE                                                                                //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////


#include <tegia/dao/entity.h>
#include <tegia/db/mysql/mysql.h>


////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// namespace tegia::sphinx                                                                //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

/**
 * 
 * 
*/

namespace tegia {
namespace sphinx {

	/**
	 *	\brief		Выполняет экранирование строки для SQL-запроса в Sphinx
	 *	\authors	Горячев Игорь 
	 *	\details
	 */

	std::string escape(const std::string &str);



}	// END namespace sphinx
}	// END namespace tegia

#endif