#include <tegia/db/mysql/mysql.h>
#include "../../threads/data.h"

namespace tegia {
namespace mysql {


/////////////////////////////////////////////////////////////////////////////////////////////
/**
	\brief		Выполнение SQL-запроса
	\authors	Горячев Игорь 
	\details
*/
/////////////////////////////////////////////////////////////////////////////////////////////


tegia::mysql::records * query(const std::string &context, const std::string &query, bool trace)
{
	// return tegia::threads::data->query(context,query,trace);
	return tegia::threads::data->mysql_provider->query(context,query,trace);
};



std::string strip(const std::string &input)
{
	// return tegia::threads::data->mysql_strip(input);
	return tegia::threads::data->mysql_provider->strip(input);
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
	auto res = tegia::threads::data->mysql_provider->query(context,"START TRANSACTION;");
	auto code = res->code;
	delete res;
	return code;
};


int commit(const std::string &context)
{
	auto res = tegia::threads::data->mysql_provider->query(context,"COMMIT;");
	auto code = res->code;
	delete res;
	return code;
};


int rollback(const std::string &context)
{
	auto res = tegia::threads::data->mysql_provider->query(context,"ROLLBACK;");
	auto code = res->code;
	delete res;
	return code;
};


} // END namespace transaction
} // END namespace mysql
} // END namespace tegia


