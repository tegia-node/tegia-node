#include <tegia/db/mysql/mysql.h>
#include "mysql_provider.h"

namespace tegia {
namespace mysql {


	/////////////////////////////////////////////////////////////////////////////////////////////
	/**
		\brief		Создает пул соединений с СУБД
		\authors	Горячев Игорь 
		\details
	*/
	/////////////////////////////////////////////////////////////////////////////////////////////


	bool connect(int thread_count, nlohmann::json config)
	{
		tegia::mysql::provider::add(thread_count, config);
		return true;
	};


	/////////////////////////////////////////////////////////////////////////////////////////////
	/**
		\brief		Выполняет экранирование строки для SQL-запроса
		\authors	Горячев Игорь 
		\details
	*/
	/////////////////////////////////////////////////////////////////////////////////////////////


	
	std::string strip(const std::string& query)
	{
		return tegia::mysql::provider::get("core")->strip(query);
	};
	


	/////////////////////////////////////////////////////////////////////////////////////////////
	/**
		\brief		Извлекает имя и значение ключа при возникновении ошибки Duplicate entry
		\authors	Горячев Игорь 
		\details
	*/
	/////////////////////////////////////////////////////////////////////////////////////////////
	
	std::tuple<bool,std::string,std::string> get_duplicate_info(const std::string &info)
	{
		//
		// Example:
		// Duplicate entry '7a1081f27470b9e2a9ef6d440af57791' for key 'persons_base.md5'
		//

		std::size_t found1 = info.find("Duplicate entry '");
		std::size_t found2 = info.find("' for key '");

		if(found1 != std::string::npos && found2 != std::string::npos)
		{
			std::string _value = info.substr(17, found2-17);
			std::string _key   = info.substr(found2 + 11);
			_key.pop_back();

			// std::cout << "this->dup_value = [" << this->dup_value << "]" << std::endl;
			// std::cout << "this->dup_key   = [" << this->dup_key << "]" << std::endl;

			return std::make_tuple(true,_key, _value);
		}

		std::cout << _ERR_TEXT_ << "not found duplicate_info on " << info << std::endl;
		LWARNING("not found duplicate_info on " << info);
		return std::make_tuple(false,"","");
	};


	/////////////////////////////////////////////////////////////////////////////////////////////
	/**
		\brief		Выполнение SQL-запроса
		\authors	Горячев Игорь 
		\details
	*/
	/////////////////////////////////////////////////////////////////////////////////////////////


	tegia::mysql::records * query(const std::string &connection_name, const std::string &query, bool trace)
	{
		return tegia::mysql::provider::get(connection_name)->query(query,trace);
	};





}	// END namespace mysql
}	// END namespace tegia

