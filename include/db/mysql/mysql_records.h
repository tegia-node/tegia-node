#ifndef H_TEGIA_DB_MYSQL_RECORD
#define H_TEGIA_DB_MYSQL_RECORD

////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// INCLUDES                                                                               //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

//	MySQL
	#include <mysql/mysql.h>
	#include "mysql_const.h"

//	STL 
	#include <string>
	#include <iostream>

	#include <utility>
	#include <unordered_map>
	#include <vector>

	#include <tegia/core/json.h>

////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// RECORDS CLASS                                                                          //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

namespace tegia { 
namespace mysql {

	class record_info;

	/** 
		-----------------------------------------------------------------------------------------------
		CLASS TEGIA::MYSQL::RECORD
		-----------------------------------------------------------------------------------------------
		
		\brief   Класс представляет собой обертку над результатами выполнения SQL-запроса. 
		\authors Горячев Игорь 
		\details 

		Класс представляет собой обертку над результатами выполнения SQL-запроса. Если SQL-запрос 
		предполагает объединение двух или более таблиц, то в результатах запроса может получиться два 
		поля данных с одинаковым наименованием, но из разных таблиц. В libmysql эта ситуация 
		разрешается таким образом:
		- результат запроса доступен по числовому индексу;
		- существует отдельная функция, которая позволяет получить информацию о поле данных.

		Данная реализация предлагает в качестве индекса поля данных использовать текстовую строку 
		следующего вида: {имя таблицы}::{имя поля данных}. Соответственно, получить результат запроса 
		можно указав полное наменование таблицы и полное наименование поля данных.

		\code 
			auto ret = tegia::mysql::query(connection_name,"SELECT * FROM _users;");
			std::cout << "[" << ret->get("_users::login",0) << "]" << std::endl;   
			delete ret; 
		\endcode

		Важно: 
		- Для полей, получаемых в результате вычислений имя таблицы остается пустым:
		\code 
			auto ret = tegia::mysql::query(connection_name,"SELECT count(*) FROM _users;");
			std::cout << "[ ::count(*) = " << ret->get("::count(*)",0) << "]" << std::endl;
			delete ret;
		\endcode

		- Если в SQL-запросе для поля определен пседоним, то для доступа к результатам запроса так же 
			необходимо использовать псевдоним поля:
		\code 
			auto ret = tegia::mysql::query(connection_name,"SELECT count(*) as count FROM _users;");
			std::cout << "[ ::count = " << ret->get("::count",0) << "]" << std::endl;
			delete ret;
		\endcode

	*/


	class records
	{     
		friend class connection;
		friend class pool;

		protected:

			int fields_num = 0;
			int rec_num = 0;

			std::vector<std::unordered_map<std::string, std::string> > recs;
			std::unordered_map<std::string, record_info*> fields_name;
			std::map<int, record_info*> fields_offset;

			std::string query = "";
			int _last_insert_id = 0;
			int _affected_rows = 0;

			records(const std::string& query, const int& last_insert_id);
			void setRecNum(int recNum);
			void add_field(MYSQL_FIELD * field);
			void add_row(const MYSQL_ROW &row, const int &offset);

		public:

			int code;
			std::string info;

			virtual ~records();
			std::string get(const std::string &key, const int offset);
			nlohmann::json json();
			int count();
			int getLastInsertId() const;
			int get_affected_rows() const;


	};  // end class records

}   // end namespace mysql
}   // end namespace tegia


#endif 



