#ifndef _H_VK_API_GROUP_GETMEMBERS_
#define _H_VK_API_GROUP_GETMEMBERS_

// -------------------------------------------------------------------------------------- //
//                                      INCLUDES                                          //
// -------------------------------------------------------------------------------------- //

// STL 
	 #include <string>
	 #include <iostream>
	 #include <sstream>


	 // #include "../http.h"
	 #include "../core/http.h"
	 #include "../core/http2.h"
	 #include "../core/json.h"

	 #include "vk_api.h"


namespace vk { 
namespace groups {     


////////////////////////////////////////////////////////////////////////////////////////////
/** 

	\brief Возвращает информацию о заданном сообществе или о нескольких сообщества

	https://vk.com/dev/groups.getById

*/   
////////////////////////////////////////////////////////////////////////////////////////////
		
std::tuple<int,nlohmann::json> get(
	const std::string &key,
	const std::string &group_ids,
	const tegia::http::proxy &_proxy = tegia::http::proxy())
{
	std::string post = "v=" + vk_api_version + "&group_ids=" + 
			group_ids + "&access_token=" + key + 
			"&fields=type,deactivated,is_closed,age_limits,ban_info,country,city,contacts,description,site,status,verified,members_count,market";  

	return vk::run2("groups.getById", post, _proxy);
};




////////////////////////////////////////////////////////////////////////////////////////////
/** 
 * 
	\brief Возвращает список участников сообщества

	https://vk.com/dev/groups.getMembers

*/   
////////////////////////////////////////////////////////////////////////////////////////////

std::tuple<int,nlohmann::json> members(
	const std::string &key,
	const std::string &group_id,
	const long long int &offset,
	const long long int &count,
	const tegia::http::proxy &_proxy = tegia::http::proxy() )
{
	std::string post = "v=" + vk_api_version + 
						"&access_token=" + key + 
						"&group_id=" + group_id + 
						"&offset=" + core::cast<std::string>(offset) + 
						"&count=" + core::cast<std::string>(count) + "&sort=id_asc";
	return vk::run2("groups.getMembers", post, _proxy);
};

////////////////////////////////////////////////////////////////////////////////////////////
/** 
 * 
	\brief Возвращает список участников сообщества

	https://vk.com/dev/groups.getMembers

*/   
////////////////////////////////////////////////////////////////////////////////////////////

std::tuple<int,nlohmann::json> members_full(
	const std::string &key,
	const std::string &group_id,
	const long long int &offset,
	const long long int &count,
	const tegia::http::proxy &_proxy = tegia::http::proxy())
{
	std::string post = "v=" + vk_api_version + 
						"&access_token=" + key + 
						"&group_id=" + group_id + 
						"&offset=" + core::cast<std::string>(offset) + 
						"&fields=" + USER_FIELDS + 
						"&count=" + core::cast<std::string>(count) + "&sort=id_asc";
	return vk::run2("groups.getMembers", post, _proxy);
};




long long int format(nlohmann::json *data)
{
	nlohmann::json model;
	model["vk"] = (*data);
	model["model"] = nlohmann::json::object();

	//
	// STATUS
	//
	//	0 - deleted
	//	1 - active
	//	2 - banned
	//	3 - closed
	//  4 - private
	//	5 - geo_blocked
	//

	model["model"]["status"] = 1;

	if(model["vk"].contains("deactivated") == true)
	{
		std::string _status = model["vk"]["deactivated"].get<std::string>();
		bool flag = false;

		//
		// Группа заблокирована
		//

		if(_status == "banned")
		{
			model["model"]["status"] = 2;
			flag = true;
		}

		//
		// Группа удалена
		//

		if(_status == "deleted")
		{
			model["model"]["status"] = 0;
			flag = true;			
		}

		//
		// Этот материал заблокирован на территории РФ на основании требования Генеральной прокуратуры РФ
		//

		if(_status == "geo_blocked")
		{
			model["model"]["status"] = 5;
			flag = true;
		}

		//
		// Неизвестный статус
		//

		if(flag == false)
		{
			std::cout << _ERR_TEXT_ << "unknown group state " << _status << std::endl;
			std::cout << model.dump() << std::endl; 
			exit(0);
		}
	}

	// is_closed
	//  0 — открытое;
	//  1 — закрытое;
	//  2 — частное.

	int is_closed = model["vk"]["is_closed"].get<int>();
	switch(is_closed)
	{
		case 0: break;
		case 1: model["model"]["status"] = 3; break;
		case 2: model["model"]["status"] = 4; break;
	}

	//
	// MEMBERS
	//

	if(model["vk"].contains("members_count") == true)
	{
		model["vk"]["members"] = model["vk"]["members_count"].get<long long int>();
	}
	else
	{
		model["vk"]["members"] = 0;
	}

	//
	// ID GROUP
	//

	(*data) = model;
	return model["vk"]["id"].get<long long int>();

};





			
};  // end groups namespace



}; // end vkssss namespace



#endif 

