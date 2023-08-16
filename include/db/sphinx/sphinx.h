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


	/**
	 *	\brief		Выполняет вставку или замену RT-индекса в Sphinx
	 *	\authors	Горячев Игорь 
	 *	\details
	 */

	template<class _Obj_> tegia::mysql::result<_Obj_> replace(const std::string& connection_name, _Obj_ *obj)
	{
		tegia::mysql::result<_Obj_> _result;
		_result.obj = obj;

		//
		// Генерируем SQL-запрос
		//

		std::string table_name = obj->access->get_table_name();
		
		std::string columns = "";
		std::string values = "";

		for (auto getters_iterator = obj->access->fields_getter_begin(); 
				getters_iterator != obj->access->fields_getter_end();
				getters_iterator++)
		{
			if (getters_iterator->first == "_id")
			{
				continue;
			}
			columns += "`" + getters_iterator->first + "`,";

			//  Формируем список значений
			std::string val = tegia::sphinx::escape(getters_iterator->second());
			values += "'" + val + "',";
		}

		columns.pop_back();
		values.pop_back();
		std::string query_str = "REPLACE INTO `" + table_name + "` (" + columns + ") VALUES (" + values + ")";

		auto records = tegia::mysql::query(connection_name,query_str);

		_result.code = records->code;
		_result.info = records->info;	

		obj->_id(records->getLastInsertId());

		delete records;
		return _result;
	};

}	// END namespace sphinx
}	// END namespace tegia

#endif