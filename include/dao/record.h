#ifndef H_DB_DAO_RECORD
#define H_DB_DAO_RECORD

// -------------------------------------------------------------------------------------- //
//                                      INCLUDES                                          //
// -------------------------------------------------------------------------------------- //

// STL 
   #include <string>
   #include <iostream>
   #include <sstream>
   #include <unordered_map>

   #include "../core/core.h"
   #include "../core/json.h"



//  -------------------------------------------------------------------------------------------- //
//  NAMESPASE DAO                                                                                //
/** -----------------------------------------------------------------------------------------------
    \brief    Пространство имен для классов работы с СУБД
    \details  
    \authors  Горячев Игорь 
*/
namespace dao { 



template<class _D_>
class baseProperty
{
   protected:
      _D_ *value;

   public:
      baseProperty(_D_ *value): value(value)
      {     };


      virtual ~baseProperty()
      {
         delete this->value;
         this->value = nullptr;
      };

      // GETTER
      virtual _D_ operator() (void) const 
      {
         return *(this->value);
      };

      // SETTER
      virtual _D_ operator() (const _D_ &value)
      {
         *(this->value) = value;
         return *(this->value);
      };

      virtual _D_ * getP()
      {
         return this->value;
      }

};

//  -------------------------------------------------------------------------------------------- //
//  CLASS DAO::RECORD                                                                            //
/** -----------------------------------------------------------------------------------------------
    \brief   Класс обеспечивает установку соединения с СУБД и выполнение SQL-запросов
    \authors Горячев Игорь 
    \details 
*/

class Record
{                     
   protected:

      std::unordered_map<std::string, baseProperty<std::string> *> _values;
 

   public:   


      Record()               
      {      

      };

      virtual ~Record() 
      {
         for(auto it = this->_values.begin(); it != this->_values.end(); it++)
         {
            //delete it->second->getP();
            delete it->second;
         }

      };


      virtual baseProperty<std::string> & field(const std::string &name)
      {
         auto it = this->_values.find(name);
         if(it != this->_values.end())
         {
            return *(it->second);
         }

         std::string err;
         err = "unknown property " + name; 
         throw err;
      };



      virtual bool add(const std::string &name, const std::string &value)
      {
         if(this->_values.find(name) != this->_values.end())
         {
            return false;
         }

         this->_values.insert( std::pair<std::string, baseProperty<std::string> *>(
               name, new baseProperty<std::string>(new std::string(value)) ) );

      };


      virtual Json::Value get2JSON()
      {
         Json::Value root;
         for (auto it = this->_values.begin(); it != this->_values.end(); it++)
         {
            root[it->first] = (*it->second)();
         }
         return root;
      };





};  // end class cObject











}   // end namespace dao


#endif 



