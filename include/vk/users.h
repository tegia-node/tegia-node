#ifndef _H_VK_API_USERS_
#define _H_VK_API_USERS_

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
namespace users {     


////////////////////////////////////////////////////////////////////////////////////////////
/** 


*/   
////////////////////////////////////////////////////////////////////////////////////////////
	

std::tuple<int,nlohmann::json> get(
        const std::string &key,
        const std::string &user_ids,
		const tegia::http::proxy &_proxy = tegia::http::proxy())
{
    std::string post =  "v=" + vk_api_version + 
                        "&access_token=" + key + 
                        "&user_ids=" + user_ids +
                        "&fields=" + USER_FIELDS;
    return vk::run2( "users.get", post, _proxy);
};


////////////////////////////////////////////////////////////////////////////////////////////
/** 


*/   
////////////////////////////////////////////////////////////////////////////////////////////
	

std::tuple<int,nlohmann::json> followers(
        const std::string &key,
        const std::string &user_id,
        const long long int &offset,
        const long long int &count,
		const tegia::http::proxy &_proxy = tegia::http::proxy())
{
	std::string post = "v=" + vk_api_version + 
					"&access_token=" + key + 
					"&user_id=" + user_id + 
					"&offset=" + core::cast<std::string>(offset) + 
					"&count=" + core::cast<std::string>(count) + 
					"&fields=" + USER_FIELDS;
	return vk::run2("users.getFollowers", post, _proxy);
};


////////////////////////////////////////////////////////////////////////////////////////////
/** 


*/   
////////////////////////////////////////////////////////////////////////////////////////////
	

std::tuple<int,nlohmann::json> friends(
        const std::string &key,
        const long long int &user_id,
        const long long int &offset,
        const long long int &count,
		const tegia::http::proxy &_proxy = tegia::http::proxy())
{
	std::string post = "v=" + vk_api_version + 
						"&access_token=" + key + 
						"&user_id=" + core::cast<std::string>(user_id) + 
						"&offset=" + core::cast<std::string>(offset) + 
						"&count=" + core::cast<std::string>(count);
	return vk::run2("friends.get", post, _proxy);
};



////////////////////////////////////////////////////////////////////////////////////////////
/** 


*/
////////////////////////////////////////////////////////////////////////////////////////////



std::tuple<int,nlohmann::json> search(
        const std::string &key,
        const std::string &q,
        const std::string &t_birth,
		const tegia::http::proxy &_proxy = tegia::http::proxy())
{
	std::string birth_year = "";
	std::string birth_month = "";
	std::string birth_day = "";

	if(t_birth.length() == 10)
	{
		// 2100-01-01
		birth_year  = t_birth.substr(0,4);
		birth_month = t_birth.substr(5,2);
		birth_day   = t_birth.substr(8,2);
	}

	std::cout << "t_birth     = " << t_birth << std::endl;
	std::cout << "birth_year  = " << birth_year << std::endl;
	std::cout << "birth_month = " << birth_month << std::endl;
	std::cout << "birth_day   = " << birth_day << std::endl;

	std::string post = "v=" + vk_api_version + 
						"&access_token=" + key + 
						"&q=" + tegia::http::escape(q) + 
						"&count=1000" + 
						"&birth_day=" + birth_day + 
						"&birth_month=" + birth_month +
						"&birth_year=" + birth_year;
	return vk::run2("users.search", post, _proxy);
};




////////////////////////////////////////////////////////////////////////////////////////////
/** 


*/   
////////////////////////////////////////////////////////////////////////////////////////////


void check_avatar_url(const std::string &name, nlohmann::json *data)
{
	if( (*data).contains(name) == true)
	{
		std::string url = (*data)[name].get<std::string>();
		auto found = url.find(".userapi.com");
		if(found != std::string::npos)
		{
			url = "https://pp" + url.substr(found);
		}
		(*data)[name] = url;
	}
};


////////////////////////////////////////////////////////////////////////////////////////////
/** 


*/   
////////////////////////////////////////////////////////////////////////////////////////////


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
	//

	model["model"]["status"] = 1;
	if(model["vk"].contains("deactivated") == true)
	{
		std::string deactivated = model["vk"]["deactivated"].get<std::string>();
		bool flag = false;

		if(flag == false && deactivated == "deleted")
		{
			model["model"]["status"] = 0;
			flag = true;
		}

		if(flag == false && deactivated == "banned")
		{
			model["model"]["status"] = 2;
			flag = true;
		}

		if(flag == false)
		{
			std::cout << _ERR_TEXT_ << "unknown [deactivated] value [" << model["vk"]["deactivated"].get<std::string>() << "]" << std::endl;
			exit(0);
		}
	}
	else
	{
		if(model["vk"].contains("is_closed") == true)
		{
			if(model["vk"]["is_closed"].get<bool>() == true)
			{
				model["model"]["status"] = 3;
			}
		}
	}

	//
	// t_birth
	//

	model["model"]["t_birth"] = "1000-01-01 00:00:00";
	if(model["vk"].contains("bdate") == true)
	{
		// Пытаемся распарсить дату рождения
		std::string bdate = model["vk"]["bdate"].get<std::string>();
		auto arr_bdate = core::explode(bdate,".");

		// std::cout << bdate << " size() = " << arr_bdate.size() << std::endl;

		if(arr_bdate.size() == 3)
		{
			if(arr_bdate[2].size() == 4)
			{
				bdate = arr_bdate[2];
			}
			else
			{
				std::cout << "unknown bdate [" << bdate << "]" << std::endl;
				exit(0);
			}

			if(arr_bdate[1].size() == 1)
			{
				bdate = bdate + "-0" + arr_bdate[1];
			}
			else
			{
				bdate = bdate + "-" + arr_bdate[1];
			}

			if(arr_bdate[0].size() == 1)
			{
				bdate = bdate + "-0" + arr_bdate[0];
			}
			else
			{
				bdate = bdate + "-" + arr_bdate[0];
			}

			bdate = bdate + " 00:00:00";

			//
			// Проверить дату на корректность
			//

			if(core::time::check(bdate,"%Y-%m-%d %H:%M:%S") == true)
			{
				model["model"]["t_birth"] = bdate;
			}
			else
			{
				model["model"]["t_birth"] = "1000-01-01 00:00:00";
			}
		}
	}

	//
	// PROFILE AVATARS DOMAIN
	//

	/*
	check_avatar_url("photo_50", &(model["vk"]) );
	check_avatar_url("photo_100", &(model["vk"]) );
	check_avatar_url("photo_200", &(model["vk"]) );
	check_avatar_url("photo_max", &(model["vk"]) );
	check_avatar_url("photo_200_orig", &(model["vk"]) );
	check_avatar_url("photo_400_orig", &(model["vk"]) );
	check_avatar_url("photo_max_orig", &(model["vk"]) );
	*/

	if(model["vk"].contains("photo_max_orig") == true)
	{
		model["photo"] = model["vk"]["photo_max_orig"].get<std::string>();
	}

	//
	// Удаляем поля, которые могут регулярно меняться, но при этом не имеют важного сысла
	//

	if(model["vk"].contains("track_code"))
	{
		model["vk"].erase("track_code");
	}

	if(model["vk"].contains("status_audio"))
	{
		model["vk"].erase("status_audio");
	}

	//
	// ID USER
	//

	(*data) = model;
	return model["vk"]["id"].get<long long int>();
};


			
}; // end users namespace
}; // end vk namespace

#endif 

