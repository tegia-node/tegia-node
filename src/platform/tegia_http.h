
#ifndef H_TEGIA_HTTP
#define H_TEGIA_HTTP


#include <iostream>
#include <thread>


#include <stdlib.h>
#include <cstdio>
#include <signal.h>
#include <execinfo.h>

#include "const.h"
#include "core.h"
#include "jsonextension.h"
#include "cstring.h"
#include "k_xml.h"
#include <curl/curl.h>

#include <algorithm>






////////////////////////////////////////////////////////////////////////////////////////////
/**
    \namespace tegia

*/ 
////////////////////////////////////////////////////////////////////////////////////////////
namespace tegia
{

  class http_response
  {
     public: 
            http_response(){};
            ~http_response(){}; 

            std::string data;  
            std::string header;
            int status = 0;
  };   // end class tegia::http_response

  class http_request
  {
     public: 
            http_request()
            {
               this->useraget = "Mozilla/5.0 (Windows NT 6.1; rv:52.0) Gecko/20100101 Firefox/52.0";
            };
            ~http_request(){}; 

            std::string url;
            std::string host;
            std::string body;
            std::string method;
            std::string useraget;
            std::string userpwd;
            std::vector<std::string> headers;
  };   // end class tegia::http_request


  class http_data
  {
     public:
        http_data(){};
        ~http_data(){};

        http_request   request;
        http_response  response;
  };



  class http
  {
     protected:
        CURL *curl;

     public:
        http()
        {
           this->curl = curl_easy_init();
        }
        ~http()
        {
           curl_easy_cleanup(this->curl);   
        }

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

     ////////////////////////////////////////////////////////////////////////////////////////////////////////////
     /**
        params[referer] = <string>
        params[headers] = Json::arrayValue
        params["auth"]["userpwd"] = <string>; // Строка для basic-аутентификации
     
     */   
        http_data * run(const std::string &method, const std::string &url, Json::Value params = Json::nullValue)
        {
           http_data *data = new http_data();

        // Устанавливаем параметры   
           if(params.type() == Json::nullValue)
           {
              data->request.url = url;
              data->request.host = tegia::http::host(url);
              data->request.method = method;
              data->request.headers = tegia::http::default_headers();
              data->request.headers.push_back("Host: " + data->request.host);
              data->request.useraget = "Mozilla/5.0 (Windows NT 6.1; rv:52.0) Gecko/20100101 Firefox/52.0";
              data->request.userpwd = "";
           }
           else
           {
              // TODO: )))
           }

           if(this->curl) 
           {
           // Устанавливаем заголовки запроса
              struct curl_slist *chunk = NULL;
              for(auto it = data->request.headers.begin(); it != data->request.headers.end(); it++)
              {
                 chunk = curl_slist_append(chunk, (*it).c_str() );
              }
              curl_easy_setopt(this->curl, CURLOPT_HTTPHEADER, chunk);

           // Устанавливаем параметры запроса 
              curl_easy_setopt(this->curl, CURLOPT_URL, data->request.url.c_str() );

              curl_easy_setopt(this->curl, CURLOPT_WRITEFUNCTION, tegia::http::callback);
              curl_easy_setopt(this->curl, CURLOPT_WRITEDATA, &(data->response.data) );  

              curl_easy_setopt(this->curl, CURLOPT_HEADERFUNCTION, tegia::http::callback);
              curl_easy_setopt(this->curl, CURLOPT_HEADERDATA, &(data->response.header) );

              curl_easy_setopt(this->curl, CURLOPT_CUSTOMREQUEST, data->request.method.c_str());
              curl_easy_setopt(this->curl, CURLOPT_USERAGENT,data->request.useraget.c_str());

              curl_easy_setopt(this->curl, CURLOPT_TCP_KEEPALIVE, 1L);

              if(data->request.userpwd != "")
              {
                  curl_easy_setopt(this->curl, CURLOPT_HTTPAUTH, (long)CURLAUTH_BASIC);
                  curl_easy_setopt(this->curl, CURLOPT_USERPWD, data->request.userpwd.c_str());
              }

           }
        

           return data;
        };

     ////////////////////////////////////////////////////////////////////////////////////////////////////////////
     /**
        \brief Извлекает host из url
     */
        inline static std::string host(const std::string &url)
        {
            std::string url2 = tegia::core::explode(url.substr(8,url.size()-9),"/")[0];
            return url2;
        }   

     ////////////////////////////////////////////////////////////////////////////////////////////////////////////
     /**
        \brief Возвращает headers, используемый для запросов по-умолчанию
     */
        static std::vector<std::string> default_headers()     
        {
           std::vector<std::string> headers;
           headers.push_back("Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8");
           headers.push_back("Accept-Language: ru-RU,ru;q=0.8,en-US;q=0.5,en;q=0.3");
           return headers;
        }
  };







}   // end namespace tegia

#endif // H_TEGIA_HTTP