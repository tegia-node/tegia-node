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

struct table_t
{
	std::string name = "";
	std::string comment = "";
	nlohmann::json data = nlohmann::json::object();
};


/*

*/


} // END namespace mysql
} // END namespace tegia

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

	std::string date(const std::string &datetime);

	std::tuple<int,table_t*> table(const std::string &context, const std::string &name);


} // END namespace mysql
} // END namespace tegia


namespace tegia{
namespace mysql{
namespace transaction{

	int start(const std::string &context);
	int commit(const std::string &context);
	int rollback(const std::string &context);

} // END namespace transaction
} // END namespace mysql
} // END namespace tegia

#endif