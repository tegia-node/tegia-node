#ifndef H_CORE
#define H_CORE
// -------------------------------------------------------------------------------------- //
//                                      INCLUDES                                          //
// -------------------------------------------------------------------------------------- //

#include <string>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <sstream>
#include <fstream>

#include <unistd.h>
#include <vector>
#include <map>
#include <ctime>  // C++
#include <iomanip>
#include <algorithm>
#include <uuid/uuid.h>  //  
#include <sys/stat.h>   //  for stat
#include <curl/curl.h>

#include <thread>
#include <chrono>
using namespace std::chrono_literals;

// -------------------------------------------------------------------------------------- //
//                                      STRUCTURES                                        //
// -------------------------------------------------------------------------------------- //

namespace lua
{
	 inline void print(const std::string &str)
	 {
			std::cout << str << std::endl;
	 };
}  // end namespase lua


// -------------------------------------------------------------------------------------- //
//                                      STRUCTURES                                        //
// -------------------------------------------------------------------------------------- //



namespace core
{

	std::string read_from_file(const std::string& path);

	[[deprecated("use tegia::random::uuid()")]]
	std::string generateUUID();

	unsigned int timestamp(const std::string &str, const std::string &format = "%4d-%2d-%2dT%2d:%2d:%2d.%3d");



/** ================================================================================================

		Флаг empty показывает, включать ли в массив пустые строки: 
			если empty = true, то пустые строки включаются в массив
			если empty = false, то пустые строки не включаются в массив

**/
	inline  std::vector<std::string> explode(std::string source, std::string separator, bool empty = true)
	{
		std::vector<std::string> out;

		std::string::size_type pos = 0;
		std::string::size_type find_pos;

		find_pos = source.find(separator, pos);

		while(find_pos != source.npos)
		{
			if(pos != find_pos)
			{
				out.push_back(source.substr(pos, find_pos - pos));
			}
			else
			{
				if(empty == true)
				{
						out.push_back(""); 
				}
			}
			pos = find_pos + separator.length();
			find_pos = source.find(separator, pos);
		}

		std::string tmp = source.substr(pos);
		out.push_back(source.substr(pos));

		return out;
	};  


	inline std::string implode(std::vector<std::string> source, std::string separator)
	{
		std::string out;

		 for(auto it = source.begin(); it != source.end(); it++)
		 {
			 out = out + (*it) + separator;
		 }

		 out = out.substr(0,out.size() - separator.size());

		 return out;
	};


/** ================================================================================================

*/
	inline int setbit(int & value, int index, bool bit)
	{
		if(bit == 1)
		{
			value|=(1<<index);
		}
		else
		{
			value&=~(1<<index); 
		}
		return value;
	}

}  // end namespace core





////////////////////////////////////////////////////////////////////////////////////////////
/** 
 
*/   
////////////////////////////////////////////////////////////////////////////////////////////
		



namespace core
{

namespace url
{

// =================================================================================================

	[[deprecated("use tegia::http::escape()")]]
	inline std::string encode(const std::string &url)
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

		return returnstrig;
	}

	
// =================================================================================================
	[[deprecated("use tegia::http::unescape()")]]
	inline std::string decode(const std::string &url)
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

		return returnstrig;
	}


}  // end namespace url
}  // end namespace core


namespace tegia
{
   using namespace std::literals::string_literals;

   inline std::string const CONFIG_MESSAGE_DATA = "config"s;
   inline std::string const ACTION_MESSAGE_DATA = "message"s;
   inline std::string const NODE_UUID = "uuid"s;
   inline std::string const ADD_ACTOR_LIST = "add_actors"s;
   inline std::string const HANDSHAKE_MESSAGE = "handshake"s;
   inline std::string const REMOVE_ACTOR_LIST = "remove_actors"s;
   inline std::string const WELCOME_DATA = "welcome"s;
   inline std::string const ACTION_MESSAGE = "body"s;
   inline std::string const ACTOR_NAME = "actor"s;
   inline std::string const ACTION_NAME = "action"s;
   inline std::string const ACTION_PRIORITY = "priority"s;

}



#endif