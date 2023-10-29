#include <tegia/core/http.h>

namespace tegia {
namespace http {



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
		auto arr = core::explode(each->data,"\t");
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






}  // END namespace http
}  // END namespace tegia	


