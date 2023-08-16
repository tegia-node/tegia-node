#ifndef H_TEGIA_DB_MYSQL_POOL
#define H_TEGIA_DB_MYSQL_POOL

////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// INCLUDES                                                                               //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

#include <tegia/node/logger.h>
#include <tegia/const.h>
#include <tegia/db/mysql/mysql_records.h>

#include "mysql_connection.h"
	
////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// POOL CLASS                                                                             //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

namespace tegia { 
namespace mysql {

class pool
{
	protected:
		//! Число попыток установки соединения  
		int max_try = 10;
		//! Текущее число проведенных попыток установки соединения
		int curr_try = 0;   

		//! максимально число соединнеий в пуле   
		int max_conn;
		//! текущее число соединений в пуле
		int curr_conn;
		//! имя пула соединений
		std::string name;   

		std::string type;
		//! массив установленных соединений
		std::vector<tegia::mysql::connection *> _connections;
			
		// 
		std::string host;
		int         port;
		std::string user;
		std::string password;
		std::string dbname;

	public:  

		pool(
			const std::string &name, 
			const std::string &type,
			const int max_connections,
			const std::string &host, 
			const int port,
			const std::string &user, 
			const std::string &password, 
			const std::string &dbname = ""
		);
		virtual ~pool();

		std::pair<int,std::string> add_connection(void);
		records * query(const std::string &query,bool trace = false);
		std::string strip(const std::string &input);
		void clear(void);

};


}   // namespace mysql
}   // namespace tegia


#endif 



