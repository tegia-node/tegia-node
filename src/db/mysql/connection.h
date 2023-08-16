#ifndef H_TEGIA_DB_MYSQL_CONNECTION
#define H_TEGIA_DB_MYSQL_CONNECTION

////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// INCLUDES                                                                               //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

//	STL 
	#include <tuple>

//	MySQL
	#include <mysql/mysql.h>
	#include <mysql/errmsg.h>

//	TEGIA
	#include <tegia/core/string.h>

	#include <tegia2/db/mysql/const.h>
	#include <tegia2/db/mysql/records.h>



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

		std::string name;
		std::string type;
		std::string host;
		int port;
		std::string user;
		std::string password;
		std::string dbname;
		MYSQL *hConnect;
		
	public:
		connection(
			const std::string &_name, 
			const std::string &_type, 
			const std::string &_host, 
			const int _port, 
			const std::string &_user, 
			const std::string &_password, 
			const std::string &_dbname);
		virtual ~connection();

		std::tuple<int, std::string> connect(void);
		void close(void);

		records * query(const std::string &query, bool trace = false);
		records * requery(const std::string &query);
};


}   // end namespace mysql
}   // end namespace tegia



#endif 



