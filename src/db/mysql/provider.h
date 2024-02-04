#ifndef H_TEGIA_DB_MYSQL_PROVIDER
#define H_TEGIA_DB_MYSQL_PROVIDER

////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// INCLUDES                                                                               //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

//	STL
	#include <string>
	#include <unordered_map>
	#include <iostream>

//	MySQL
	#include <mysql/mysql.h>
	#include <mysql/errmsg.h>

	#include "connection.h"

//	TEGIA
	#include <tegia/core/const.h>
	#include <tegia/core/json.h>


////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// CONNECTION CLASS                                                                       //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

namespace tegia { 
namespace mysql {


struct context_t
{
	std::string dbname;
	std::string connection_name;
	tegia::mysql::connection * connection;
};


class provider
{		
	private:
		std::unordered_map<std::string, tegia::mysql::connection *> _connections;
		std::unordered_map<std::string, context_t> _contexts;
		
	public:
		provider();
		virtual ~provider();
		bool add_connection(const std::string &name, nlohmann::json config);
		bool add_database(const std::string &name, nlohmann::json config);
		tegia::mysql::records * query(const std::string &name, const std::string &query, bool trace = false);
		std::string strip(const std::string &input);
};


}   // end namespace mysql
}   // end namespace tegia



#endif 



