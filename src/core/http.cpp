#include <tegia/core/http.h>
namespace tegia {
namespace http {


////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*


*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////


int url_t::parse(const std::string &url)
{
	this->raw = url;

	CURLUcode rc;

	CURLU *curl = curl_url();
	if(!curl)
	{
		// error
		return 1;
	}

	rc = curl_url_set(curl, CURLUPART_URL, this->raw.c_str(), 0);
	if(rc)
	{
		// error
		return 2;
	}

	//
	// SCHEME
	//

	char *_scheme;
	rc = curl_url_get(curl, CURLUPART_SCHEME, &_scheme, 0);
	if(rc)
	{
		// error
		curl_url_cleanup(curl);
		return 3;
	}
	this->scheme = _scheme;
	curl_free(_scheme);

	//
	// HOST
	//

	char *_host;
	rc = curl_url_get(curl, CURLUPART_HOST, &_host, 0);
	if(rc) 
	{
		// error
		curl_url_cleanup(curl);
		return 4;
	}
	this->host = _host;
	curl_free(_host);

	//
	// PATH
	//

	char *_path;
	rc = curl_url_get(curl, CURLUPART_PATH, &_path, 0);
	if(rc) 
	{
		// error
		curl_url_cleanup(curl);
		return 5;
	}
	this->path = _path;
	curl_free(_path);

	curl_url_cleanup(curl);

	return 0;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*


*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////

int request::parseurl(const std::string &_url)
{
	this->url = _url;

	CURLUcode rc;

	CURLU *curl = curl_url();
	if(!curl)
	{
		// error
		return 1;
	}

	rc = curl_url_set(curl, CURLUPART_URL, this->url.c_str(), 0);
	if(rc)
	{
		// error
		return 2;
	}

	//
	// SCHEME
	//

	char *_scheme;
	rc = curl_url_get(curl, CURLUPART_SCHEME, &_scheme, 0);
	if(rc)
	{
		// error
		curl_url_cleanup(curl);
		return 3;
	}
	this->scheme = _scheme;
	curl_free(_scheme);

	//
	// HOST
	//

	char *_host;
	rc = curl_url_get(curl, CURLUPART_HOST, &_host, 0);
	if(rc) 
	{
		// error
		curl_url_cleanup(curl);
		return 4;
	}
	this->host = _host;
	curl_free(_host);

	//
	// PATH
	//

	char *_path;
	rc = curl_url_get(curl, CURLUPART_PATH, &_path, 0);
	if(rc) 
	{
		// error
		curl_url_cleanup(curl);
		return 5;
	}
	this->path = _path;
	curl_free(_path);

	curl_url_cleanup(curl);

	return 0;
}




////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*


*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////


client::client()
{
	this->curl = curl_easy_init();
	if(this->curl) 
	{
		this->request = new tegia::http::request();
		this->response = new tegia::http::response();

		curl_easy_setopt(this->curl, CURLOPT_USERAGENT,this->request->useragent.c_str());

		curl_easy_setopt(this->curl, CURLOPT_HEADERFUNCTION, tegia::http::client::writeheader);
		curl_easy_setopt(this->curl, CURLOPT_HEADERDATA, reinterpret_cast<void*>(this->response) );
		curl_easy_setopt(this->curl, CURLOPT_WRITEFUNCTION, tegia::http::client::writedata);
		curl_easy_setopt(this->curl, CURLOPT_WRITEDATA, reinterpret_cast<void*>(this->response) );	

		curl_easy_setopt(this->curl, CURLOPT_FOLLOWLOCATION, 0L);

		curl_easy_setopt(this->curl, CURLOPT_TCP_KEEPALIVE, 1L);
		curl_easy_setopt(this->curl, CURLOPT_COOKIEFILE, "");

		this->set_header("Accept","text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9");
		this->set_header("Accept-Language","ru-RU,ru;q=0.9,en-US;q=0.8,en;q=0.7");
	}
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*


*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////


client::~client()
{
 	curl_easy_cleanup(this->curl); 
	delete this->request;
	delete this->response;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*


*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////


bool client::set_timeout(long seconds)
{
	CURLcode res = curl_easy_setopt(this->curl, CURLOPT_TIMEOUT, seconds);
	if(res == CURLE_OK)
	{
		return true;	
	}
	return false;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*


*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////


bool client::set_verbose(bool flag)
{
	if(flag == true)
	{
		this->verbose = true;
		curl_easy_setopt(this->curl, CURLOPT_VERBOSE, 1L);
	}
	else
	{
		this->verbose = false;
		curl_easy_setopt(this->curl, CURLOPT_VERBOSE, 0L);
	}
	
	return true;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*


*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////


bool client::set_header(const std::string &header, const std::string &value)
{
	if(value == "")
	{
		this->request->headers.erase(header);
	}
	else
	{
		this->request->headers.insert_or_assign(header,value);
	}
	return true;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*


*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////


bool client::set_ssl(const std::string &ssl_key_path,const std::string &ssl_sert_path,bool is_check)
{
	CURLcode res;

	std::cout << "ssl_sert_path = [" << ssl_sert_path << "]" << std::endl;
	std::cout << "ssl_key_path  = [" << ssl_key_path << "]" << std::endl;

	res = curl_easy_setopt(this->curl, CURLOPT_SSLCERT, ssl_sert_path);
	if(res != CURLE_OK)
	{
		std::cout << "CURLOPT_SSLCERT res = " << res << std::endl;
	}

	res = curl_easy_setopt(this->curl, CURLOPT_SSLKEY, ssl_key_path);
	if(res != CURLE_OK)
	{
		std::cout << "CURLOPT_SSLKEY res = " << res << std::endl;
	}

	if(is_check == true)
	{
		curl_easy_setopt(this->curl, CURLOPT_SSL_VERIFYHOST, 1L);
		curl_easy_setopt(this->curl, CURLOPT_SSL_VERIFYPEER, 1L);
	}
	else
	{
		curl_easy_setopt(this->curl, CURLOPT_SSL_VERIFYHOST, 0L);
		curl_easy_setopt(this->curl, CURLOPT_SSL_VERIFYPEER, 0L);
	}

	return true;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*


*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////


bool client::set_sslp12(const std::string &ssl_sert_path,const std::string &ssl_sert_password,bool is_check)
{
	CURLcode res;

	res = curl_easy_setopt(this->curl, CURLOPT_SSLCERT, ssl_sert_path.c_str());
	if(res != CURLE_OK)
	{
		std::cout << "CURLOPT_SSLCERT res = " << res << std::endl;
	}

	res = curl_easy_setopt(this->curl, CURLOPT_SSLCERTTYPE, "p12");
	if(res != CURLE_OK)
	{
		std::cout << "CURLOPT_SSLCERTTYPE res = " << res << std::endl;
	}

	if(ssl_sert_password != "")
	{
		curl_easy_setopt(this->curl, CURLOPT_KEYPASSWD, ssl_sert_password.c_str());
	}

	if(is_check == true)
	{
		curl_easy_setopt(this->curl, CURLOPT_SSL_VERIFYHOST, 1L);
		curl_easy_setopt(this->curl, CURLOPT_SSL_VERIFYPEER, 1L);
		curl_easy_setopt(this->curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);

	}
	else
	{
		curl_easy_setopt(this->curl, CURLOPT_SSL_VERIFYHOST, 0L);
		curl_easy_setopt(this->curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(this->curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);

	}

	return true;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*


*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////


bool client::set_proxy(const std::string &_addr,const std::string &_port)
{
	std::string proxy = _addr + ":" + _port;

	std::cout << "[" << proxy << "]" << std::endl;

	curl_easy_setopt(this->curl, CURLOPT_PROXY, proxy.c_str());
	return true;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*


*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////


bool client::set_proxy(const tegia::http::proxy &_proxy = tegia::http::proxy())
{
	return true;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*


*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////


bool client::set_useragent(const std::string &_useragent)
{
	this->request->useragent = _useragent;
	curl_easy_setopt(this->curl, CURLOPT_USERAGENT,this->request->useragent.c_str());
	return true;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*


*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////


std::string client::get_redirect_url()
{
	char *redirect_url = nullptr;
	curl_easy_getinfo (this->curl, CURLINFO_REDIRECT_URL, &redirect_url);
	return redirect_url;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*


*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////


bool client::set_cookie(const std::string &_name, const std::string &_value, const std::string &_path)
{
	tegia::http::cookie _cookie;
	_cookie.name = _name;
	_cookie.value = _value;
	_cookie.path = _path;
	this->cookielist.insert({_cookie.name + _cookie.path,_cookie});
	return true;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*


*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////


int client::run()
{
	CURLcode res;
	this->response->step = 0;
	this->response->status = 0;
	this->response->data.clear();
	this->response->raw_headers.clear();
	this->response->headers.clear();
	this->response->current = 0;
	this->response->total = 0;

	//
	// HEADERS
	//

	struct curl_slist * _headers = nullptr;
	for(auto it = this->request->headers.begin();it != this->request->headers.end();it++)
	{
		_headers = curl_slist_append(_headers, std::string(it->first + ": " + it->second).c_str());  
	}
	curl_easy_setopt(this->curl, CURLOPT_HTTPHEADER, _headers);

	//
	// COOKIE
	//

//		std::cout << this->request->scheme << std::endl;
//		std::cout << this->request->host << std::endl;
//		std::cout << this->request->path << std::endl;

	if(this->cookielist.size() > 0)
	{
		//
		// Отправлять только те cookie, которые соотвесттвуют запросу
		//

		std::string str_cookie = "";
		for(auto it = this->cookielist.begin(); it != this->cookielist.end(); it++)
		{
			size_t len = it->second.path.length();
			if(this->request->path.substr(0,len) == it->second.path)
			{
				str_cookie = str_cookie + it->second.name + "=" + it->second.value + ";";

				if(this->verbose == true)
				{
					it->second.print();
					std::cout << _OK_TEXT_ << "send cookie" << std::endl;
				}
			}
			else
			{
				if(this->verbose == true)
				{
					it->second.print();
					std::cout << _ERR_TEXT_ << "not send cookie" << std::endl;
				}
			}
		}

		curl_easy_setopt(this->curl, CURLOPT_COOKIELIST, "ALL");
		curl_easy_setopt(this->curl, CURLOPT_COOKIE, str_cookie.c_str());
	}
	

	//
	// RUN QUERY
	//

	res = curl_easy_perform(this->curl);
	if(res != CURLE_OK)
	{
		this->response->status = res; //std::cout << "res = " << res << std::endl;

		curl_slist_free_all(_headers);
		return this->response->status;
	}

	//
	// RESPONSE CODE
	//

	curl_easy_getinfo (this->curl, CURLINFO_RESPONSE_CODE, &this->response->status);
		
	//
	// RESPONSE COOKIE
	//
	
	this->response->cookielist.clear();
	struct curl_slist *_cookies = nullptr; 
	res = curl_easy_getinfo (this->curl, CURLINFO_COOKIELIST, &_cookies); 
	
	struct curl_slist *each = _cookies;
	while(each) 
	{
		// std::cout << each->data << std::endl;
		// exit(0);

		cookie _cookie;
		auto arr = tegia::string::explode(each->data,"\t");
		if(arr.size() == 7)
		{
			_cookie.host = arr[0];
			_cookie.is_subdomains = arr[1];
			_cookie.path = arr[2];
			_cookie.secure = arr[3];
			_cookie.expiry = arr[4];
			_cookie.name = arr[5];
			_cookie.value = arr[6];
			_cookie._raw = each->data;
		}
		else
		{
			_cookie.host = arr[0];
			_cookie.is_subdomains = arr[1];
			_cookie.path = arr[2];
			_cookie.secure = arr[3];
			_cookie.expiry = arr[4];
			_cookie.name = arr[5];
			_cookie.value = "";
			_cookie._raw = each->data;
		}

		//
		//
		//
		
		this->response->cookielist.insert({_cookie.name + _cookie.path,_cookie});
		std::pair res = this->cookielist.insert({_cookie.name + _cookie.path,_cookie});
		if(res.second == false)
		{
			res.first->second = _cookie; 
		}
	
		each = each->next;
	}
	curl_slist_free_all(_cookies);

	//
	// 
	//

	curl_slist_free_all(_headers);
	return this->response->status;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*


*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////


int client::get(const std::string &_url,const std::string &_filename)
{
	if(this->curl) 
	{
		this->request->parseurl(_url);
		curl_easy_setopt(this->curl, CURLOPT_URL, _url.c_str() );
		curl_easy_setopt(this->curl, CURLOPT_CUSTOMREQUEST, "GET");
		curl_easy_setopt(this->curl, CURLOPT_POSTFIELDS, "");

		if(_filename != "")
		{
			this->response->is_file = true;
			this->response->filename = _filename;
			this->response->file.open(_filename.c_str(), std::ios::binary);
			int result = this->run();
			this->response->file.close();
			std::cout << " " << std::endl;
			return result;
		}
		else
		{
			this->response->is_file = false;
			return this->run();
		}
	}
	return -1;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*


*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////


int client::post(const std::string &_url, const std::string &_data)
{
	if(this->curl) 
	{
		this->request->parseurl(_url);
		curl_easy_setopt(this->curl, CURLOPT_URL, _url.c_str() );
		curl_easy_setopt(this->curl, CURLOPT_CUSTOMREQUEST, "POST");
		curl_easy_setopt(this->curl, CURLOPT_POSTFIELDS, _data.c_str());

		return this->run();
	}
	return -1;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*


*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::atomic<bool> tegia::http::stream_client_v1::is_stopped{false};

stream_client_v1::stream_client_v1()
{
	this->curl = curl_easy_init();
	if(this->curl) 
	{
		this->request = new tegia::http::request();
		this->response = new tegia::http::stream_response_v1();

		curl_easy_setopt(this->curl, CURLOPT_USERAGENT,this->request->useragent.c_str());

		curl_easy_setopt(this->curl, CURLOPT_HEADERFUNCTION, tegia::http::stream_client_v1::writeheader);
		curl_easy_setopt(this->curl, CURLOPT_HEADERDATA, reinterpret_cast<void*>(this->response) );
		curl_easy_setopt(this->curl, CURLOPT_WRITEFUNCTION, tegia::http::stream_client_v1::writedata);
		curl_easy_setopt(this->curl, CURLOPT_WRITEDATA, reinterpret_cast<void*>(this->response) );	

		curl_easy_setopt(this->curl, CURLOPT_FOLLOWLOCATION, 0L);

		curl_easy_setopt(this->curl, CURLOPT_TCP_KEEPALIVE, 1L);
		curl_easy_setopt(this->curl, CURLOPT_COOKIEFILE, "");

		curl_easy_setopt(
			this->curl, 
			CURLOPT_XFERINFOFUNCTION, 
			&stream_client_v1::xfer_info_callback
		);
		curl_easy_setopt(
			this->curl, 
			CURLOPT_XFERINFODATA, 
			&is_stopped // Передаем флаг
		);
		curl_easy_setopt(
			this->curl, 
			CURLOPT_NOPROGRESS, 
			0L
		);


		this->set_header("Accept","text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9");
		this->set_header("Accept-Language","ru-RU,ru;q=0.9,en-US;q=0.8,en;q=0.7");
	}
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*


*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////


stream_client_v1::~stream_client_v1()
{
 	curl_easy_cleanup(this->curl); 
	delete this->request;
	delete this->response;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*


*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////


bool stream_client_v1::set_timeout(long seconds)
{
	CURLcode res = curl_easy_setopt(this->curl, CURLOPT_TIMEOUT, seconds);
	if(res == CURLE_OK)
	{
		return true;	
	}
	return false;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*


*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////


bool stream_client_v1::set_verbose(bool flag)
{
	if(flag == true)
	{
		this->verbose = true;
		curl_easy_setopt(this->curl, CURLOPT_VERBOSE, 1L);
	}
	else
	{
		this->verbose = false;
		curl_easy_setopt(this->curl, CURLOPT_VERBOSE, 0L);
	}
	
	return true;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*


*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////


bool stream_client_v1::set_header(const std::string &header, const std::string &value)
{
	if(value == "")
	{
		this->request->headers.erase(header);
	}
	else
	{
		this->request->headers.insert_or_assign(header,value);
	}
	return true;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*


*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////


bool stream_client_v1::set_ssl(const std::string &ssl_key_path,const std::string &ssl_sert_path,bool is_check)
{
	CURLcode res;

	std::cout << "ssl_sert_path = [" << ssl_sert_path << "]" << std::endl;
	std::cout << "ssl_key_path  = [" << ssl_key_path << "]" << std::endl;

	res = curl_easy_setopt(this->curl, CURLOPT_SSLCERT, ssl_sert_path);
	if(res != CURLE_OK)
	{
		std::cout << "CURLOPT_SSLCERT res = " << res << std::endl;
	}

	res = curl_easy_setopt(this->curl, CURLOPT_SSLKEY, ssl_key_path);
	if(res != CURLE_OK)
	{
		std::cout << "CURLOPT_SSLKEY res = " << res << std::endl;
	}

	if(is_check == true)
	{
		curl_easy_setopt(this->curl, CURLOPT_SSL_VERIFYHOST, 1L);
		curl_easy_setopt(this->curl, CURLOPT_SSL_VERIFYPEER, 1L);
	}
	else
	{
		curl_easy_setopt(this->curl, CURLOPT_SSL_VERIFYHOST, 0L);
		curl_easy_setopt(this->curl, CURLOPT_SSL_VERIFYPEER, 0L);
	}

	return true;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*


*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////


bool stream_client_v1::set_sslp12(const std::string &ssl_sert_path,const std::string &ssl_sert_password,bool is_check)
{
	CURLcode res;

	res = curl_easy_setopt(this->curl, CURLOPT_SSLCERT, ssl_sert_path.c_str());
	if(res != CURLE_OK)
	{
		std::cout << "CURLOPT_SSLCERT res = " << res << std::endl;
	}

	res = curl_easy_setopt(this->curl, CURLOPT_SSLCERTTYPE, "p12");
	if(res != CURLE_OK)
	{
		std::cout << "CURLOPT_SSLCERTTYPE res = " << res << std::endl;
	}

	if(ssl_sert_password != "")
	{
		curl_easy_setopt(this->curl, CURLOPT_KEYPASSWD, ssl_sert_password.c_str());
	}

	if(is_check == true)
	{
		curl_easy_setopt(this->curl, CURLOPT_SSL_VERIFYHOST, 1L);
		curl_easy_setopt(this->curl, CURLOPT_SSL_VERIFYPEER, 1L);
		curl_easy_setopt(this->curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);

	}
	else
	{
		curl_easy_setopt(this->curl, CURLOPT_SSL_VERIFYHOST, 0L);
		curl_easy_setopt(this->curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(this->curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);

	}

	return true;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*


*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////


bool stream_client_v1::set_proxy(const std::string &_addr,const std::string &_port)
{
	std::string proxy = _addr + ":" + _port;

	std::cout << "[" << proxy << "]" << std::endl;

	curl_easy_setopt(this->curl, CURLOPT_PROXY, proxy.c_str());
	return true;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*


*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////


bool stream_client_v1::set_proxy(const tegia::http::proxy &_proxy = tegia::http::proxy())
{
	return true;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*


*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////


bool stream_client_v1::set_useragent(const std::string &_useragent)
{
	this->request->useragent = _useragent;
	curl_easy_setopt(this->curl, CURLOPT_USERAGENT,this->request->useragent.c_str());
	return true;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*


*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////


std::string stream_client_v1::get_redirect_url()
{
	char *redirect_url = nullptr;
	curl_easy_getinfo (this->curl, CURLINFO_REDIRECT_URL, &redirect_url);
	return redirect_url;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*


*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////


bool stream_client_v1::set_cookie(const std::string &_name, const std::string &_value, const std::string &_path)
{
	tegia::http::cookie _cookie;
	_cookie.name = _name;
	_cookie.value = _value;
	_cookie.path = _path;
	this->cookielist.insert({_cookie.name + _cookie.path,_cookie});
	return true;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*


*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////


int stream_client_v1::run()
{
	CURLcode res;
	this->response->step = 0;
	this->response->status = 0;
	this->response->raw_headers.clear();
	this->response->headers.clear();
	this->response->current = 0;
	this->response->total = 0;

	//
	// HEADERS
	//

	struct curl_slist * _headers = nullptr;
	for(auto it = this->request->headers.begin();it != this->request->headers.end();it++)
	{
		_headers = curl_slist_append(_headers, std::string(it->first + ": " + it->second).c_str());  
	}
	curl_easy_setopt(this->curl, CURLOPT_HTTPHEADER, _headers);

	//
	// COOKIE
	//

//		std::cout << this->request->scheme << std::endl;
//		std::cout << this->request->host << std::endl;
//		std::cout << this->request->path << std::endl;

	if(this->cookielist.size() > 0)
	{
		//
		// Отправлять только те cookie, которые соотвесттвуют запросу
		//

		std::string str_cookie = "";
		for(auto it = this->cookielist.begin(); it != this->cookielist.end(); it++)
		{
			size_t len = it->second.path.length();
			if(this->request->path.substr(0,len) == it->second.path)
			{
				str_cookie = str_cookie + it->second.name + "=" + it->second.value + ";";

				if(this->verbose == true)
				{
					it->second.print();
					std::cout << _OK_TEXT_ << "send cookie" << std::endl;
				}
			}
			else
			{
				if(this->verbose == true)
				{
					it->second.print();
					std::cout << _ERR_TEXT_ << "not send cookie" << std::endl;
				}
			}
		}

		curl_easy_setopt(this->curl, CURLOPT_COOKIELIST, "ALL");
		curl_easy_setopt(this->curl, CURLOPT_COOKIE, str_cookie.c_str());
	}
	

	//
	// RUN QUERY
	//

	res = curl_easy_perform(this->curl);
	if(res != CURLE_OK)
	{
		this->response->status = res; //std::cout << "res = " << res << std::endl;

		curl_slist_free_all(_headers);
		return this->response->status;
	}

	//
	// RESPONSE CODE
	//

	curl_easy_getinfo (this->curl, CURLINFO_RESPONSE_CODE, &this->response->status);
		
	//
	// RESPONSE COOKIE
	//
	
	this->response->cookielist.clear();
	struct curl_slist *_cookies = nullptr; 
	res = curl_easy_getinfo (this->curl, CURLINFO_COOKIELIST, &_cookies); 
	
	struct curl_slist *each = _cookies;
	while(each) 
	{
		// std::cout << each->data << std::endl;
		// exit(0);

		cookie _cookie;
		auto arr = tegia::string::explode(each->data,"\t");
		if(arr.size() == 7)
		{
			_cookie.host = arr[0];
			_cookie.is_subdomains = arr[1];
			_cookie.path = arr[2];
			_cookie.secure = arr[3];
			_cookie.expiry = arr[4];
			_cookie.name = arr[5];
			_cookie.value = arr[6];
			_cookie._raw = each->data;
		}
		else
		{
			_cookie.host = arr[0];
			_cookie.is_subdomains = arr[1];
			_cookie.path = arr[2];
			_cookie.secure = arr[3];
			_cookie.expiry = arr[4];
			_cookie.name = arr[5];
			_cookie.value = "";
			_cookie._raw = each->data;
		}

		//
		//
		//
		
		this->response->cookielist.insert({_cookie.name + _cookie.path,_cookie});
		std::pair res = this->cookielist.insert({_cookie.name + _cookie.path,_cookie});
		if(res.second == false)
		{
			res.first->second = _cookie; 
		}
	
		each = each->next;
	}
	curl_slist_free_all(_cookies);

	//
	// 
	//

	curl_slist_free_all(_headers);
	return this->response->status;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*


*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////


int stream_client_v1::get(const std::string &_url,const std::string &_filename)
{
	if(this->curl) 
	{
		this->request->parseurl(_url);
		curl_easy_setopt(this->curl, CURLOPT_URL, _url.c_str() );
		curl_easy_setopt(this->curl, CURLOPT_CUSTOMREQUEST, "GET");
		curl_easy_setopt(this->curl, CURLOPT_POSTFIELDS, "");

		if(_filename != "")
		{
			this->response->is_file = true;
			this->response->filename = _filename;
			this->response->file.open(_filename.c_str(), std::ios::binary);
			int result = this->run();
			this->response->file.close();
			std::cout << " " << std::endl;
			return result;
		}
		else
		{
			this->response->is_file = false;
			return this->run();
		}
	}
	return -1;
};

void stream_client_v1::stop_streaming()
{
	//response->done.store(true, std::memory_order_release);
	response->set_done();
	is_stopped.store(true, std::memory_order_release); 
} 
///////////////////////////////////////////////////////////////////////////////////////////////////////////
/*


*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////

// stream_client::stream_client() 
// {
	
//     this->curl = curl_easy_init();
//     if(this->curl) 
//     {
		

// 		 curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L); 
//         // Инициализация базовых параметров
//         this->request = new tegia::http::request();
//         this->response = new tegia::http::stream_response_v1();
// 		curl_easy_setopt(curl, CURLOPT_TCP_NODELAY, 1L);
// 		// curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1);
//         curl_easy_setopt(this->curl, CURLOPT_USERAGENT, this->request->useragent.c_str());
//         curl_easy_setopt(this->curl, CURLOPT_HEADERFUNCTION, tegia::http::stream_client::writeheader);
//         curl_easy_setopt(this->curl, CURLOPT_HEADERDATA, reinterpret_cast<void*>(this->response));
//         curl_easy_setopt(this->curl, CURLOPT_WRITEFUNCTION, tegia::http::stream_client::writedata);
//         curl_easy_setopt(this->curl, CURLOPT_WRITEDATA, reinterpret_cast<void*>(this->response));
// 		curl_easy_setopt(this->curl, CURLOPT_HEADER , true);
//   		//curl_easy_setopt(this->curl, CURLOPT_RETURNTRANSFER, true);
//   		curl_easy_setopt(this->curl, CURLOPT_SSL_VERIFYPEER, 0);
//   		curl_easy_setopt(this->curl, CURLOPT_SSL_VERIFYHOST, 0);
//         // ... другие настройки ...

//         // Инициализация multi handle
//         multi_handle = curl_multi_init();
//         curl_multi_setopt(multi_handle, CURLMOPT_SOCKETFUNCTION, &stream_client::socket_callback);
//         curl_multi_setopt(multi_handle, CURLMOPT_SOCKETDATA, this);

//         // Создание epoll
//         epoll_fd = epoll_create1(0);
//         if(epoll_fd == -1) {
//             throw std::runtime_error("epoll_create1 failed");
//         }

//         // Запуск рабочего потока (ТОЛЬКО ОДИН РАЗ!)
//         running.store(true);
//         worker_thread = std::thread([this]() {
// 			constexpr int MAX_EVENTS = 10;
// 			epoll_event events[MAX_EVENTS];

// 			while(running.load()) {
// 				//std::cerr << "Worker thread: checking events...\n";
// 				// 1. Обработка сообщений CURL
// 				process_messages();
// 				// 2. Ожидание событий
// 				int num_events = epoll_wait(epoll_fd, events, MAX_EVENTS, 100);
// 				if(num_events < 0 && errno != EINTR) {
// 					perror("epoll_wait");
// 					break;
// 				}
// 				if(num_events == 0) {
// 					curl_multi_socket_action(multi_handle, CURL_SOCKET_TIMEOUT, 0, &still_running);
// 					//std::cerr << "No events: timer action executed\n";
// 				}
// 				for(int i = 0; i < num_events; ++i) {
// 					int action = 0;
// 					if(events[i].events & EPOLLIN)
// 						action |= CURL_CSELECT_IN;
// 					if(events[i].events & EPOLLOUT)
// 						action |= CURL_CSELECT_OUT;
// 					if(events[i].events & (EPOLLERR | EPOLLHUP))
// 						action |= CURL_CSELECT_ERR;
// 					curl_multi_socket_action(multi_handle, events[i].data.fd, action, &still_running);
// 					//std::cerr << "Event processed on fd " << events[i].data.fd << "\n";
// 				}
// 			}
// 		});

//     }
//     else {
//         throw std::runtime_error("Failed to initialize CURL easy handle");
//     }
// }

// // Добавляем в деструктор:
// stream_client::~stream_client() {
//     running.store(false);
//     if(worker_thread.joinable()) worker_thread.join();
//     close(epoll_fd);
//     curl_multi_cleanup(multi_handle);
//     // ... существующий код ...
// }

// ////////////////////////////////////////////////////////////////////////////////////////////////////////////
// /*


// */
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////


// void stream_client::process_messages() {
//     CURLMsg* msg;
//     int msgs_left;
//     while ((msg = curl_multi_info_read(multi_handle, &msgs_left))) 
//     {
//         if (msg->msg == CURLMSG_DONE) 
//         {
//             long http_code = 0;
//             curl_easy_getinfo(msg->easy_handle, CURLINFO_RESPONSE_CODE, &http_code);
//             //std::cerr << "Request completed with HTTP code: " << http_code << std::endl;
//         }
//     }
// }

// int stream_client::socket_callback(CURL* easy, curl_socket_t s, int action,
//                                   void* userp, void* socketp) {
//     stream_client* self = static_cast<stream_client*>(userp);
//     struct epoll_event event;
//     event.events = 0;
//     event.data.fd = s;

//     if(action == CURL_POLL_REMOVE) {
//         epoll_ctl(self->epoll_fd, EPOLL_CTL_DEL, s, nullptr);
//         return 0;
//     }

//     if(action & CURL_POLL_IN) event.events |= EPOLLIN;
//     if(action & CURL_POLL_OUT) event.events |= EPOLLOUT;

//     int op = socketp ? EPOLL_CTL_MOD : EPOLL_CTL_ADD;
//     fcntl(s, F_SETFL, fcntl(s, F_GETFL) | O_NONBLOCK);
//     epoll_ctl(self->epoll_fd, op, s, &event);
//     return 0;
// }

// int stream_client::timer_callback(CURLM* multi, long timeout_ms, void* userp) {
//      stream_client* self = static_cast<stream_client*>(userp);
//     // Для обработки таймаутов можно игнорировать, если используется epoll_wait с таймаутом
//     return 0;
// }

// int stream_client::get(const std::string &_url, const std::string &_filename) {

//     if (_filename != "") {
//         this->response->file.open(_filename.c_str(), std::ios::binary);
//         if (!this->response->file.is_open()) {
//             //std::cerr << "Failed to open file: " << _filename << std::endl;
//             return -1;
//         }
//         this->response->is_file = true;
//         this->response->filename = _filename;
//     }

	

//       struct curl_slist* headers = nullptr;
//     for (const auto& [key, value] : request->headers) {
//         headers = curl_slist_append(headers, (key + ": " + value).c_str());
//     }
//     curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);


//     CURLcode res;
//     res = curl_easy_setopt(curl, CURLOPT_URL, _url.c_str());
//     if (res != CURLE_OK) {
//         if (this->response->is_file) {
//             this->response->file.close();
//         }
//         //std::cerr << "curl_easy_setopt(CURLOPT_URL) failed: " << curl_easy_strerror(res) << std::endl;
//         return -1;
//     }
// 	std::cerr << "get2\n";
//     res = curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
//     if (res != CURLE_OK) {
//         if (this->response->is_file) {
//             this->response->file.close();
//         }
//         //std::cerr << "curl_easy_setopt(CURLOPT_HTTPGET) failed: " << curl_easy_strerror(res) << std::endl;
//         return -1;
//     }
//     CURLMcode mres = curl_multi_add_handle(multi_handle, curl);
// 	curl_multi_socket_action(multi_handle, CURL_SOCKET_TIMEOUT, 0, &still_running);
//     if (mres != CURLM_OK) {
//         if (this->response->is_file) {
//             this->response->file.close();
//         }
//         //std::cerr << "curl_multi_add_handle failed: " << curl_multi_strerror(mres) << std::endl;
//         return -1;
//     }

//     return 0;
// }

// ////////////////////////////////////////////////////////////////////////////////////////////////////////////
// /*


// */
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////


// bool stream_client::set_header(const std::string &header, const std::string &value)
// {
// 	if(value == "")
// 	{
// 		this->request->headers.erase(header);
// 	}
// 	else
// 	{
// 		this->request->headers.insert_or_assign(header,value);
// 	}
// 	return true;
// };


// ////////////////////////////////////////////////////////////////////////////////////////////////////////////
// /*


// */
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////


// bool stream_client::set_ssl(const std::string &ssl_key_path,const std::string &ssl_sert_path,bool is_check)
// {
// 	CURLcode res;

// 	std::cout << "ssl_sert_path = [" << ssl_sert_path << "]" << std::endl;
// 	std::cout << "ssl_key_path  = [" << ssl_key_path << "]" << std::endl;

// 	res = curl_easy_setopt(this->curl, CURLOPT_SSLCERT, ssl_sert_path);
// 	if(res != CURLE_OK)
// 	{
// 		std::cout << "CURLOPT_SSLCERT res = " << res << std::endl;
// 	}

// 	res = curl_easy_setopt(this->curl, CURLOPT_SSLKEY, ssl_key_path);
// 	if(res != CURLE_OK)
// 	{
// 		std::cout << "CURLOPT_SSLKEY res = " << res << std::endl;
// 	}

// 	if(is_check == true)
// 	{
// 		curl_easy_setopt(this->curl, CURLOPT_SSL_VERIFYHOST, 1L);
// 		curl_easy_setopt(this->curl, CURLOPT_SSL_VERIFYPEER, 1L);
// 	}
// 	else
// 	{
// 		curl_easy_setopt(this->curl, CURLOPT_SSL_VERIFYHOST, 0L);
// 		curl_easy_setopt(this->curl, CURLOPT_SSL_VERIFYPEER, 0L);
// 	}

// 	return true;
// };


// ////////////////////////////////////////////////////////////////////////////////////////////////////////////
// /*


// */
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////


// bool stream_client::set_sslp12(const std::string &ssl_sert_path,const std::string &ssl_sert_password,bool is_check)
// {
// 	CURLcode res;

// 	res = curl_easy_setopt(this->curl, CURLOPT_SSLCERT, ssl_sert_path.c_str());
// 	if(res != CURLE_OK)
// 	{
// 		std::cout << "CURLOPT_SSLCERT res = " << res << std::endl;
// 	}

// 	res = curl_easy_setopt(this->curl, CURLOPT_SSLCERTTYPE, "p12");
// 	if(res != CURLE_OK)
// 	{
// 		std::cout << "CURLOPT_SSLCERTTYPE res = " << res << std::endl;
// 	}

// 	if(ssl_sert_password != "")
// 	{
// 		curl_easy_setopt(this->curl, CURLOPT_KEYPASSWD, ssl_sert_password.c_str());
// 	}

// 	if(is_check == true)
// 	{
// 		curl_easy_setopt(this->curl, CURLOPT_SSL_VERIFYHOST, 1L);
// 		curl_easy_setopt(this->curl, CURLOPT_SSL_VERIFYPEER, 1L);
// 		curl_easy_setopt(this->curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);

// 	}
// 	else
// 	{
// 		curl_easy_setopt(this->curl, CURLOPT_SSL_VERIFYHOST, 0L);
// 		curl_easy_setopt(this->curl, CURLOPT_SSL_VERIFYPEER, 0L);
// 		curl_easy_setopt(this->curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);

// 	}

// 	return true;
// };


// ////////////////////////////////////////////////////////////////////////////////////////////////////////////
// /*


// */
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////


// bool stream_client::set_proxy(const std::string &_addr,const std::string &_port)
// {
// 	std::string proxy = _addr + ":" + _port;

// 	std::cout << "[" << proxy << "]" << std::endl;

// 	curl_easy_setopt(this->curl, CURLOPT_PROXY, proxy.c_str());
// 	return true;
// };


// ////////////////////////////////////////////////////////////////////////////////////////////////////////////
// /*


// */
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////


// bool stream_client::set_proxy(const tegia::http::proxy &_proxy = tegia::http::proxy())
// {
// 	return true;
// };


// ////////////////////////////////////////////////////////////////////////////////////////////////////////////
// /*


// */
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////


// bool stream_client::set_useragent(const std::string &_useragent)
// {
// 	this->request->useragent = _useragent;
// 	curl_easy_setopt(this->curl, CURLOPT_USERAGENT,this->request->useragent.c_str());
// 	return true;
// };


// ////////////////////////////////////////////////////////////////////////////////////////////////////////////
// /*


// */
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////


// std::string stream_client::get_redirect_url()
// {
// 	char *redirect_url = nullptr;
// 	curl_easy_getinfo (this->curl, CURLINFO_REDIRECT_URL, &redirect_url);
// 	return redirect_url;
// };


// ////////////////////////////////////////////////////////////////////////////////////////////////////////////
// /*


// */
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////


// bool stream_client::set_cookie(const std::string &_name, const std::string &_value, const std::string &_path)
// {
// 	tegia::http::cookie _cookie;
// 	_cookie.name = _name;
// 	_cookie.value = _value;
// 	_cookie.path = _path;
// 	this->cookielist.insert({_cookie.name + _cookie.path,_cookie});
// 	return true;
// };


// ////////////////////////////////////////////////////////////////////////////////////////////////////////////
// /*


// */
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////


// int stream_client::run()
// {
// 	CURLcode res;
// 	this->response->step = 0;
// 	this->response->status = 0;
// 	this->response->raw_headers.clear();
// 	this->response->headers.clear();
// 	this->response->current = 0;
// 	this->response->total = 0;

// 	//
// 	// HEADERS
// 	//
// 	struct curl_slist * _headers = nullptr;
// 	for(auto it = this->request->headers.begin();it != this->request->headers.end();it++)
// 	{
// 		_headers = curl_slist_append(_headers, std::string(it->first + ": " + it->second).c_str());  
// 	}
// 	curl_easy_setopt(this->curl, CURLOPT_HTTPHEADER, _headers);

// 	//
// 	// COOKIE
// 	//

// //		std::cout << this->request->scheme << std::endl;
// //		std::cout << this->request->host << std::endl;
// //		std::cout << this->request->path << std::endl;

// 	if(this->cookielist.size() > 0)
// 	{
// 		//
// 		// Отправлять только те cookie, которые соотвесттвуют запросу
// 		//

// 		std::string str_cookie = "";
// 		for(auto it = this->cookielist.begin(); it != this->cookielist.end(); it++)
// 		{
// 			size_t len = it->second.path.length();
// 			if(this->request->path.substr(0,len) == it->second.path)
// 			{
// 				str_cookie = str_cookie + it->second.name + "=" + it->second.value + ";";

// 				if(this->verbose == true)
// 				{
// 					it->second.print();
// 					std::cout << _OK_TEXT_ << "send cookie" << std::endl;
// 				}
// 			}
// 			else
// 			{
// 				if(this->verbose == true)
// 				{
// 					it->second.print();
// 					std::cout << _ERR_TEXT_ << "not send cookie" << std::endl;
// 				}
// 			}
// 		}

// 		curl_easy_setopt(this->curl, CURLOPT_COOKIELIST, "ALL");
// 		curl_easy_setopt(this->curl, CURLOPT_COOKIE, str_cookie.c_str());
// 	}

// 	//
// 	// RUN QUERY
// 	//

// 	curl_easy_perform(this->curl);
// 	return 1;
// }

////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*


*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////

}  // END namespace http
}  // END namespace tegia	


