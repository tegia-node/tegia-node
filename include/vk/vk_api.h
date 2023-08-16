#ifndef _H_VK_API_
#define _H_VK_API_

#include <tegia/core/http.h>
#include "vk_api_error.h"

const std::string vk_api_version	=	"5.131";
const std::string vk_api_url		=	"https://api.vk.com/method/";

const std::string USER_FIELDS		=	"about," 
										"activities,"
										"blacklisted,"
										"blacklisted_by_me,"
										"bdate,"
										"books,"
										"can_post,"
										// "can_be_invited_group,"  REM 
										"can_see_all_posts,"
										"can_see_audio,"
										"can_send_friend_request,"
										"can_write_private_message,"
										"career,"
										"city,"
										// "common_count,"          REM: fields could not contain common_count for unauthorized user
										"connections,"
										"contacts,"
										"country,"
										// "crop_photo,"
										"domain,"
										"education,"
										"exports,"
										// "followers_count,"       REM: Количество подписчиков пользователя
										"friend_status,"
										"games,"
										"has_mobile,"
										"has_photo,"
										"home_town,"
										"interests,"
										"is_favorite,"
										"is_friend,"
										"is_hidden_from_feed,"
										// "last_seen,"            REM: Время последнего посещения
										"lists,"
										"maiden_name,"
										"military,"
										"movies,"
										"music,"
										"nickname,"
										"occupation,"
										"personal,"
										"photo_100,"
										"photo_200,"
										"photo_200_orig,"
										"photo_400_orig,"
										"photo_50,"
										"photo_id,"
										"photo_max,"
										"photo_max_orig,"
										"quotes,"
										"relation,"
										"relatives,"
										"schools,"
										"screen_name,"
										"sex,"
										"site,"
										"status,"
										"timezone,"
										"tv,"
										"universities,"
										"verified,"
										"wall_comments";


namespace vk {



std::tuple<int,nlohmann::json> run2(const std::string &method, const std::string &post, const tegia::http::proxy &_proxy)
{
	auto http = new tegia::http::client();
	std::tuple<int,nlohmann::json> result;
	nlohmann::json return_data;

	http->set_proxy(_proxy);

	bool flag = true;
	while(flag)
	{
		http->response->data.clear();
		int status = http->post(vk_api_url + method,post);
		switch(status)
		{
			case 200:
			{
				try
				{
					std::u32string utf32_data = tegia::string::str_to_u32str(http->response->data);
					return_data = nlohmann::json::parse(utf32_data);
					if(return_data.contains("response") == true)
					{
						result = std::make_tuple(200,return_data["response"]);
						flag = false;
					}
					else
					{
						int err_code = return_data["error"]["error_code"].get<int>();
						switch(err_code)
						{
							case 6:		// VK: Слишком много запросов в секунду.
							case 10:	// VK: Произошла внутренняя ошибка сервера.
							{
								std::cout << _ERR_TEXT_ << "[repeat] VK: Слишком много запросов в секунду\n";
								std::this_thread::sleep_for(1000ms);
							}
							break;

							default:
							{
								result = std::make_tuple(err_code,return_data);
								flag = false;
							}
							break;
						}
					}					
				}

				catch (nlohmann::json::exception& e)
				{
					std::cout << e.what() << '\n';
					std::cout << http->response->data << "\n";

					exit(0);
				}

				catch(std::range_error& exception)
				{
					nlohmann::json tmp;

					tmp["key"] = "method";
					tmp["value"] = method;
					return_data["error"]["request_params"].push_back(tmp);

					tmp["key"] = "post_data";
					tmp["value"] = post;
					return_data["error"]["request_params"].push_back(tmp);
					
					return_data["error"]["error_code"] = 2000;
					return_data["error"]["error_type"] = "tegia::string::str_to_u32str";
					return_data["error"]["error_msg"] = "std::range_error exception";

					result = std::make_tuple(2000,return_data);
					flag = false;
				}

				catch(std::exception& e)
				{
					std::cout << "run2 exception" << std::endl;
					std::cout << e.what() << std::endl;
					exit(0);
				}
			}
			break;

			case 6:		// CURLE_COULDNT_RESOLVE_HOST
			case 18:	// CURLE_PARTIAL_FILE
			case 28:	// CURLE_OPERATION_TIMEDOUT
			case 56:	// CURLE_RECV_ERROR
			case 77:	// CURLE_SSL_CACERT_BADFILE
			case 500:	// HTTP: INTERNAL SERVER ERROR
			case 502:	// HTTP: Bad Gateway
			case 504:	// HTTP: Gateway Timeout
			{
				std::cout << _ERR_TEXT_ << "[repeat] http status = " << status << std::endl;
				std::this_thread::sleep_for(1000ms);
			}
			break;

			default:
			{
				nlohmann::json tmp;

				tmp["key"] = "method";
				tmp["value"] = method;
				return_data["error"]["request_params"].push_back(tmp);

				tmp["key"] = "post_data";
				tmp["value"] = post;
				return_data["error"]["request_params"].push_back(tmp);
				
				return_data["error"]["error_code"] = 1000 + status;
				return_data["error"]["error_type"] = "curl";
				return_data["error"]["error_msg"] = "curl";

				result = std::make_tuple(1000 + status,return_data);
				flag = false;
			}
			break;
		}
	}

	delete http;
	return result;
};



namespace utils {


std::tuple<int,nlohmann::json> resolve(
        const std::string &key,
        const std::string &screen_name,
		const tegia::http::proxy &_proxy = tegia::http::proxy())
{
    std::string post =  "v=" + vk_api_version + 
                        "&access_token=" + key + 
                        "&screen_name=" + screen_name;
    return vk::run2("utils.resolveScreenName",post,_proxy);
};


}	// END namespace utils


}	// END namespace vk


#endif 

