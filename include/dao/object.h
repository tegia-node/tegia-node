#ifndef H_DB_DAO_OBJECT
#define H_DB_DAO_OBJECT

// -------------------------------------------------------------------------------------- //
//                                      INCLUDES                                          //
// -------------------------------------------------------------------------------------- //

// STL 
	#include <string>
	#include <iostream>
	#include <sstream>


	#include "Property.h"
	#include "../core/core.h"
	#include "../core/json.h"

	#include "access.h"

	#define PROPDEF(Type, Name) protected: Type _##Name; public: Property2<Type> Name;

	#define PROPDEFF2(Type, Name, value, getter, setter) public: Type _##Name; public: Property2<Type> Name = Property2<Type>("Name", this, &(this->_##Name), value, getter, setter);
	
	
	// #define PROPDEFFF3(Type, Name, value, getter, setter) protected: Type _##Name; public: Property3<Type> Name = Property3<Type>("Name", this, &(this->_##Name), value, getter, setter);


	#define PROP(Type, Name) Prop<Type> Name = Prop<Type>(#Name, this);

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

class cObject
{   	
   //friend class db::mysql::connect;
   template<class T> friend class Property2;
                     friend class cAccess;

   PROPDEFF2(int, _id, 0, nullptr, nullptr)                     

   protected:
   // локальный идентификатор объекта
      //int __id;
   // setter from __id   
      //int _id(int id)
      //{
      //   this->__id = id;
      //   return this->__id;
      //};

      // Флаг, который устанавливается, если объект загружен из БД
      bool _is_load;  

    
   public:   

      cAccess * access;

      cObject(const std::string& objectType): access(new cAccess(objectType))                   
      {      
         /*this->access->add2("_id", 
             std::bind(&dao::cObject<_Access_>::_id, this, void), 
             std::bind(&dao::cObject<_Access_>::_id, this, std::placeholders::_1) ) ;*/
      // Инициализация property _id
         //this->_id.init("_id",&(this->__id), nullptr, nullptr);
         this->access->add("_id", &dao::cObject::_id, this);

      };

      virtual ~cObject() {
        delete this->access;
      };

  
      /**
          \brief Функция индексирования / переиндексирования объекта
      */
      void indexed(std::string action = "replace")
      {        };
      
     
      /**
          \brief Функция формирует json объекта
      */

      virtual nlohmann::json json()
      {
         nlohmann::json root;
         for (auto getters_iterator = this->access->fields_getter_begin(); 
                   getters_iterator != this->access->fields_getter_end();
                   getters_iterator++)
         {
            root[getters_iterator->first] = getters_iterator->second();
         }

         return root;
      };


};  // end class cObject











}   // end namespace dao


#endif 



