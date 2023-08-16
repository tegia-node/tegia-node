#include <tegia2/db/mysql/mysql.h>
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


	tegia::mysql::records * query(const std::string &connection_name, const std::string &query, bool trace)
	{
		return tegia::threads::data->query(connection_name,query,trace);
	};





}	// END namespace mysql
}	// END namespace tegia

