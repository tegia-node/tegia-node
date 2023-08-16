#ifndef _H_VK_API_WALL_
#define _H_VK_API_WALL_

// -------------------------------------------------------------------------------------- //
//                                      INCLUDES                                          //
// -------------------------------------------------------------------------------------- //

// STL 
	#include <string>
	#include <iostream>
	#include <sstream>


	#include "../core/http.h"
	#include "../core/http2.h"
	#include "../core/json.h"

	#include "vk_api.h"

namespace vk { 
namespace wall {     


////////////////////////////////////////////////////////////////////////////////////////////
/** 


*/   
////////////////////////////////////////////////////////////////////////////////////////////
	


std::tuple<int,nlohmann::json> get(
		const std::string &key,
		const long long int &owner_id,
		const long long int &offset,
		const long long int &count,
		const tegia::http::proxy &_proxy = tegia::http::proxy())
{
	std::string post =	"v=" + vk_api_version + 
						"&access_token=" + key + 
						"&owner_id=" + core::cast<std::string>(owner_id) +
						"&extended=1" +  
						"&offset=" + core::cast<std::string>(offset) + 
						"&count=" + core::cast<std::string>(count);
	return vk::run2("wall.get", post, _proxy);
};



std::tuple<int,nlohmann::json> comments(
		const std::string &key,
		const long long int &owner_id,
		const long long int post_id,
		const long long int comment_id,
		const long long int &offset,
		const long long int &count,
		const tegia::http::proxy &_proxy = tegia::http::proxy())
{
	std::string post = "";

	if(comment_id > 0)
	{
		post =	"v=" + vk_api_version + 
				"&access_token=" + key + 
				"&owner_id=" + core::cast<std::string>(owner_id) +
				"&post_id=" + core::cast<std::string>(post_id) + 
				"&comment_id=" + core::cast<std::string>(comment_id) +
				"&need_likes=1"
				"&sort=desc"
				"&preview_length=0"
				"&offset=" + core::cast<std::string>(offset) + 
				"&count=" + core::cast<std::string>(count);
	}
	else
	{
		post =	"v=" + vk_api_version + 
				"&access_token=" + key + 
				"&owner_id=" + core::cast<std::string>(owner_id) +
				"&post_id=" + core::cast<std::string>(post_id) + 
				"&need_likes=1"
				"&sort=desc"
				"&preview_length=0"
				"&offset=" + core::cast<std::string>(offset) + 
				"&count=" + core::cast<std::string>(count);
	}

	return vk::run2("wall.getComments", post, _proxy);
};


		
}; // end wall namespace
}; // end vk namespace

#endif 

