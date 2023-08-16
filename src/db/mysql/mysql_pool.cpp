#include "mysql_pool.h"

namespace tegia { 
namespace mysql {

///////////////////////////////////////////////////////////////////////////////////////////////////
/** 
	\brief   Конструктор
	\authors Горячев Игорь 
	\details    

	\param name             имя пула соединений
	\param max_connections  максимальное число соединений в пуле
	\param host             имя хоста, где расположен инстанс СУБД
	\param port             порт, по которому осуществляется подключение к СУБД
	\param user             имя пользователя от которого устанавливается соединение
	\param password         пароль пользователя от которого устанавливается соединение
	\param dbname           имя БД, к которой устанавливается соединение. dbname может быть пустым, 
									тогда устанавливается соединение со всем инстансом с учетом прав доступа, 
									имеющихся у пользователя
*/ 
///////////////////////////////////////////////////////////////////////////////////////////////////

pool::pool(
	const std::string &name, 
	const std::string &type,
	const int max_connections,
	const std::string &host, 
	const int port,
	const std::string &user, 
	const std::string &password, 
	const std::string &dbname)
{

	this->max_conn = max_connections;   
	this->curr_conn = 0;
	this->_connections.reserve(this->max_conn);

	this->host = host;
	this->port = port;
	this->user = user;
	this->password = password;
	this->dbname = dbname; 

	this->name = name;
	this->type = type;
 
	for(int i = 0; i < this->max_conn; i++)
	{
		auto ret = this->add_connection();

		switch(ret.first)
		{
			case 0:
			{
				// Соединение успешно установлено, все ок
			}
			break;

			case DB_ERROR_INIT:
			{

			}
			
			// Коды ошибок mysql_real_connect()
			case DB_ERROR + CR_SERVER_GONE_ERROR:
			case DB_ERROR + CR_CONN_HOST_ERROR:
			case DB_ERROR + CR_CONNECTION_ERROR:
			case DB_ERROR + CR_IPSOCK_ERROR:
			case DB_ERROR + CR_OUT_OF_MEMORY:
			case DB_ERROR + CR_SOCKET_CREATE_ERROR:
			case DB_ERROR + CR_UNKNOWN_HOST:
			case DB_ERROR + CR_VERSION_ERROR:
			case DB_ERROR + CR_NAMEDPIPEOPEN_ERROR:
			case DB_ERROR + CR_NAMEDPIPEWAIT_ERROR:
			case DB_ERROR + CR_NAMEDPIPESETSTATE_ERROR:
			{
				/* 
					Причина завершения работы Платформы с кодом ошибки: такого рода ошибки при установке 
					соединения в подавляющем большинстве случаев означают, что что-то не так с MySQL-сервером
					или сетью. Сами по себе такие проблемы не исправляются, поэтому правильнее "закрыть" данный
					пул и исправить внешние ошибки с сетью или MySQL. 
				*/          
				LERROR( "[" << DB_ERROR << "] Пул не создан")
				this->clear();	
			}
			break;

			case DB_ERROR + CR_SERVER_LOST:
			{
				LERROR( "[" << DB_ERROR << "] Превышено число попыток соединения")
			}
			break;

			default:
			{
				LERROR( "[" << DB_ERROR << "] Превышено число попыток соединения")
			}
			break;
		} // end  switch(ret.first)
	} // end for(int i = 0; i < this->max_conn; i++)
};

///////////////////////////////////////////////////////////////////////////////////////////////////
/** 
	\brief   Деструктор
	\authors Горячев Игорь 
	\details    
*/ 
///////////////////////////////////////////////////////////////////////////////////////////////////

pool::~pool()
{
	this->clear();
};

///////////////////////////////////////////////////////////////////////////////////////////////////
/** 
	\brief   Закрываются все открытые соединения в пуле
	\authors Горячев Игорь 
	\details    
*/ 
///////////////////////////////////////////////////////////////////////////////////////////////////

void pool::clear(void)
{
	for(auto it = this->_connections.begin();
		it != this->_connections.end();
		++it)
	{
		(*it)->close();
		delete *it;
	}

	this->_connections.clear();
	this->curr_conn = 0;  
};

///////////////////////////////////////////////////////////////////////////////////////////////////
/** 
	\brief   Функция создания пула соединений
	\authors Горячев Игорь   
	\details Пытаемся 10 раз установить соединение с БД. Если после 10 раз установить 
				соединение не удалось, то возвращаем ошибку  
*/ 
///////////////////////////////////////////////////////////////////////////////////////////////////


std::pair<int,std::string> pool::add_connection(void)
{
	std::pair<int,std::string> ret;
	
	for(int i = 0; i < 10; i++)
	{
		tegia::mysql::connection * db = new tegia::mysql::connection(this->type, this->host, this->port, this->user, this->password, this->dbname);
		ret = db->init();

		if(ret.first != 0)
		{
			LERROR( "Пул [" << this->name << "] [" << ret.first << "] " << ret.second );  
			delete db;
		}
		else
		{
			this->_connections.push_back(db);
			this->curr_conn++;

			LNOTICE( "Пул [" << this->name << "] Соединение с БД установлено" );
			return std::make_pair<int,std::string>(0,"ok");
		}
	}
	return ret;
};


///////////////////////////////////////////////////////////////////////////////////////////////////
/** 
	\brief   Выполнение SQL-запроса
	\authors Горячев Игорь 
	\details    
*/ 
///////////////////////////////////////////////////////////////////////////////////////////////////

records * pool::query(const std::string &query, bool trace)
{
	while(true)
	{
		for(auto it = this->_connections.begin(); it != this->_connections.end();++it)
		{
			if( (*it)->_mutex.try_lock() )
			{
				return (*it)->query(query,trace);
			}
		}   

		/*
			Если нет не одного свободного соединения, то необходимо написать об этом в журнал. 
			В системе анализа журнала должен стоять обработчик на это событие. Есои такое сообщение 
			встречается часто, то необходимо увеличить число соединений в пуле.
		*/  

		LWARNING("Пул [" << this->name << "] Нет свободных соединений в пуле");
	}
};







// -------------------------------------------------------------------------------------------- //
// Подготовка строки для записи в БД
// -------------------------------------------------------------------------------------------- //


std::string pool::strip(const std::string &input)
{
	for(auto it = this->_connections.begin(); it != this->_connections.end();++it)
	{
		if( (*it)->hConnect != nullptr)
		{
			char* from = new char[strlen(input.c_str()) * 3 + 1];
			mysql_real_escape_string((*it)->hConnect, from, input.c_str(), input.length());
			std::string retStr(from);
			delete [] from;
			return retStr;
		}
	}
	LERROR("Пул " << this->name << ". Нет соединений в пуле");
	return "";
};




}   // end namespace mysql
}   // end namespace tegia



