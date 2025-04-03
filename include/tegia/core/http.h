#ifndef H_CORE_HTTP
#define H_CORE_HTTP

// ------------------------------------------------------------------------------------------

#include <iostream>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <unordered_map>
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>

#include <stdlib.h>
#include <cstdio>
#include <signal.h>
#include <execinfo.h>

#include "const.h"
#include "json.h"
#include <curl/curl.h>
#include <algorithm>
#include <queue>

#include <tegia/core/cast.h>
#include <tegia/core/string.h>

struct FILE_DATA
{
	long long int total = 1;
	long long int current = 1;
	std::string filename;
	std::ofstream file;

	std::unordered_map<std::string,std::string> headers;
	std::string raw_headers;
};

#define _PRINT_ true


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
///////////////////////////////////////////////////////////////////////////////////////////////////////////////


namespace tegia {
namespace http {

class url_t
{
	public:
		url_t() = default;
		~url_t() = default;

		int parse(const std::string &url);

		std::string raw;
		std::string scheme;
		std::string host;
		std::string path;
};

} // END namespace http
} // END namespace tegia


namespace tegia {
namespace http {


	inline std::string escape(const std::string &url)
	{
		std::string returnstrig;

		CURL *curl = curl_easy_init();
		if(curl) 
		{
			char *output = curl_easy_escape(curl, url.c_str(), url.length());
			if(output) 
			{
				//printf("Encoded: %s\n", output);
				returnstrig = std::string(output);
				curl_free(output);
			}
			curl_easy_cleanup(curl);
		} 

		return std::move(returnstrig);
	}

	inline std::string unescape(const std::string &url)
	{
		std::string returnstrig;

		CURL *curl = curl_easy_init();
		if(curl) 
		{
			int outlen = 0;
			char *output = curl_easy_unescape(curl, url.c_str(), url.length(), &outlen);
			if(output) 
			{
				//printf("Encoded: %s\n", output);
				returnstrig = std::string(output);
				curl_free(output);
			}
		} 

		return std::move(returnstrig);
	}	

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
///////////////////////////////////////////////////////////////////////////////////////////////////////////////


class client;
class stream_client;

struct cookie
{
	std::string host;
	std::string is_subdomains;
	std::string path;
	std::string secure;
	std::string expiry;
	std::string name;
	std::string value;
	std::string _raw;

	void print()
	{
		std::cout << " Host:               " << host << std::endl;
		std::cout << " Include subdomains: " << is_subdomains << std::endl;
		std::cout << " Path:               " << path << std::endl;
		std::cout << " Secure:             " << secure << std::endl;
		std::cout << " Expiry:             " << expiry << std::endl;
		std::cout << " Name:               " << name << std::endl;
		std::cout << " Value:              " << value << std::endl;
	};
};



struct proxy
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



class request
{
	friend class client;
	friend class stream_client;

	private:
		std::unordered_map<std::string,std::string> headers;

		std::string useragent;

		std::string url; 
		std::string scheme;
		std::string host;
		std::string path;


		int parseurl(const std::string &_url);

	public:
		request()
		{
			this->useragent = "Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:107.0) Gecko/20100101 Firefox/107.0";
		};
		~request(){};

};


class response
{
	friend class client;

	private:
		int step = 0; // Этап загрузки данных

		std::unordered_map<std::string,cookie> cookielist;
		
		long long int total = 1;
		long long int current = 1;
		bool is_file = false;		// Сохранять результаты в файл или нет
									// is_file = true  - данные сохраняются в файл
									// is_file = false - данные сохраняются в data 
		std::string filename;
		std::ofstream file;

		std::string raw_headers;

	public:
		std::string data;
		int status;

		std::unordered_map<std::string,std::string> headers;
};


class stream_response
{
	friend class client;
	friend class stream_client;

	private:
		int step = 0; // Этап загрузки данных

		std::unordered_map<std::string,cookie> cookielist;
		
		long long int total = 1;
		long long int current = 1;
		bool is_file = false;		// Сохранять результаты в файл или нет
									// is_file = true  - данные сохраняются в файл
									// is_file = false - данные сохраняются в data 
		std::string filename;
		std::ofstream file;

		std::string raw_headers;
		std::queue<std::string> response_queue;
    	std::mutex mtx;
    	std::condition_variable cv;
    	std::atomic<bool> done = false;

	public:
		
		void push(const std::string &data) {
			std::lock_guard<std::mutex> lock(mtx);
			response_queue.push(data);
			cv.notify_one();
		}

		// Блокирующий метод для извлечения данных
		// Возвращает true, если данные получены, и false, если поток завершает работу
		bool pop(std::string &data) {
			std::unique_lock<std::mutex> lock(mtx);
			cv.wait(lock, [&] { return !response_queue.empty() || done.load(std::memory_order_acquire); });
			if (!response_queue.empty()) {
				data = response_queue.front();
				response_queue.pop();
				return true;
			}
			return false;
		}

		// Метод для уведомления об окончании передачи данных
		void set_done() {
			std::lock_guard<std::mutex> lock(mtx);
			done.store(true, std::memory_order_release);
        	cv.notify_all();
		}
	
		int status;

		std::unordered_map<std::string,std::string> headers;
};



/**
   \class query Реализует клиент для выполнения http-запросов
*/
class client
{
	private:
		CURL *curl;
		// std::string useragent = "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/105.0.0.0 Safari/537.36";
		std::unordered_map<std::string,cookie> cookielist;

		bool verbose = false;
		
		int run();

	public:
		client();
		~client();
		int get(const std::string &_url, const std::string &_filename = "");
		int post(const std::string &_url, const std::string &_data);

		
		tegia::http::request * request;
		tegia::http::response * response;


		bool set_timeout(long seconds);
		bool set_verbose(bool flag);
		bool set_header(const std::string &header, const std::string &value);
		bool set_ssl(const std::string &ssl_key_path,const std::string &ssl_sert_path,bool is_check);
		bool set_sslp12(const std::string &ssl_sert_path,const std::string &ssl_sert_password,bool is_check);
		bool set_proxy(const std::string &_addr,const std::string &_port);
		bool set_proxy(const tegia::http::proxy &_proxy);
		bool set_useragent(const std::string &_useragent);
		bool set_cookie(const std::string &_name, const std::string &_value, const std::string &_path);


		std::string get_redirect_url();




		static std::size_t writedata(
				const char* in,
				std::size_t size,
				std::size_t num,
				std::string* out)
		{
			const std::size_t totalBytes(size * num);
			tegia::http::response * _response = reinterpret_cast<tegia::http::response*>(out);

			if(_response->step == 1)
			{
				// std::cout << _OK_TEXT_ << "start load data" << std::endl;
				_response->total = core::cast<long long int>(_response->headers["content-length"]);
				_response->step = 2;
			}

			if(_response->is_file == true)
			{
				_response->current = _response->current + totalBytes;
				_response->file.write(in, totalBytes);
				_response->file.flush();
				//std::cout << "\rloaded [" << floor( ((double) _response->current / (double) _response->total) * 100) << "%] [" << _response->current << "/" << _response->total << "] bytes" << std::flush;
			}
			else
			{
				_response->data.append(in, totalBytes);
			}

			return totalBytes;
		}


		static std::size_t writeheader(
			const char* in,
			std::size_t size,
			std::size_t num,
			void* out)
	{
		const std::size_t totalBytes(size * num);
		tegia::http::response * _response = reinterpret_cast<tegia::http::response*>(out);
		_response->raw_headers.append(in, totalBytes);

		if(_response->step == 0)
		{
			// std::cout << _OK_TEXT_ << "start load header" << std::endl;
			_response->step = 1;
		}

		auto vhead = tegia::string::explode( std::string(in,totalBytes),": ",false);
		if(vhead.size() == 2)
		{
			_response->headers.insert({ tegia::string::to_lower(vhead[0]),core::string::trim(vhead[1])});
		}

		/*
		else
		{
			if(vhead[0] == "\r\n" || vhead[0] == "\r\n\r\n")
			{
				std::cout << _OK_TEXT_ << "END HEADER\n";
			}
			else
			{
				_response->headers.insert({vhead[0],""});
			}
		}*/
		
		return totalBytes;
	};


};


class stream_client
{
	private:
		CURL *curl;
		std::unordered_map<std::string,cookie> cookielist;

		bool verbose = false;
		static std::atomic<bool> is_stopped;
		int run();

		static int xfer_info_callback(
			void* clientp,
			curl_off_t dltotal,
			curl_off_t dlnow,
			curl_off_t ultotal,
			curl_off_t ulnow) {
			std::atomic<bool>* data = static_cast<std::atomic<bool>*>(clientp);
			return data->load(std::memory_order_relaxed) ? 1 : 0; // 1 = прервать
		}
		

	public:
		stream_client();
		~stream_client();
		int get(const std::string &_url, const std::string &_filename = "");
		void stop_streaming();

		
		tegia::http::request * request;
		tegia::http::stream_response * response;


		bool set_timeout(long seconds);
		bool set_verbose(bool flag);
		bool set_header(const std::string &header, const std::string &value);
		bool set_ssl(const std::string &ssl_key_path,const std::string &ssl_sert_path,bool is_check);
		bool set_sslp12(const std::string &ssl_sert_path,const std::string &ssl_sert_password,bool is_check);
		bool set_proxy(const std::string &_addr,const std::string &_port);
		bool set_proxy(const tegia::http::proxy &_proxy);
		bool set_useragent(const std::string &_useragent);
		bool set_cookie(const std::string &_name, const std::string &_value, const std::string &_path);


		std::string get_redirect_url();




		static std::size_t writedata(
				const char* in,
				std::size_t size,
				std::size_t num,
				void* out)
		{
			const std::size_t totalBytes(size * num);
			tegia::http::stream_response * _response = reinterpret_cast<tegia::http::stream_response*>(out);

			if(_response->step == 1)
			{
				// std::cout << _OK_TEXT_ << "start load data" << std::endl;
				_response->total = core::cast<long long int>(_response->headers["content-length"]);
				_response->step = 2;
			}

			if(_response->is_file == true)
			{
				_response->current = _response->current + totalBytes;
				_response->file.write(in, totalBytes);
				_response->file.flush();
			}
			else
			{
				_response->push(std::string(in, totalBytes));
			}
			
			if(is_stopped.load())
			{

			}	

			return totalBytes;
		}


		static std::size_t writeheader(
			const char* in,
			std::size_t size,
			std::size_t num,
			void* out)
	{
		const std::size_t totalBytes(size * num);
		tegia::http::stream_response * _response = reinterpret_cast<tegia::http::stream_response*>(out);
		_response->raw_headers.append(in, totalBytes);

		if(_response->step == 0)
		{
			_response->step = 1;
		}

		auto vhead = tegia::string::explode( std::string(in,totalBytes),": ",false);
		if(vhead.size() == 2)
		{
			_response->headers.insert({ tegia::string::to_lower(vhead[0]),core::string::trim(vhead[1])});
		}
		
		return totalBytes;
	};



};

}  // END namespace http
}  // END namespace tegia

#endif