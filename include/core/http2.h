#ifndef H_HTTP2
#define H_HTTP2
// ------------------------------------------------------------------------------------------

#include <iostream>
#include <thread>
#include <mutex>
#include <algorithm>

#include <stdlib.h>
#include <cstdio>
#include <signal.h>
#include <execinfo.h>
#include <math.h>

#include "../const.h"
#include "core.h"
#include "cast.h"
#include "string.h"
#include "json.h"

#include <curl/curl.h>



////////////////////////////////////////////////////////////////////////////////////////////
/**   
   REM:
     Любой http-запрос содержит в себе два компонента:
     * request - параментры http-запроса
     * response - результаты выполнения http-запроса 


   REQUEST:
     * method
     * url
     * headers
     * cookie
     * user aget
     * keep alive


   RESPONSE:

*/ 
////////////////////////////////////////////////////////////////////////////////////////////


/*

	libCURL ERRORS
	https://curl.se/libcurl/c/libcurl-errors.html\

	6	CURLE_COULDNT_RESOLVE_HOST
		Couldn't resolve host. The given remote host was not resolved. 

	28	CURLE_OPERATION_TIMEDOUT
		Operation timeout. The specified time-out period was reached according to the conditions. 

	35	CURLE_SSL_CONNECT_ERROR
		A problem occurred somewhere in the SSL/TLS handshake. You really want the error 
		buffer and read the message there as it pinpoints the problem slightly more. Could 
		be certificates (file formats, paths, permissions), passwords, and others. 	

	77	CURLE_SSL_CACERT_BADFILE
		Problem with reading the SSL CA cert (path? access rights?) 

*/


struct HTTP_DATA
{
	long long int total = 1;
	long long int current = 1;
	std::string filename;
	std::ofstream file;

   std::unordered_map<std::string,std::string> headers;
   std::string raw_headers;
};


namespace core
{
	////////////////////////////////////////////////////////////////////////////////////////////
	/**
		 \class http2
		Обертка над libcurl для выполнения http-запросов
	*/ 
	////////////////////////////////////////////////////////////////////////////////////////////
	
	class [[deprecated]] http2
	{
      public:

         struct Proxy
         {
            std::string addr;
            std::string usr;
            std::string pwd;
            long        port = 0;
            long        type = CURLPROXY_HTTP;
            long        http_auth   = CURLAUTH_NONE;
            long        socks5_auth = CURLAUTH_NONE;
            bool        use_tunnel = false;
         };

		protected:
			//std::vector<std::string> cookies;   // Массив с куками, полученными по итогам предыдущего запроса 
			struct curl_slist *scookies = nullptr;
			nlohmann::json cookies;
			nlohmann::json history;

			int cout_req = 0;

         std::string proxy_addr;
         std::string proxy_usr;
         std::string proxy_pwd;
         long        proxy_port = 0;
         long        proxy_type = CURLPROXY_HTTP;
         long        proxy_http_auth   = CURLAUTH_NONE;
         long        proxy_socks5_auth = CURLAUTH_NONE;
         bool        proxy_use_tunnel = false;

			CURL *curl;

			std::mutex http_request_mutex;

         Proxy proxy_set;

			// nlohmann::json header2json(const std::string &header);
			nlohmann::json cookie2json(CURL *curl);

			// std::ofstream _file;
			

		public: 

			nlohmann::json header2json(const std::string &header);

			std::string url;
			std::string body;
			std::string method;
			std::string useraget_current;  // user agent для текущего запроса
			std::string buffer;  
			std::string header;

         HTTP_DATA _data;

			http2();
			~http2();

			nlohmann::json get_history(int count);

			static std::size_t writeheader(
					const char* in,
					std::size_t size,
					std::size_t num,
					void* out)
			{
            const std::size_t totalBytes(size * num);
            HTTP_DATA * _data = reinterpret_cast<HTTP_DATA*>(out);
            _data->raw_headers.append(in, totalBytes);

            //std::cout << "[" << std::string(in,totalBytes) << "]\n";

            auto vhead = core::explode( std::string(in,totalBytes),": ",false);
            if(vhead.size() == 2)
            {
               // std::cout << "[" << vhead[0] << "] [" << core::string::trim(vhead[1]) << "]\n";
               _data->headers.insert({vhead[0],core::string::trim(vhead[1])});
               if(vhead[0] == "Content-Length")
               {
                  _data->total = core::cast<long long int>( core::string::trim(vhead[1]) );
               }

               if(vhead[0] == "content-length")
               {
                  _data->total = core::cast<long long int>( core::string::trim(vhead[1]) );
               }

            }
            else
            {
               if(vhead[0] == "\n\n" || vhead[0] == "\r\n\r\n")
               {
                  std::cout << _OK_TEXT_ << "END HEADER\n";
               }
            }
            
            return totalBytes;
         }


			static std::size_t writedata(
					const char* in,
					std::size_t size,
					std::size_t num,
					std::string* out)
			{
				const std::size_t totalBytes(size * num);
				out->append(in, totalBytes);
				return totalBytes;
			}


			static std::size_t writefile(
					const char* in,
					std::size_t size,
					std::size_t num,
					void* out)
			{
				const std::size_t totalBytes(size * num);
				HTTP_DATA * data = reinterpret_cast<HTTP_DATA*>(out);
				data->file.write(in, totalBytes);
				data->file.flush();
				data->current = data->current + totalBytes;

				std::cout << "\rloaded [" << floor( ((double) data->current / (double) data->total) * 100) << "%] [" << data->current << "/" << data->total << "] bytes" << std::flush;

				return totalBytes;
			}

         /**
            \brief Возвращает параметры get-запроса по-умолчанию. Данный метод используется если необходимо выполнить 
               запрос с нестандартными параметрами. Для этого, с помощью этого метода получают параметры get-запроса
               и дополняют или модифицирут как необходимо
         
            ---------------------------------------------------------------------------------------------

            \param[out] get_request
            \parblock JSON-объект, содержащий информацию о настройках get-запроса 
            \code
            {
               "useragent": "Mozilla/5.0 (Windows NT 6.1; rv:52.0) Gecko/20100101 Firefox/52.0",
               "headers":
               [
                  "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,* / *;q=0.8",
                  "Accept-Language: ru-RU,ru;q=0.8,en-US;q=0.5,en;q=0.3"
               ]   
            }
            \endcode
            \endparblock
         */

          static nlohmann::json default_get_request();
          static nlohmann::json default_post_request();
          static nlohmann::json default_ftp_request();

          int default_curl_opt(const std::string &method, nlohmann::json *request, struct curl_slist* headers);


         /** 
             \brief Выполнение GET-запроса
           
             ---------------------------------------------------------------------------------------------
          
              \param[in] url 
              \parblock
                 url, на который нужно выполнить GET-запрос
              \endparblock

              \param[in] request 
              \parblock
                 Json объект с параметрами запроса. Для получения параменов запроса, используемые по-умолчанию можно 
                 использовать core::http2::default_get_request();

                 \code{.js}
                   {
                      "useragent": "Mozilla/5.0 (Windows NT 6.1; rv:52.0) Gecko/20100101 Firefox/52.0",
                      "headers":
                      [
                         "Accept: text/html,application/xhtml+xml,application/xml",
                         "Accept-Language: ru-RU,ru;q=0.8,en-US;q=0.5,en;q=0.3"
                      ],
                      "ssl":
                      {
                         "certificate": <string>,
                         "private_key": <string>,
                         "check": true | false
                      }
                   }
                 \endcode

              \endparblock

              \param[out] status
              \parblock
                 Статус выполнения http-запроса.

                 Если status < 100, то значит при выполнении запроса произошла ошибка на уровне библиотеки libcurl.
                 Коды ошибок libcurl приведены тут: https://curl.haxx.se/libcurl/c/libcurl-errors.html

                 Если status >= 100, то он означает код состояния http-ответа сервера:
                 https://en.wikipedia.org/wiki/List_of_HTTP_status_codes
              \endparblock

              
              ---------------------------------------------------------------------------------------------

              ### Простой get-запрос

              \code{.cpp}
                  
                 auto http = new core::http2();
                 int  status = http->get("http://tegia.ru");
                 if(status == 200)
                 {
                    std::cout << http->buffer << std::endl;
                 }
                 delete http;

              \endcode

              
              #### Get-запрос с ssl-аутентификацией

              \code{.cpp} 
                 // Request по-умолчанию 
                 nlohmann::json request = core::http2::default_get_request();
                 // Информаци об SSL-сертификате
                 request["ssl"]["certificate"] = "/var/tegia/configurations/government/_bin/ssl/160903.crt.pem";
                 request["ssl"]["private_key"] = "/var/tegia/configurations/government/_bin/ssl/160903.key.pem";
                 // Верификация сертификата. Для самоподписанных сертификатоов должено быть false
                 request["ssl"]["check"] = false;

                 auto http = new core::http2();
                 int  status = http->get("http://tegia.ru",request);
                 if(status == 200)
                 {
                    std::cout << http->buffer << std::endl;
                 }
                 delete http;

              \endcode

         */   
         ////////////////////////////////////////////////////////////////////////////////////////////
  
         int get(const std::string &url, nlohmann::json request = nlohmann::json() );
         int head(const std::string &url, nlohmann::json request = nlohmann::json() );
         int post(const std::string &url, const std::string &post_data, nlohmann::json request = nlohmann::json() );

         int file(const std::string &url, const std::string &filename, nlohmann::json request = nlohmann::json() );

         int ftp(const std::string &url, nlohmann::json request = nlohmann::json() );


         nlohmann::json json()
         {
            nlohmann::json data;
            auto v = core::explode(this->header,"\r\n");

            for(auto it = v.begin(); it != v.end(); it++)
            {
               data["response"]["headers"].push_back( (*it) );
            }

            data["response"]["buffer"] = this->buffer;
            data["request"]["url"] = this->url;
            data["request"]["body"] = this->body;
            data["request"]["method"] = this->method;
            data["request"]["useraget_current"] = this->useraget_current;

            while(this->scookies) 
            {
               data["request"]["cookie"].push_back(this->scookies->data);
               this->scookies = this->scookies->next;
            }

            return data;
         }



         nlohmann::json cookie_to_json()
         {
            nlohmann::json data;
            while(this->scookies) 
            {
               data.push_back(this->scookies->data);
               this->scookies = this->scookies->next;
            }
            return data;
         }




         nlohmann::json get_history()
         {
            return this->history;
         }

         std::string get_host(const std::string &url)
         {
            std::string url2 = core::explode(url.substr(8,url.size()-9),"/")[0];
            return url2;
         }

         nlohmann::json cookie2json( );

         void set_cookies( const nlohmann::json& jcooks );



         /** 
             \brief Установка параметров прокси-соединения
          
              \param[in] _addr 
                 адрес прокси-сервера. Может содержать тип прокси, добавлением в начале адреса
                 соответствующего префикса ( http://, https://, socks4://, socks4a://, socks5://, socks5h://)
                 Может содержать порт, добавлением в конце адреса через :

              \param[in] _port 
                 порт прокси-сервера

              \param[in] _type 
                 тип прокси-сервера ( см. структуру curl_proxytype )

              \param[in] _usr 
                 имя пользователя

              \param[in] _pwd 
                 пароль

              \param[in] _http_auth 
                 Метод авторизации HTTP ( CURLAUTH_BASIC или CURLAUTH_NTLM )

              \param[in] _socks5_auth 
                 Метод аутентификации SOCKS5 ( CURLAUTH_BASIC, CURLAUTH_GSSAPI, CURLAUTH_NONE )
                 если используем SOCKS5 прокси

              \param[in] _use_tunnel 
                 true для туннелирования через указанный HTTP-прокси.
         */  

         void set_proxy( 
            const std::string& _addr, 
            long _port = 0, 
            long _type = CURLPROXY_HTTP, 
            const std::string& _usr = "", 
            const std::string& _pwd = "", 
            long _auth = CURLAUTH_NONE,
            long _socks5_auth = CURLAUTH_NONE, 
		      bool _use_tunnel = false );

         void set_proxy( const Proxy& _proxy_set );


   }; // end class http



}   // end namespace core

#endif