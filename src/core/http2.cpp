#include "../../include/core/http2.h"


bool set_proxy(CURL *curl)
{
   //curl_easy_setopt(curl, CURLOPT_HTTPPROXYTUNNEL, 1);
   //curl_easy_setopt(curl, CURLOPT_PROXYAUTH, CURLAUTH_NTLM);
   //curl_easy_setopt(curl, CURLOPT_PROXY, "http://193.42.108.208:65233/");
   //curl_easy_setopt(curl, CURLOPT_PROXYPORT, "65233");
   //curl_easy_setopt(curl, CURLOPT_PROXYUSERPWD, "igor:D0p8JeI");

   /*
   curl_setopt($ch, CURLOPT_HTTPPROXYTUNNEL, 1);
   curl_setopt($ch, CURLOPT_PROXYAUTH, CURLAUTH_NTLM );
   curl_setopt($ch, CURLOPT_PROXY, 'my.proxy');
   curl_setopt($ch, CURLOPT_PROXYPORT, 'my.port');
   curl_setopt($ch, CURLOPT_PROXYUSERPWD, 'domain\user:password'); 
   */   

   return true;
};


namespace core
{

////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////////////////

http2::http2()
{
	this->useraget_current = "Mozilla/5.0 (Windows NT 6.1; rv:52.0) Gecko/20100101 Firefox/52.0";
	this->curl = curl_easy_init();
	if(this->curl) 
	{

	}
};

////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////////////////

http2::~http2()
{ 
   curl_slist_free_all(this->scookies);
   curl_easy_cleanup(this->curl);   
};

////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////////////////

nlohmann::json http2::default_get_request()
{
   nlohmann::json tmp;
   tmp["useragent"] = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/86.0.4240.75 Safari/537.36";
   tmp["headers"].push_back("Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8");
   tmp["headers"].push_back("Accept-Language: ru-RU,ru;q=0.8,en-US;q=0.5,en;q=0.3");
   tmp["method"] = "GET";
   return tmp;
};

////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////////////////

nlohmann::json http2::default_post_request()
{
   nlohmann::json tmp;
   tmp["useragent"] = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/86.0.4240.75 Safari/537.36";
   tmp["headers"].push_back("Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8");
   tmp["headers"].push_back("Accept-Language: ru-RU,ru;q=0.8,en-US;q=0.5,en;q=0.3");
   tmp["method"] = "POST";
   return tmp;
};


nlohmann::json http2::default_ftp_request()
{
   nlohmann::json tmp;
   tmp["method"] = "FTP";
   return tmp;
};

////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////////////////


int http2::default_curl_opt(const std::string &method, nlohmann::json *request, struct curl_slist* headers)
{
	// ---------------------------------------------------------------
	// Заголовки запроса
	// ---------------------------------------------------------------

	(*request)["method"] = method;
	for(auto header_data: (*request)["headers"])
	{
		headers = curl_slist_append(headers,header_data.get<std::string>().c_str());  
	}
	curl_easy_setopt(this->curl, CURLOPT_HTTPHEADER, headers);

	// ---------------------------------------------------------------
	// SSL
	// ---------------------------------------------------------------
	
	// TODO: верифицировать все поля request["ssl"]
	//  - существование и тип поля
	//  - фактическое существование файлов сертификата и ключа

	if((*request)["ssl"].is_object() == true)
	{
		curl_easy_setopt(this->curl, CURLOPT_SSLCERT, (*request)["ssl"]["certificate"].get<std::string>().c_str());
		curl_easy_setopt(this->curl, CURLOPT_SSLKEY, (*request)["ssl"]["private_key"].get<std::string>().c_str());
		if((*request)["ssl"]["check"].get<bool>() == true)
		{
			curl_easy_setopt(this->curl, CURLOPT_SSL_VERIFYPEER, 1L);
			curl_easy_setopt(this->curl, CURLOPT_SSL_VERIFYHOST, 0L);
		}
		else
		{
			curl_easy_setopt(this->curl, CURLOPT_SSL_VERIFYPEER, 0L);
			curl_easy_setopt(this->curl, CURLOPT_SSL_VERIFYHOST, 0L);
		}
	}

	// ---------------------------------------------------------------
	// curl_easy_setopt 
	// ---------------------------------------------------------------

	curl_easy_setopt(this->curl, CURLOPT_TIMEOUT, 30L);
	// curl_easy_setopt(this->curl, CURLOPT_VERBOSE, 1L);

	if("HEAD" == method)
	{
		curl_easy_setopt(this->curl, CURLOPT_HEADERFUNCTION, http2::writedata);
		curl_easy_setopt(this->curl, CURLOPT_HEADERDATA, &(this->header) );
		curl_easy_setopt(this->curl, CURLOPT_NOBODY ,1L);
		curl_easy_setopt(this->curl, CURLOPT_USERAGENT,(*request)["useragent"].get<std::string>().c_str());
		curl_easy_setopt(this->curl, CURLOPT_TCP_KEEPALIVE, 1L);
		curl_easy_setopt(this->curl, CURLOPT_CUSTOMREQUEST, "HEAD");
	}

	if("GET" == method)
	{
		curl_easy_setopt(this->curl, CURLOPT_HEADERFUNCTION, http2::writedata);
		curl_easy_setopt(this->curl, CURLOPT_HEADERDATA, &(this->header) );
		curl_easy_setopt(this->curl, CURLOPT_WRITEFUNCTION, http2::writedata);
		curl_easy_setopt(this->curl, CURLOPT_WRITEDATA, &(this->buffer) );
		curl_easy_setopt(this->curl, CURLOPT_USERAGENT,(*request)["useragent"].get<std::string>().c_str());
		curl_easy_setopt(this->curl, CURLOPT_TCP_KEEPALIVE, 1L);
		curl_easy_setopt(this->curl, CURLOPT_CUSTOMREQUEST, "GET");
	}

	if("POST" == method)
	{
		curl_easy_setopt(this->curl, CURLOPT_HEADERFUNCTION, http2::writedata);
		curl_easy_setopt(this->curl, CURLOPT_HEADERDATA, &(this->header) );
		curl_easy_setopt(this->curl, CURLOPT_WRITEFUNCTION, http2::writedata);
		curl_easy_setopt(this->curl, CURLOPT_WRITEDATA, &(this->buffer) );
		curl_easy_setopt(this->curl, CURLOPT_USERAGENT,(*request)["useragent"].get<std::string>().c_str());
		curl_easy_setopt(this->curl, CURLOPT_TCP_KEEPALIVE, 1L);
		curl_easy_setopt(this->curl, CURLOPT_CUSTOMREQUEST, "POST");
	}

	if("FTP" == method)
	{
		curl_easy_setopt(this->curl, CURLOPT_HEADERFUNCTION, http2::writedata);
		curl_easy_setopt(this->curl, CURLOPT_HEADERDATA, &(this->header) );
		curl_easy_setopt(this->curl, CURLOPT_WRITEFUNCTION, http2::writedata);
		curl_easy_setopt(this->curl, CURLOPT_WRITEDATA, &(this->buffer) );
		//curl_easy_setopt(this->curl, CURLOPT_USERAGENT,(*request)["useragent"].get<std::string>().c_str());
		//curl_easy_setopt(this->curl, CURLOPT_TCP_KEEPALIVE, 1L);
		//curl_easy_setopt(this->curl, CURLOPT_CUSTOMREQUEST, "POST");
	}

	if("FILE" == method)
	{
		curl_easy_setopt(this->curl, CURLOPT_HEADERFUNCTION, http2::writeheader);
		curl_easy_setopt(this->curl, CURLOPT_HEADERDATA, reinterpret_cast<void*>(&this->_data) );
		curl_easy_setopt(this->curl, CURLOPT_WRITEFUNCTION, http2::writefile);
		curl_easy_setopt(this->curl, CURLOPT_WRITEDATA, reinterpret_cast<void*>(&this->_data) );
		curl_easy_setopt(this->curl, CURLOPT_CUSTOMREQUEST, "GET");
		curl_easy_setopt(this->curl, CURLOPT_TIMEOUT, 3600L);
		curl_easy_setopt(this->curl, CURLOPT_USERAGENT,(*request)["useragent"].get<std::string>().c_str());

		// curl_easy_setopt(this->curl, CURLOPT_VERBOSE, 1L);
	}

	if( !proxy_set.addr.empty() )
	{ 
		curl_easy_setopt( this->curl, CURLOPT_PROXY, this->proxy_set.addr.c_str() );

		if( this->proxy_set.port > 0 )
		{
			curl_easy_setopt( this->curl, CURLOPT_PROXYPORT, this->proxy_set.port );
		}

		if ( this->proxy_set.type != CURLPROXY_HTTP)
		{
			curl_easy_setopt( this->curl, CURLOPT_PROXYTYPE, this->proxy_set.type );
		}

		//curl_easy_setopt( this->curl, CURLOPT_SSL_VERIFYPEER, 0 );
		//curl_easy_setopt( this->curl, CURLOPT_SSL_VERIFYHOST, 0 );

		if( !this->proxy_set.usr.empty() )
		{
			curl_easy_setopt( this->curl, CURLOPT_PROXYUSERNAME, this->proxy_set.usr.c_str() );
			curl_easy_setopt( this->curl, CURLOPT_PROXYPASSWORD, this->proxy_set.pwd.c_str() );
			
			if( this->proxy_set.http_auth != CURLAUTH_NONE ) 
			{
				curl_easy_setopt( this->curl, CURLOPT_PROXYAUTH, 	 this->proxy_set.http_auth );
			}

			if( this->proxy_set.socks5_auth != CURLAUTH_NONE ) 
			{
				curl_easy_setopt( this->curl, CURLOPT_SOCKS5_AUTH, 	 this->proxy_set.socks5_auth );
			}
		}

		curl_easy_setopt( this->curl, CURLOPT_HTTPPROXYTUNNEL, proxy_set.use_tunnel? 1 : 0 );
	}
	else
	{
		curl_easy_setopt( this->curl, CURLOPT_PROXY, nullptr );
	}

	return 0;
};

////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////////////////

nlohmann::json http2::header2json(const std::string &header)
{
	auto v = core::explode(header,"\r\n",false);
	nlohmann::json headers;
	for(auto it = v.begin(); it != v.end(); it++)
	{
		auto vhead = core::explode( (*it),": ",false);
		if(vhead.size() == 2)
		{
			headers[vhead[0]] = vhead[1];
		}

		if(vhead.size() == 1 && vhead[0] != "")
		{
			headers["Protocol"] = vhead[0]; 
		}
	}
	return headers;
};

////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////////////////

nlohmann::json http2::cookie2json(CURL *curl)
{
	struct curl_slist *cookies = nullptr; 
	struct curl_slist *p_cookies = nullptr; 
	curl_easy_getinfo (curl, CURLINFO_COOKIELIST, &(cookies)); 

	p_cookies = cookies;

	nlohmann::json data;
	while(cookies) 
	{
		data.push_back(cookies->data);
		cookies = cookies->next;
	}

	curl_slist_free_all(p_cookies);
	return data;
};

////////////////////////////////////////////////////////////////////////////////////////////
//
// Получение куков в виде json
//
////////////////////////////////////////////////////////////////////////////////////////////

nlohmann::json http2::cookie2json( )
{
	return std::move( cookie2json( this->curl ) );
}

////////////////////////////////////////////////////////////////////////////////////////////
//
// Добавление куков
//
////////////////////////////////////////////////////////////////////////////////////////////

void http2::set_cookies( const nlohmann::json& jcooks )
{
	for( const auto& cook_data: jcooks )
	{
		curl_easy_setopt(curl, CURLOPT_COOKIELIST, cook_data.get<std::string>().c_str() );
	}

}

////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////////////////

nlohmann::json http2::get_history(int count)
{
	if(count == 0)
	{
		return this->history;
	}
	return this->history[count - 1];
};


////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////////////////

int http2::head(const std::string &url, nlohmann::json request)
{
	nlohmann::json response;
	if(request.is_null())
	{
		request = http2::default_get_request();
	}

	request["url"] = url;

	CURLcode res;
	int status = 0;
	this->header = "";

	if(this->curl) 
	{
		// Устанавливаем заголовки запроса   
		struct curl_slist *headers = nullptr;
		this->default_curl_opt("HEAD", &request, headers);   

		curl_easy_setopt(this->curl, CURLOPT_URL, url.c_str() );
		curl_easy_setopt(this->curl, CURLOPT_COOKIEFILE, "");

		int res = curl_easy_perform(this->curl);
		if(res != CURLE_OK)
		{
			// TODO: ЗАПИСАТЬ В ЛОГ!!!!
			// Коды ошибок libcurl приведены тут: https://curl.haxx.se/libcurl/c/libcurl-errors.html
			// Если status < 100, то значит возникла ошибка в libcurl и запрос не выполнен совсем
			response["error"] = res;
			return res;
		}
		else 
		{
			curl_easy_getinfo (this->curl, CURLINFO_RESPONSE_CODE, &status);
			response["status"] = status;
			response["headers"] = this->header2json(this->header);
			response["cookies"] = this->cookie2json(this->curl);
		}

		curl_slist_free_all(headers);

		nlohmann::json data;
		data["request"] = request;
		data["response"] = response;
		this->history.push_back(data);
		this->cout_req = this->cout_req + 1;

		return status;

	}

	return -1;
};

////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////////////////

int http2::post(const std::string &url, const std::string &post_data, nlohmann::json request)
{
	nlohmann::json response;
	if(request.is_null() == true)
	{
		request = http2::default_post_request();
	}

	request["url"] = url;

	CURLcode res;
	int status = 0;

	if(this->curl) 
	{
		// Устанавливаем заголовки запроса   
		struct curl_slist *headers = nullptr;
		this->default_curl_opt("POST", &request, headers);   

		curl_easy_setopt(this->curl, CURLOPT_URL, url.c_str() );
		curl_easy_setopt(this->curl, CURLOPT_COOKIEFILE, "");
		curl_easy_setopt(this->curl, CURLOPT_POSTFIELDS, post_data.c_str());
		curl_easy_setopt(this->curl, CURLOPT_TIMEOUT, 130);
		
		if(request["basic"].is_null() == false)
		{
			curl_easy_setopt(this->curl, CURLOPT_HTTPAUTH, (long)CURLAUTH_BASIC);
			curl_easy_setopt(this->curl, CURLOPT_USERNAME, request["basic"]["user"].get<std::string>().c_str());
			curl_easy_setopt(this->curl, CURLOPT_PASSWORD, request["basic"]["password"].get<std::string>().c_str());            
		}

		int res = curl_easy_perform(this->curl);
		if(res != CURLE_OK)
		{
			// TODO: ЗАПИСАТЬ В ЛОГ!!!!
			// Коды ошибок libcurl приведены тут: https://curl.haxx.se/libcurl/c/libcurl-errors.html
			// Если status < 100, то значит возникла ошибка в libcurl и запрос не выполнен совсем
			response["error"] = res;
			curl_slist_free_all(headers);
			return res;
		}
		else 
		{
			curl_easy_getinfo (this->curl, CURLINFO_RESPONSE_CODE, &status);

			response["status"] = status;
			response["headers"] = this->header2json(this->header);
			response["cookies"] = this->cookie2json(this->curl);

			curl_slist_free_all(headers);
		}   

		nlohmann::json data;
		data["request"] = request;
		data["response"] = response;
		this->history.push_back(data);

		return status;

	}

	return -1;
}

////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////////////////

int http2::get(const std::string &url, nlohmann::json request)
{
	nlohmann::json response;
	if(request.is_null())
	{
		request = http2::default_get_request();
	}

	request["url"] = url;

	// std::cout << request.dump() << std::endl;

	CURLcode res;
	int status = 0;

	if(this->curl) 
	{
		// Устанавливаем заголовки запроса
		struct curl_slist *headers = nullptr;
		this->default_curl_opt("GET", &request, headers);

		curl_easy_setopt(this->curl, CURLOPT_URL, url.c_str() );
		curl_easy_setopt(this->curl, CURLOPT_COOKIEFILE, "");

		if(request["basic"].is_null() == false)
		{
			curl_easy_setopt(this->curl, CURLOPT_HTTPAUTH, (long)CURLAUTH_BASIC);
			curl_easy_setopt(this->curl, CURLOPT_USERNAME, request["basic"]["user"].get<std::string>().c_str());
			curl_easy_setopt(this->curl, CURLOPT_PASSWORD, request["basic"]["password"].get<std::string>().c_str());
		}

		char errbuf[CURL_ERROR_SIZE];
		curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errbuf);

		// curl_easy_setopt(this->curl, CURLOPT_SSL_VERIFYPEER, 0L);

		int res = curl_easy_perform(this->curl);
		if(res != CURLE_OK)
		{
			// TODO: ЗАПИСАТЬ В ЛОГ!!!!
			// Коды ошибок libcurl приведены тут: https://curl.haxx.se/libcurl/c/libcurl-errors.html
			// Если status < 100, то значит возникла ошибка в libcurl и запрос не выполнен совсем

			//size_t len = strlen(errbuf);
			//fprintf(stderr, "\nlibcurl: (%d) ", res);
			//if(len)
			//   fprintf(stderr, "%s%s", errbuf,
			//         ((errbuf[len - 1] != '\n') ? "\n" : ""));
			//else
			//fprintf(stderr, "%s\n", curl_easy_strerror( (int) res));

			response["error"] = res;
			curl_slist_free_all(headers);
			return res;
		}
		else 
		{
			curl_easy_getinfo (this->curl, CURLINFO_RESPONSE_CODE, &status);
			response["status"] = status;

			// Читаем полученные заголовки ответа
			response["headers"] = this->header2json(this->header);
			// Читаем полученные cookie   
			response["cookies"] = this->cookie2json(this->curl);

			curl_slist_free_all(headers);
		}

		nlohmann::json data;
		data["request"] = request;
		data["response"] = response;
		this->history.push_back(data);

		return status;
	}

	return -1;
}; 



////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////////////////

int http2::ftp(const std::string &url, nlohmann::json request)
{
	nlohmann::json response;
	if(request.is_null())
	{
		request = http2::default_ftp_request();
	}
	request["url"] = url;

	//std::cout << core::cast<std::string>(request) << std::endl;

	CURLcode res;
	int status = 0;

	if(this->curl) 
	{
				// Устанавливаем заголовки запроса   
				struct curl_slist *headers = nullptr;
				this->default_curl_opt("FTP", &request, headers);   

				curl_easy_setopt(this->curl, CURLOPT_URL, url.c_str() );
				curl_easy_setopt(this->curl, CURLOPT_DIRLISTONLY, 1L);


				/*if(request["basic"].is_null() == false)
				{
				curl_easy_setopt(this->curl, CURLOPT_HTTPAUTH, (long)CURLAUTH_BASIC);
				curl_easy_setopt(this->curl, CURLOPT_USERNAME, request["basic"]["user"].get<std::string>().c_str());
				curl_easy_setopt(this->curl, CURLOPT_PASSWORD, request["basic"]["password"].get<std::string>().c_str());            
				}*/

				char errbuf[CURL_ERROR_SIZE];
				curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errbuf);


				//curl_easy_setopt(this->curl, CURLOPT_SSL_VERIFYPEER, 0L);


				int res = curl_easy_perform(this->curl);
				if(res != CURLE_OK)
				{
				// TODO: ЗАПИСАТЬ В ЛОГ!!!!
				// Коды ошибок libcurl приведены тут: https://curl.haxx.se/libcurl/c/libcurl-errors.html
				// Если status < 100, то значит возникла ошибка в libcurl и запрос не выполнен совсем

				size_t len = strlen(errbuf);
				fprintf(stderr, "\nlibcurl: (%d) ", res);
				if(len)
					fprintf(stderr, "%s%s", errbuf,
							((errbuf[len - 1] != '\n') ? "\n" : ""));
				//else
					//fprintf(stderr, "%s\n", curl_easy_strerror( (int) res));

				response["error"] = res;
				curl_slist_free_all(headers);
				return res;
				}
				else 
				{
				curl_easy_getinfo (this->curl, CURLINFO_RESPONSE_CODE, &status);
				response["status"] = status;

				// Читаем полученные заголовки ответа
				//   response["headers"] = this->header2json(this->header);
				// Читаем полученные cookie   
				//   response["cookies"] = this->cookie2json(this->curl);

				curl_slist_free_all(headers);
				}   

				

				//nlohmann::json data;
				//data["request"] = request;
				//data["response"] = response;
				//this->history.push_back(data);

				return status;

	}

	return -1;
}; 


////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////////////////

int http2::file(const std::string &url, const std::string &filename, nlohmann::json request)
{
	nlohmann::json response;
	if(request.is_null())
	{
		request = http2::default_ftp_request();
	}
	request["url"] = url;

	//std::cout << core::cast<std::string>(request) << std::endl;

	CURLcode res;
	int status = 0;

	if(this->curl) 
	{
		this->_data.file.open(filename.c_str(), std::ios::binary);
		this->_data.current = 0;
		this->_data.total = 0;

		// Устанавливаем заголовки запроса   
		struct curl_slist *headers = nullptr;
		this->default_curl_opt("FILE", &request, headers);   

		curl_easy_setopt(this->curl, CURLOPT_URL, url.c_str() );

		char errbuf[CURL_ERROR_SIZE];
		curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errbuf);


		int res = curl_easy_perform(this->curl);
		if(res != CURLE_OK)
		{
			size_t len = strlen(errbuf);
			fprintf(stderr, "\nlibcurl: (%d) ", res);
			if(len)
				fprintf(stderr, "%s%s", errbuf,((errbuf[len - 1] != '\n') ? "\n" : ""));
			//else
				//fprintf(stderr, "%s\n", curl_easy_strerror( (int) res));

			response["error"] = res;
			curl_slist_free_all(headers);
			return res;
		}
		else 
		{
			curl_easy_getinfo (this->curl, CURLINFO_RESPONSE_CODE, &status);
			response["status"] = status;

			// Читаем полученные заголовки ответа
			//   response["headers"] = this->header2json(this->header);
			// Читаем полученные cookie   
			//   response["cookies"] = this->cookie2json(this->curl);

			curl_slist_free_all(headers);
		}

		this->_data.file.close();
		return status;
	}

	return -1;
}; 

void http2::set_proxy( 
	const std::string& _addr, 
	long _port /*= 0*/, 
	long _type /*= CURLPROXY_HTTP*/, 
	const std::string& _usr /*= ""*/, 
	const std::string& _pwd /*= ""*/, 
	long _http_auth /*= CURLAUTH_NONE*/,
	long _socks5_auth /*= CURLAUTH_NONE*/, 
	bool _use_tunnel /*= false*/ )
{
	proxy_set.addr = _addr;
	proxy_set.usr  = _usr;
	proxy_set.pwd  = _pwd;
	proxy_set.port = _port;
	proxy_set.type = _type;
	proxy_set.http_auth = _http_auth;
	proxy_set.socks5_auth = _socks5_auth;
	proxy_set.use_tunnel  = _use_tunnel;
}

void http2::set_proxy( const Proxy& _proxy_set )
{
	proxy_set = _proxy_set;
}



} // end namespace core