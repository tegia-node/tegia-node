#ifndef H_TEGIA_DB_MYSQL_CONNECTION
#define H_TEGIA_DB_MYSQL_CONNECTION

////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// INCLUDES                                                                               //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

//	STL 
	#include <mutex>

//	MySQL
	#include <mysql/mysql.h>
	#include <mysql/errmsg.h>

//	TEGIA
	#include <tegia/core/string.h>
	#include <tegia/node/logger.h>

	#include <tegia/db/mysql/mysql_const.h>
	#include <tegia/db/mysql/mysql_records.h>

////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// CONNECTION CLASS                                                                       //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

namespace tegia { 
namespace mysql {

class pool;

class connection
{
	friend class tegia::mysql::pool;
		
	protected:
		std::mutex _mutex;	

		std::string type;
		std::string host;
		int port;
		std::string user;
		std::string password;
		std::string dbname;
		MYSQL *hConnect;
		
	public:
		connection(
			const std::string &type, 
			const std::string &host, 
			const int port, 
			const std::string &user, 
			const std::string &password, 
			const std::string &dbname);
		virtual ~connection();

		std::pair<int, std::string> init(void);
		void close(void);

		records * query(const std::string &query, bool trace = false);
		records * requery(const std::string &query);
};


}   // end namespace mysql
}   // end namespace tegia



#endif 



