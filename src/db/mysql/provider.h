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
	#include <tegia/const.h>
	#include <tegia2/core/json.h>


////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// CONNECTION CLASS                                                                       //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

namespace tegia { 
namespace mysql {

class provider
{		
	private:
		std::unordered_map<std::string, tegia::mysql::connection *> _connections;
		
	public:
		provider();
		virtual ~provider();
		bool add(const std::string &name, nlohmann::json config);
		tegia::mysql::records * query(const std::string &name, const std::string &query, bool trace = false);
};


}   // end namespace mysql
}   // end namespace tegia



#endif 



