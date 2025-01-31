#include <tegia/db/mysql/mysql.h>
// #include "../../threads/data.h"
#include "../../threads/thread_t.h"

#include <mysql/mysql.h>
#include <mysql/errmsg.h>

namespace tegia {
namespace mysql {


/////////////////////////////////////////////////////////////////////////////////////////////
/**
	\brief		Выполнение SQL-запроса
	\authors	Горячев Игорь 
	\details
*/
/////////////////////////////////////////////////////////////////////////////////////////////


void init()
{
	if (mysql_library_init(0, nullptr, nullptr)) 
	{
		std::cerr << "Could not initialize MySQL library!" << std::endl;
		exit(1);
	}
};



tegia::mysql::records * query(const std::string &context, const std::string &query, bool trace)
{
	// std::cout << "mysql query run" << std::endl;
	return tegia::threads::thread->mysql_provider()->query(context,query,trace);
};


std::string strip(const std::string &input)
{
	return tegia::threads::thread->mysql_provider()->strip(input);
};


std::string date(const std::string &datetime)
{
	if(datetime == "") return "NULL";
	return "'" + datetime + "'";
};


std::tuple<int,table_t*> table(const std::string &context, const std::string &name)
{
	std::string query = "SHOW TABLE STATUS WHERE Name = '" + tegia::mysql::strip(name) + "';";
	auto res = tegia::threads::thread->mysql_provider()->query(context,query,false);
	
	if(res->code != 200)
	{
		int code = res->code;
		delete res;	
		return std::make_tuple(code,nullptr);
	}

	tegia::mysql::table_t * table = new tegia::mysql::table_t();
	table->data = res->json()[0];
	table->comment = table->data["Comment"].get<std::string>();
	table->name = table->data["Name"].get<std::string>();
	delete res;

	return std::make_tuple(200,table);
};


}	// END namespace mysql
}	// END namespace tegia


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////


namespace tegia{
namespace mysql{
namespace transaction{


int start(const std::string &context)
{
	auto res = tegia::threads::thread->mysql_provider()->query(context,"START TRANSACTION;");
	auto code = res->code;
	delete res;
	return code;
};


int commit(const std::string &context)
{
	auto res = tegia::threads::thread->mysql_provider()->query(context,"COMMIT;");
	auto code = res->code;
	delete res;
	return code;
};


int rollback(const std::string &context)
{
	auto res = tegia::threads::thread->mysql_provider()->query(context,"ROLLBACK;");
	auto code = res->code;
	delete res;
	return code;
};


} // END namespace transaction
} // END namespace mysql
} // END namespace tegia


