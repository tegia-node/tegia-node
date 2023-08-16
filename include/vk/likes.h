#ifndef _H_VK_API_LIKES_
#define _H_VK_API_LIKES_

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
namespace likes {     


////////////////////////////////////////////////////////////////////////////////////////////
/** 


*/   
////////////////////////////////////////////////////////////////////////////////////////////
	

// https://dev.vk.com/method/likes.getList

std::tuple<int,nlohmann::json> get(
		const std::string &key,
		const std::string &owner_id,
		const std::string &type,
		const std::string &item_id,
		const long long int &offset,
		const long long int &count,
		const tegia::http::proxy &_proxy = tegia::http::proxy())
{
	std::string post =	"v=" + vk_api_version + 
						"&access_token=" + key + 
						"type=" + type +
						"&owner_id=" + owner_id +
						"&item_id=" + item_id + 
						"&offset=" + core::cast<std::string>(offset) + 
						"&count=" + core::cast<std::string>(count);
	return vk::run2("likes.getList", post, _proxy);
};


		
}; // end likes namespace
}; // end vk namespace

#endif 

