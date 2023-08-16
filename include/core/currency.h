#ifndef H_CORE_CURRENCY
#define H_CORE_CURRENCY
// -------------------------------------------------------------------------------------- //
//                                      INCLUDES                                          //
// -------------------------------------------------------------------------------------- //

#include <iostream>
#include <sstream>

#include <tegia/core/cast.h>

namespace core {
namespace currency {

   // Возвращает число копеек из строки
   inline long int to_int(const std::string &str)
   {
      return (long int)(core::cast<long double>(str) * 100);
   }

   inline std::string format(long int value)
   {
      std::string tmp = core::cast<std::string>(value);
      

   }


}  // end currency core
}  // end namespace core



#endif