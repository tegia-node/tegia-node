#ifndef _H_VK_API_PHOTOS_
#define _H_VK_API_PHOTOS_

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
namespace photos {     


////////////////////////////////////////////////////////////////////////////////////////////
/** 


*/   
////////////////////////////////////////////////////////////////////////////////////////////
	


std::tuple<int,nlohmann::json> get(
		const std::string &key,
		const std::string &owner_id,
		const long long int &offset,
		const long long int &count,
		const tegia::http::proxy &_proxy = tegia::http::proxy())
{
	std::string post =	"v=" + vk_api_version + 
						"&access_token=" + key + 
						"&owner_id=" + owner_id + 
						"&offset=" + core::cast<std::string>(offset) + 
						"&count=" + core::cast<std::string>(count);
	return vk::run2("photos.getAll", post, _proxy);
};


			
}; // end photos namespace
}; // end vk namespace

#endif 

