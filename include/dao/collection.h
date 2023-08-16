#ifndef H_DAO_COLLECTION
#define H_DAO_COLLECTION

// -------------------------------------------------------------------------------------- //
//                                      INCLUDES                                          //
// -------------------------------------------------------------------------------------- //

// STL 
   #include <string>
   #include <iostream>
   #include <sstream>
   #include <unordered_map>


   #include "../../core.h"
   #include "../../jsonextension.h"



//  -------------------------------------------------------------------------------------------- //
//  NAMESPASE DAO                                                                                //
/** -----------------------------------------------------------------------------------------------
    \brief    Пространство имен для классов работы с СУБД
    \details  
    \authors  Горячев Игорь 
*/
namespace dao { 



//  -------------------------------------------------------------------------------------------- //
//  CLASS DAO::COLLECTION                                                                            //
/** -----------------------------------------------------------------------------------------------
    \brief   
    \authors Горячев Игорь 
    \details 
*/
/*template <class T>
class Collection2 
{
   protected:

      std::vector<T*> _objects;

   public:

      Collection2(){};

      virtual ~Collection2()
      {
         for(auto it = this->_objects.begin(); it != this->_objects.end(); it++)
         {
            delete *it;
         }
      };



      T* const obj(int offset)
      {
         if(offset > this->_objects.size() && offset <= 0)
         {
            throw std::string("Объект с таким индексом не найден");
         }

         return this->_objects.at(offset);
      };



      T* const add(T* object)
      {
         if(object == nullptr)
         {
            object = new T();
         }
         this->_objects.push_back(object);
         return this->_objects.back();
      };




      
};  // end class collection


*/


}   // end namespace dao


#endif 



