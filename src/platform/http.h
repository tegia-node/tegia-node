
#ifndef H_HTTP
#define H_HTTP
// --------------------------------------------------------------------

// STL
   #include <iostream>
   #include <thread>
   #include <stdlib.h>
   #include <cstdio>
   #include <signal.h>
   #include <execinfo.h>
   #include <algorithm>

// Libs
   #include <curl/curl.h>

// Tegia headers
   #include <tegia/const.h> 

   #include "core.h"
   #include "jsonextension.h"
   #include "cstring.h"







////////////////////////////////////////////////////////////////////////////////////////////
/**
    \namespace tegia

*/ 
////////////////////////////////////////////////////////////////////////////////////////////
namespace tegia
{
namespace core
{
   ////////////////////////////////////////////////////////////////////////////////////////////
   /**
      \class http
      Обертка над libcurl для выполнения http-запросов

      \todo Класс http без потери смысла можно переделать в inline функцию
   */ 
   ////////////////////////////////////////////////////////////////////////////////////////////
   class http
   {
      protected:
         //std::vector<std::string> cookies;   // Массив с куками, полученными по итогам предыдущего запроса 
         struct curl_slist *scookies = nullptr;

      public: 

         std::string url;
         std::string body;
         std::string method;
         std::string useraget_current;  // user agent для текущего запроса
         std::string buffer;  
         std::string header;

         http(){ };
         ~http()
         {              };






          static std::size_t callback(
                  const char* in,
                  std::size_t size,
                  std::size_t num,
                  std::string* out)
          {
              const std::size_t totalBytes(size * num);
              out->append(in, totalBytes);
              return totalBytes;
          }


}   // end namespace tegia
}   // end namespace core

#endif