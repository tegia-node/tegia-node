#ifndef H_DB2_DAO_ACCESS
#define H_DB2_DAO_ACCESS

// -------------------------------------------------------------------------------------- //
//                                      INCLUDES                                          //
// -------------------------------------------------------------------------------------- //

// STL 
   #include <string>
   #include <iostream>
   #include <sstream>


   #include "Property.h"
   #include "../core/cast.h"

//  -------------------------------------------------------------------------------------------- //
//  NAMESPASE DAO                                                                                //
/** -----------------------------------------------------------------------------------------------
    \brief    Пространство имен для классов работы с СУБД
    \details  
    \authors  Горячев Игорь 
*/
namespace dao { 



class cAccess
{
   protected:

     std::map<std::string, std::function<std::string(void)> > fields_getter;
     std::map<std::string, std::function<std::string(const std::string&)> > fields_setter;

     std::string objectType;



   public:

    typedef typename std::map<std::string, std::function<std::string(const std::string&)> >::const_iterator setters_map_iterator;

    //  begin-итератор для мапа сеттеров
    setters_map_iterator fields_setter_begin()
    {
      return this->fields_setter.begin();
    };

    //  end-итератор для мапа сеттеров
    setters_map_iterator fields_setter_end()
    {
      return this->fields_setter.end();
    };

    typedef typename std::map<std::string, std::function<std::string(void)> >::const_iterator getters_map_iterator;
    //  begin-итератор для мапа сеттеров
    getters_map_iterator fields_getter_begin()
    {
      return this->fields_getter.begin();
    };

    //  end-итератор для мапа сеттеров
    getters_map_iterator fields_getter_end()
    {
      return this->fields_getter.end();
    };

    void set_table_name(const std::string& objectType)
    {
      this->objectType = objectType;
    };

    std::string get_table_name() const
    {
      return this->objectType;
    };



     cAccess(const std::string& objectType)
     {    
        this->objectType = objectType;
     };


     ~cAccess()
     {
        this->fields_getter.clear();
        this->fields_setter.clear();
     };


     std::string get(const std::string &name)
     {
        return fields_getter[name]();
     };

      void set(const std::string &name, const std::string &value)
      {
         if(this->fields_setter.contains(name) == true)
         {
            this->fields_setter[name](value);
         }
         else
         {
            // TODO !!!

            std::cout << "not found setter for [" << objectType << "::" << name << "] \n";
            exit(0);
         }
      };



     template<class T, class valC>
     void add(const std::string &name, Property2<valC> T::* fnc, T *that)
     {
        auto fn = std::bind(fnc, that);

     // Функция getter  
        std::function<std::string(void)> fn_getter = [=]()
        {
          std::ostringstream out;
          out<<fn()();
          return out.str();           
        };

     // Функция setter
        std::function<std::string(const std::string&)> fn_setter = [=](const std::string &value)
        {
          std::ostringstream out;
          out<<fn()( core::cast<valC>(value) );
          return out.str();           
        };

        //fields_getter.insert(std::pair<std::string, std::function<std::string(void)> >(name, fn_getter) );
        fields_getter.insert( std::make_pair(name, fn_getter) );
        //fields_setter.insert(std::pair<std::string, std::function<std::string(const std::string&)> >(name, fn_setter) );
        fields_setter.insert( std::make_pair(name, fn_setter) );
     };



     template<class T, class valC>
     void add2(const std::string &name, std::function<valC(void)> fng, std::function<valC(valC)> fns)
     {
        //auto fn = std::bind(fnc, that);

     // Функция getter  
        std::function<std::string(void)> fn_getter = [=]()
        {
          return core::cast<std::string>(fng());           
        };

     // Функция setter
        std::function<std::string(const std::string&)> fn_setter = [=](const std::string &value)
        {
          return core::cast<std::string>(fns( core::cast<valC>(value) ));            
        };

        fields_getter.insert(std::pair<std::string, std::function<std::string(void)> >(name, fn_getter) );
        fields_setter.insert(std::pair<std::string, std::function<std::string(const std::string&)> >(name, fn_setter) );
     };

};  // end class cAccess




}   // end namespace dao


#endif 



