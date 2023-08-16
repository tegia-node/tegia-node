#ifndef H_CORE_CAST
#define H_CORE_CAST
// -------------------------------------------------------------------------------------- //
//                                      INCLUDES                                          //
// -------------------------------------------------------------------------------------- //

#include <iostream>
#include <sstream>
#include <codecvt>
#include <string>
#include <locale>

#include <tegia/core/json.h>


template<class Facet>
struct deletable_facet : Facet
{
	template<class ...Args>
	deletable_facet(Args&& ...args) : Facet(std::forward<Args>(args)...) {}
	~deletable_facet() {}
};

//////////////////////////////////////////////////////////////////////////////////////////
//
// Шаблон функции для преобразования типов
//
//////////////////////////////////////////////////////////////////////////////////////////


namespace core{

template<class TO, class IN>
TO cast(IN value)
{
   // std::cout << "template<class TO, class IN>cast()" << std::endl;

   // положим, что from_val и to_val - это, соответственно преобразуемое
   TO to_val;

   // значение и приемник результата преобразования
   std::ostringstream o_str;
   o_str << value;

   std::istringstream i_str(o_str.str());
   i_str >> to_val;

   return to_val;
};

}  // end namespace core


//////////////////////////////////////////////////////////////////////////////////////////
//
//  std::string  ->  { TO }
//
//////////////////////////////////////////////////////////////////////////////////////////

namespace core
{
   template<> std::string cast<std::string,const std::string &>(const std::string &value);
   template<> std::string cast<std::string,std::string>(std::string value);

   template<> int cast<int,const std::string &>(const std::string &value);
   template<> long int cast<long int,const std::string &>(const std::string &value);
   template<> unsigned long int cast<unsigned long int,const std::string &>(const std::string &value);
   template<> long long int cast<long long int,const std::string &>(const std::string &value);
   template<> unsigned long long int cast<unsigned long long int,const std::string &>(const std::string &value);
   template<> nlohmann::json cast<nlohmann::json,const std::string &>(const std::string &value);
}  // end namespace core

//////////////////////////////////////////////////////////////////////////////////////////
//
//  { IN }  ->  string
//
//////////////////////////////////////////////////////////////////////////////////////////

namespace core
{
   template<> std::string cast<std::string, nlohmann::json>(nlohmann::json value);
}

//////////////////////////////////////////////////////////////////////////////////////////
//
//  { IN }  ->  u32string
//
//////////////////////////////////////////////////////////////////////////////////////////

namespace core
{
   template<> std::u32string cast<std::u32string,const std::string &>(const std::string &value);
   template<> std::u32string cast<std::u32string,std::string>(std::string value);
   template<> std::u32string cast<std::u32string,int &>(int &value);
   template<> std::u32string cast<std::u32string,int>(int value);
   template<> std::u32string cast<std::u32string,unsigned int &>(unsigned int &value);
   template<> std::u32string cast<std::u32string,unsigned int>(unsigned int value);
   template<> std::u32string cast<std::u32string,long int &>(long int &value);
   template<> std::u32string cast<std::u32string,long int>(long int value);
   template<> std::u32string cast<std::u32string,unsigned long int &>(unsigned long int &value);
   template<> std::u32string cast<std::u32string,unsigned long int>(unsigned long int value);
   template<> std::u32string cast<std::u32string,long long int &>(long long int &value);
   template<> std::u32string cast<std::u32string,long long int>(long long int value);
   template<> std::u32string cast<std::u32string,unsigned long long int &>(unsigned long long int &value);
   template<> std::u32string cast<std::u32string,unsigned long long int>(unsigned long long int value);
}  // end namespace core


//////////////////////////////////////////////////////////////////////////////////////////
//
//  u32string  ->  { TO }
//
//////////////////////////////////////////////////////////////////////////////////////////

namespace core
{
   template<> std::string cast<std::string,const std::u32string &>(const std::u32string &u32str);
   template<> std::string cast<std::string,std::u32string>(std::u32string u32str);
   template<> int cast<int,const std::u32string &>(const std::u32string &u32str);
   template<> long int cast<long int,const std::u32string &>(const std::u32string &u32str);
   template<> unsigned long int cast<unsigned long int,const std::u32string &>(const std::u32string &u32str);
   template<> long long int cast<long long int,const std::u32string &>(const std::u32string &u32str);
   template<> unsigned long long int cast<unsigned long long int,const std::u32string &>(const std::u32string &u32str);
}  // end namespace core

#endif