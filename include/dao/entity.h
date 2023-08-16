#ifndef H_DB_DAO_ENTITY
#define H_DB_DAO_ENTITY

// -------------------------------------------------------------------------------------- //
//                                      INCLUDES                                          //
// -------------------------------------------------------------------------------------- //

// STL 
   #include <string>
   #include <iostream>
   #include <sstream>

   #include "object.h"

//  -------------------------------------------------------------------------------------------- //
//  NAMESPASE DAO                                                                                //
/** -----------------------------------------------------------------------------------------------
    \brief    Пространство имен для классов работы с СУБД
    \details  
    \authors  Горячев Игорь 
*/
namespace dao { 



//  -------------------------------------------------------------------------------------------- //
//  CLASS DAO::COBJECT                                                                            //
/** -----------------------------------------------------------------------------------------------
    \brief   Класс обеспечивает установку соединения с СУБД и выполнение SQL-запросов
    \authors Горячев Игорь 
    \details 
*/

class Entity: public dao::cObject 
{
	protected:
		
	public:
		Entity(const std::string& entityType): dao::cObject(entityType)                   
		{

		};

		virtual ~Entity()
		{      };


		/**
			 \brief Функция индексирования / переиндексирования объекта
		*/
		void indexed(std::string action = "replace")
		{        };
		
		virtual std::string get_sql_values()
		{
			return "";
		};


};  // end class Entity





}   // end namespace dao


#endif 



