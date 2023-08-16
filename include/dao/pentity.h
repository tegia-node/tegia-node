#ifndef H_DB_DAO_PIBENTITY
#define H_DB_DAO_PIBENTITY

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

class PIBEntity: public dao::cObject 
{   	
   PROPDEFF2(int, _gid, 0, nullptr, nullptr) 
   PROPDEFF2(Json:Value, _actions, 0, nullptr, nullptr) 

   protected:
    
   public:   
      PIBEntity(const std::string& entityType): dao::cObject(entityType)                   
      {      

      };

      virtual ~PIBEntity()
      {      };
      


};  // end class Entity




}   // end namespace dao


#endif 



