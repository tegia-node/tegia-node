#ifndef H_TEGIA_DB_MYSQL
#define H_TEGIA_DB_MYSQL

////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// INCLUDE                                                                                //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

#include <tegia/dao/entity.h>

#include "mysql_const.h"
#include "mysql_result.h"
#include "mysql_records.h"

////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// namespace tegia::mysql                                                                 //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

/**
 * 
 * 
*/

namespace tegia
{
	namespace mysql
	{

		/**
	 *	\brief		Создает пул соединений с СУБД
	 *	\authors	Горячев Игорь 
	 *	\details
	 */

		bool connect(int thread_count, nlohmann::json config);

		/**
	 *	\brief		Выполняет экранирование строки для SQL-запроса
	 *	\authors	Горячев Игорь 
	 *	\details
	 */

		std::string strip(const std::string &query);

		/**
	 *	\brief		Извлекает имя и значение ключа при возникновении ошибки Duplicate entry
	 *	\authors	Горячев Игорь 
	 *	\details
	 */

		std::tuple<bool, std::string, std::string> get_duplicate_info(const std::string &info);

		/**
	 *	\brief		Выполнение SQL-запроса
	 *	\authors	Горячев Игорь 
	 *	\details
	 */

		tegia::mysql::records *query(const std::string &connection_name, const std::string &query, bool trace = false);

		/////////////////////////////////////////////////////////////////////////////////////////////
		/**
	 *	\brief		Выполняет чтение объекта из БД
	 *	\authors	Горячев Игорь 
	 *	\details
	 */
		/////////////////////////////////////////////////////////////////////////////////////////////

		template <class _Obj_>
		result<_Obj_> read(const std::string &connection_name, _Obj_ *obj, const std::string &where)
		{
			std::string table_name = obj->access->get_table_name();
			tegia::mysql::result<_Obj_> result;

			auto records = tegia::mysql::query(connection_name, "SELECT * FROM `" + table_name + "` WHERE " + where);

			if (records->code == 200)
			{
				if (records->count() == 1)
				{
					for (
						auto setter_iterator = obj->access->fields_setter_begin();
						setter_iterator != obj->access->fields_setter_end();
						setter_iterator++)
					{
						std::string field_name = setter_iterator->first;
						obj->access->set(field_name, records->get(table_name + "::" + field_name, 0));
					}

					result.code = 200;
					result.obj = obj;
				}
				else
				{
					result.code = 404;
					result.info = "records = " + core::cast<std::string>(records->count());
					result.obj = obj;
				}
			}
			else
			{
				result.code = records->code;
				result.info = records->info;
				result.obj = obj;
			}

			delete records;
			return result;
		};

		/////////////////////////////////////////////////////////////////////////////////////////////
		/**
	 *	\brief		Выполняет "конструирование" объекта из результатов запроса к БД
	 *	\authors	Горячев Игорь 
	 *	\details
	 */
		/////////////////////////////////////////////////////////////////////////////////////////////

		template <class _Obj_>
		result<_Obj_> read(tegia::mysql::records *records, _Obj_ *obj, const int position)
		{
			std::string table_name = obj->access->get_table_name();
			tegia::mysql::result<_Obj_> result;

			for (
				auto setter_iterator = obj->access->fields_setter_begin();
				setter_iterator != obj->access->fields_setter_end();
				setter_iterator++)
			{
				std::string field_name = setter_iterator->first;
				obj->access->set(field_name, records->get(table_name + "::" + field_name, position));
			}

			result.code = 200;
			result.obj = obj;

			return result;
		};

		/////////////////////////////////////////////////////////////////////////////////////////////
		/**
	 *	\brief		Выполняет запись объекта в БД
	 *	\authors	Горячев Игорь 
	 *	\details
	 */
		/////////////////////////////////////////////////////////////////////////////////////////////

		template <class _Obj_>
		tegia::mysql::result<_Obj_> create(
			const std::string &connection_name,
			_Obj_ *obj,
			bool trace = false,
			int on_duplicate_flag = tegia::mysql::ON_DUPLICATE_ERROR)
		{
			tegia::mysql::result<_Obj_> _result;
			_result.obj = obj;

			//
			// Генерируем SQL-запрос
			//

			std::string table_name = obj->access->get_table_name();

			std::string columns = "";
			std::string values = "";
			std::string on_duplicate = "";

			for (auto getters_iterator = obj->access->fields_getter_begin();
				 getters_iterator != obj->access->fields_getter_end();
				 getters_iterator++)
			{
				//  Формируем список столбцов
				if (getters_iterator->first == "_id")
				{
					continue;
				}
				columns += "`" + getters_iterator->first + "`,";

				//  Формируем список значений
				std::string val = tegia::mysql::strip(getters_iterator->second());
				values += "'" + val + "',";
				on_duplicate += " `" + getters_iterator->first + "` = '" + val + "',";
			}

			columns.pop_back();
			values.pop_back();
			std::string query_str = "INSERT INTO `" + table_name + "` (" + columns + ") VALUES (" + values + ")";

			if (on_duplicate_flag == tegia::mysql::ON_DUPLICATE_UPDATE)
			{
				on_duplicate.pop_back();
				query_str = query_str + " ON DUPLICATE KEY UPDATE " + on_duplicate;
			}

			//
			//  Выполняем SQL-запрос
			//

			auto records = tegia::mysql::query(connection_name, query_str, trace);
			obj->_id(records->getLastInsertId());

			_result.code = records->code;
			_result.info = records->info;

			delete records;

			return _result;
		};

		/////////////////////////////////////////////////////////////////////////////////////////////
		/**
	 *	\brief		Выполняет обновление объекта в БД
	 *	\authors	Горячев Игорь 
	 *	\details
	 */
		/////////////////////////////////////////////////////////////////////////////////////////////

		template <class _Obj_>
		tegia::mysql::result<_Obj_> update(
			const std::string &connection_name,
			_Obj_ *obj,
			const std::string &update_key = "_id")
		{
			tegia::mysql::result<_Obj_> _result;
			_result.obj = obj;

			std::string table_name = obj->access->get_table_name();

			//
			//  Перебираем сеттеры объекта данного типа
			//

			std::string fields = "";
			for (auto getters_iterator = obj->access->fields_getter_begin();
				 getters_iterator != obj->access->fields_getter_end();
				 getters_iterator++)
			{
				if (getters_iterator->first == "_id")
				{
					continue;
				}
				fields += "`" + getters_iterator->first + "` = '" + tegia::mysql::strip(getters_iterator->second()) + "',";
			}
			fields.pop_back();

			//
			// REM
			//   update_key - это поле, по которому происходит проверка поиск обновляемых объектов в БД
			//   По-умолчанию этим полем является первичный ключ, но могут быть ситуации, когда правильнее
			//   использовать другое поле, на котором есть unique индекс
			//

			std::string where = " WHERE `" + update_key + "` = '" + obj->access->get(update_key) + "'";
			std::string query_str = "UPDATE `" + table_name + "` SET " + fields + where;

			auto records = tegia::mysql::query(connection_name, query_str);

			_result.code = records->code;
			_result.info = records->info;
			_result.affected_rows = records->get_affected_rows();

			delete records;

			return _result;
		};

		////////////////////////////////////////////////////////////////////////////////////////////
		/** 
	 *	\brief      Удаление объекта из БД
	 *	\authors	Горячев Игорь 
	 *	\details
	 */
		////////////////////////////////////////////////////////////////////////////////////////////

		template <class _Obj_>
		tegia::mysql::result<_Obj_> delet(
			const std::string &connection_name,
			_Obj_ *obj,
			const std::string &unique_key,
			const std::string &unique_value = "")
		{
			tegia::mysql::result<_Obj_> _result;
			std::string table_name = obj->access->get_table_name();
			std::string query_str;

			if (unique_value == "")
			{
				query_str = "DELETE FROM `" + table_name + "` WHERE `" + unique_key + "` = '" + obj->access->get(unique_key) + "'";
			}
			else
			{
				query_str = "DELETE FROM `" + table_name + "` WHERE `" + unique_key + "` = '" + unique_value + "'";
			}

			auto records = tegia::mysql::query(connection_name, query_str);

			_result.obj = obj;
			_result.code = records->code;
			_result.info = records->info;
			_result.affected_rows = records->get_affected_rows();

			delete records;
			return _result;
		};

		// 	/////////////////////////////////////////////////////////////////////////////////////////////
		// 	/**
		//  *	\brief		Выполняет запись массива объектов в БД
		//  *	\authors	Шпилевой Дмитрий
		//  *	\details
		//  */
		// 	/////////////////////////////////////////////////////////////////////////////////////////////

		template <class _Obj_>
		tegia::mysql::result<_Obj_> combo_create(
			const std::string &connection_name,
			std::vector<_Obj_*> *obj,
			int on_duplicate_flag = tegia::mysql::ON_DUPLICATE_ERROR)
		{

			tegia::mysql::result<_Obj_> _result;
			_result.obj = obj->at(0);
			std::string table_name = obj->at(0)->access->get_table_name();

			std::string mass_query = "INSERT INTO `" + table_name + "`";

			std::string columns = "";
			for (auto getters_iterator = obj->at(0)->access->fields_getter_begin(); getters_iterator != obj->at(0)->access->fields_getter_end(); ++getters_iterator)
			{
				// Формируем список столбцов
				if (getters_iterator->first == "_id")
				{
					continue;
				}
				columns += "`" + getters_iterator->first + "`,";
			}
			columns.pop_back();
			mass_query += " (" + columns + ") VALUES";

			for (auto &it : *obj)
			{
				std::string curr_value = "";
				for (auto getters_iterator = it->access->fields_getter_begin(); getters_iterator != it->access->fields_getter_end(); ++getters_iterator)
				{
					if (getters_iterator->first == "_id")
					{
						continue;
					}
					std::string val = tegia::mysql::strip(getters_iterator->second());
					curr_value += "'" + val + "',";
				}
				curr_value.pop_back();
				mass_query += " (" + curr_value + " ),";
			}
			mass_query.pop_back();
			mass_query += ";";

			// std::cout << mass_query << std::endl;
			auto records = tegia::mysql::query(connection_name, mass_query);

			_result.code = records->code;
			_result.info = records->info;

			delete records;
			return _result;
		};

	} // END namespace mysql
} // END namespace tegia

#endif