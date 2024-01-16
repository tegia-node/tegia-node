#include "connection.h"

//	LOGGER
#include "../../node/logger.h"
#undef _LOG_LEVEL_
#define _LOG_LEVEL_ _LOG_NOTICE_
#include "../../node/log.h"
//	LOGGER

namespace tegia {
namespace mysql {

	// ///////////////////////////////////////////////////////////////////////////////////////////// //
	//                                                                                               //
	//   DB_CONNECT CLASS                                                                            //
	//                                                                                               //
	// ///////////////////////////////////////////////////////////////////////////////////////////// //


	//////////////////////////////////////////////////////////////////////////////////////////////////
	/**
		\brief Конструктор
		\authors Горячев Игорь
		
		input:
			string host      - имя хоста БД
			const int por    - номер порта
			string user      - имя пользователя, от которого подключаемся к БД
			string passsword - пароль пользователя, от которого подключаемся к БД
			string dbname    - имя базы данных, к которой выполняем подключение
		
		return:
			void - конструктор объекта не возвращает результата
		
		info:
			При создании объекта инициализируем внутренние переменные и устанавливаем соединение с БД
	*/
	//////////////////////////////////////////////////////////////////////////////////////////////////

	connection::connection(
		const std::string &_name,
		const std::string &_type,
		const std::string &_host,
		const int _port,
		const std::string &_user,
		const std::string &_password
	):	name(_name),
		type(_type),
		host(_host),
		port(_port),
		user(_user),
		password(_password),
		hConnect(nullptr)
	{ };

	//////////////////////////////////////////////////////////////////////////////////////////////////
	/**
		\brief Деструктор

	*/
	//////////////////////////////////////////////////////////////////////////////////////////////////

	connection::~connection()
	{
		this->close();
	};


	//////////////////////////////////////////////////////////////////////////////////////////////////
	/** 
		\brief   Функция установки соединения с СУБД
		\authors Горячев Игорь 

		\return  пара:
			- код ошибки (или 0 если соединение успешно установлено);
			- текстовое описание ошибки.
		\details Функция пытается установить соединение с СУБД и в случае ошибки возвращает информацию о причинах неудачи
	*/
	//////////////////////////////////////////////////////////////////////////////////////////////////

	std::tuple<int, std::string> connection::connect(void)
	{
		this->_mutex.lock();

		std::cout << "name     = " << this->name << std::endl;
		std::cout << "type     = " << this->type << std::endl;
		std::cout << "host     = " << this->host << std::endl;
		std::cout << "password = " << this->password << std::endl;
		std::cout << "port     = " << this->port << std::endl;
		std::cout << "user     = " << this->user << std::endl;


		if (this->hConnect == nullptr)
		{
			this->hConnect = mysql_init(this->hConnect);
			if (this->hConnect == nullptr)
			{
				this->_mutex.unlock();
				return std::move(std::make_tuple<int, std::string>(DB_ERROR_INIT, "insufficient memory to allocate a new object"));
			}

			mysql_options(this->hConnect, MYSQL_READ_DEFAULT_GROUP, "tegia platform");
			mysql_options(this->hConnect, MYSQL_SET_CHARSET_NAME, "utf8mb4");

			if (type == "mysql")
			{
				mysql_options(this->hConnect, MYSQL_INIT_COMMAND, "SET NAMES 'utf8mb4' COLLATE 'utf8mb4_general_ci';");
			}

			auto res = mysql_real_connect(
				this->hConnect,
				this->host.c_str(),
				this->user.c_str(),
				this->password.c_str(),
				//this->dbname.c_str(),
				NULL,
				this->port, NULL, 0);
			if (!res)
			{
				auto res_errno = mysql_errno(this->hConnect);
				auto res_error = (const char *)mysql_error(this->hConnect);
				this->close();

				this->_mutex.unlock();
				return std::make_tuple<int, std::string>(DB_ERROR + res_errno, res_error);
			}
			else
			{
				mysql_set_character_set(this->hConnect, "utf8mb4");
			}
		}

		this->_mutex.unlock();
		return std::make_tuple<int, std::string>(0, "ok");
	};


	//////////////////////////////////////////////////////////////////////////////////////////////////
	/** 
	 
		Функция закрытия соединения с БД

	*/
	//////////////////////////////////////////////////////////////////////////////////////////////////


	void connection::close(void)
	{
		if (this->hConnect != nullptr)
		{
			mysql_close(this->hConnect);
			this->hConnect = nullptr;
		}
	};


	//////////////////////////////////////////////////////////////////////////////////////////////////
	/** 

		\brief   Функция выполнения SQL-запроса к БД
	
	*/
	//////////////////////////////////////////////////////////////////////////////////////////////////

	tegia::mysql::records *connection::query(const std::string &dbname, const std::string &query, bool trace)
	{
		// std::cout << "query = " << query << std::endl;

		tegia::mysql::records * result = new tegia::mysql::records(query, 0);
		
		//
		// Если нет установленного соединения с СУБД
		//

		if (this->hConnect == nullptr)
		{
			result->code = DB_ERROR;
			result->info = "connection is null";
			return result;
		}

		if(this->dbname != dbname && this->type == "mysql")
		{
			this->dbname = dbname;
			mysql_select_db(this->hConnect,this->dbname.c_str());
		}

		//
		// Выполняем SQL-запрос
		//

		int code = mysql_query(this->hConnect, query.c_str());

		//
		// Запрос выполнен успешно
		//

		if(code == 0)
		{
			//
			// Статистика запроса
			//

			result->_last_insert_id = mysql_insert_id(this->hConnect);
			result->_affected_rows = mysql_affected_rows(this->hConnect);
			result->code = 200;
			result->info = "ok";

			//
			// Данные запроса
			//

			auto query_data = mysql_store_result(this->hConnect);
			if(query_data != nullptr)
			{
				result->setRecNum(mysql_num_rows(query_data));

				MYSQL_FIELD *field;
				MYSQL_ROW row;

				while (field = mysql_fetch_field(query_data))
				{
					result->add_field(field);
				}

				int i = 0;
				while ((row = mysql_fetch_row(query_data)))
				{
					result->add_row(row, i);
					i++;
				}				
			}
			mysql_free_result(query_data);
			return result;
		}

		//
		// Обрабатываем ошибки
		//

		result->code = DB_ERROR + mysql_errno(this->hConnect);
		result->info = (const char *)mysql_error(this->hConnect);

		switch (result->code)
		{
			//
			// Error: 2002 CR_CONNECTION_ERROR
			//

			case DB_ERROR + CR_CONNECTION_ERROR:
			{
				//LERROR("MYSQL CR_CONNECTION_ERROR Attempt to restore connection");
				delete result;
				return this->requery(dbname,query);
			}
			break;

			//
			// Error: 2003 CR_CONN_HOST_ERROR
			//

			case DB_ERROR + CR_CONN_HOST_ERROR:
			{
				//LERROR("MYSQL CR_CONN_HOST_ERROR Attempt to restore connection");
				delete result;
				return this->requery(dbname,query);
			}
			break;

			//
			// Error: 2006 CR_SERVER_GONE_ERROR
			//

			case DB_ERROR + CR_SERVER_GONE_ERROR:
			{
				//LERROR("MYSQL CR_SERVER_GONE_ERROR Attempt to restore connection");
				delete result;
				return this->requery(dbname,query);
			}
			break;

			//
			// Error: 2013 CR_SERVER_LOST
			//

			case DB_ERROR + CR_SERVER_LOST:
			{
				//LERROR("MYSQL CR_SERVER_LOST Attempt to restore connection");
				delete result;
				return this->requery(dbname,query);
			}
			break;

			//
			// Error: 4031 ER_CLIENT_INTERACTION_TIMEOUT
			//

			case DB_ERROR + 4031:
			{
				//LERROR("MYSQL ER_CLIENT_INTERACTION_TIMEOUT Attempt to restore connection");
				delete result;
				return this->requery(dbname,query);
			}
			break;

			// 
			// Error: [1025] ER_ERROR_ON_RENAME
			//

			case DB_ERROR + 1025:
			{
				//LERROR("MYSQL ER_ERROR_ON_RENAME");
				delete result;
				return this->query(this->dbname,query,trace);
			}
			break;

			default:
			{
				//LERROR("MYSQL QUERY ERROR \n	" << result->code << "\n	" << result->info << "\n	[" << result->query << "]");
				this->_mutex.unlock();

				if(trace == true)
				{
					std::cout << "result->code = " << result->code << "; query = [" << query << "]"<< std::endl;
				}
				
				return result;
			}
			break;
		}
		
		return result;
	};



	//////////////////////////////////////////////////////////////////////////////////////////////////
	/** 
		\brief   Функция пытается восстановить соединение и повторить запрос
	
	*/
	//////////////////////////////////////////////////////////////////////////////////////////////////


	tegia::mysql::records * connection::requery(const std::string &dbname, const std::string &query)
	{
		//
		// Пытаемся восстановить соединение
		//

		mysql_close(this->hConnect);
		this->hConnect = nullptr;

		auto [code,info] = this->connect();
		if (code != 0)
		{
			//LERROR("Restore connection [error]");
			// TODO: Что делать в этой ситуации?
			return nullptr;
		}
		else
		{
			//LERROR("Restore connection [ok]");

			if(this->dbname != dbname && this->type == "mysql")
			{
				this->dbname = dbname;
				mysql_select_db(this->hConnect,this->dbname.c_str());
			}

			return this->query(this->dbname,query);
		}
	};


	//////////////////////////////////////////////////////////////////////////////////////////////////
	/** 
		\brief   Функция пытается восстановить соединение и повторить запрос
	
	*/
	//////////////////////////////////////////////////////////////////////////////////////////////////


	std::string connection::strip(const std::string &input)
	{
		char* from = new char[strlen(input.c_str()) * 3 + 1];
		mysql_real_escape_string(this->hConnect, from, input.c_str(), input.length());
		std::string retStr(from);
		delete [] from;
		return retStr;
		
		/*
		for(auto it = this->_connections.begin(); it != this->_connections.end();++it)
		{
			if( (*it)->hConnect != nullptr)
			{

			}
		}
		LERROR("Пул " << this->name << ". Нет соединений в пуле");
		return "";
		*/
	};


}	// end namespace mysql
}	// end namespace tegia
