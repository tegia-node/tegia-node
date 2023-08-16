#include <tegia/const.h>
#include <tegia/core/core.h>

#include <tegia/actors/message_t.h>

using namespace std::literals::string_literals;

static std::string const MESSAGE_DATA = "data"s;
static std::string const MESSAGE_PIPELINE = "pipeline"s;
static std::string const MESSAGE_HTTP = "http"s;
static std::string const MESSAGE_CALLBACK = "callback"s;

//
//  Converts message data to pure JSON object (to be sent via TCP/IP).
//  Note: initial message_t object is destroyed and not usable any more!
// 
nlohmann::json message_t::serialize() noexcept
{
    auto json_data = nlohmann::json::object();
    if (!data.is_null()&& !data.empty()) json_data[MESSAGE_DATA] =  std::move(data);

    if (!pipeline.is_null() && !pipeline.empty())  json_data[MESSAGE_PIPELINE] = std::move(pipeline);

    if (http != nullptr) json_data[MESSAGE_HTTP] = http->request.json();

    if (!callback.empty())
    {
        json_data[MESSAGE_CALLBACK] = nlohmann::json::array();

        while (!callback.empty())
        {
            auto& [actor, action] = callback.top();
            json_data[MESSAGE_CALLBACK].push_back(nlohmann::json{{std::move(actor), std::move(action)}});
            callback.pop();
        }
    }

    return json_data;
}

//
//  Converts JSON object (obtained via TCP/IP) to the message data.
//  Note: initial JSON object is destroyed and not usable any more!
// 
void message_t::deserialize(nlohmann::json& json_data) noexcept
{
    if (json_data.contains(MESSAGE_DATA) && !json_data[MESSAGE_DATA].is_null()) data = std::move(json_data[MESSAGE_DATA]);

    if (json_data.contains(MESSAGE_PIPELINE) && !json_data[MESSAGE_PIPELINE].is_null()) pipeline = std::move(json_data[MESSAGE_PIPELINE]);

    if (json_data.contains(MESSAGE_HTTP) && !json_data[MESSAGE_HTTP].is_null())
    {
        if (http == nullptr) http = new HTTP{};
        http->request.from_json(json_data[MESSAGE_HTTP]);
    }

    if (json_data.contains(MESSAGE_CALLBACK) && json_data[MESSAGE_CALLBACK].is_array() && !json_data[MESSAGE_CALLBACK].empty())
    {
        for (auto it = json_data[MESSAGE_CALLBACK].rbegin(); it != json_data[MESSAGE_CALLBACK].rend(); ++it)
        {
            for (auto& [key, val] : it.value().items())
            {
                callback.emplace(key, *val.get_ptr<const nlohmann::json::string_t*>());
            }
        }
    }
}


nlohmann::json HTTP_REQUEST_DATA::json() const noexcept
{
    nlohmann::json tmp = nlohmann::json::object();

    tmp["query"]["request_method"] = this->request_method;
    tmp["query"]["script_name"] = this->script_name;
    tmp["query"]["content_type"] = this->content_type;
    tmp["query"]["content_length"] = this->content_length;
    tmp["query"]["redirect_status"] = this->redirect_status;
    
    tmp["server"]["server_software"] = this->server_software;
    tmp["server"]["server_name"] = this->server_name;
    tmp["server"]["gateway_interface"] = this->gateway_interface;
    tmp["server"]["server_protocol"] = this->server_protocol;
    tmp["server"]["server_port"] = this->server_port;

    tmp["user"]["remote_addr"] = this->remote_addr;
    tmp["user"]["accept"] = this->http_accept;
    tmp["user"]["user_agent"] = this->http_user_agent;
    tmp["user"]["referer"] = this->http_referer;
    tmp["user"]["origin"] = this->http_origin;
    tmp["user"]["authorization"] = this->http_authorization;

    tmp["post"] = this->post;
    tmp["query_param"] = this->query_param;

    for(auto it = this->cookie.begin(); it != this->cookie.end(); it++)
    {
        tmp["cookie"][it->first] = it->second;
    }

    return tmp;
};

void HTTP_REQUEST_DATA::from_json(nlohmann::json& json_data) noexcept
{
    this->request_method = std::move(*json_data["query"]["request_method"].get_ptr<const nlohmann::json::string_t*>());
    this->script_name =  std::move(*json_data["query"]["script_name"].get_ptr<const nlohmann::json::string_t*>());
    this->content_type =  std::move(*json_data["query"]["content_type"].get_ptr<const nlohmann::json::string_t*>());
    this->content_length =  std::move(*json_data["query"]["content_length"].get_ptr<const nlohmann::json::string_t*>());
    this->redirect_status =  std::move(*json_data["query"]["redirect_status"].get_ptr<const nlohmann::json::string_t*>());
    
    this->server_software =  std::move(*json_data["server"]["server_software"].get_ptr<const nlohmann::json::string_t*>());
    this->server_name =  std::move(*json_data["server"]["server_name"].get_ptr<const nlohmann::json::string_t*>());
    this->gateway_interface =  std::move(*json_data["server"]["gateway_interface"].get_ptr<const nlohmann::json::string_t*>());
    this->server_protocol =  std::move(*json_data["server"]["server_protocol"].get_ptr<const nlohmann::json::string_t*>());
    this->server_port =  std::move(*json_data["server"]["server_port"].get_ptr<const nlohmann::json::string_t*>());

    this->remote_addr =  std::move(*json_data["user"]["remote_addr"].get_ptr<const nlohmann::json::string_t*>());
    this->http_accept =  std::move(*json_data["user"]["accept"].get_ptr<const nlohmann::json::string_t*>());
    this->http_user_agent =  std::move(*json_data["user"]["user_agent"].get_ptr<const nlohmann::json::string_t*>());
    this->http_referer =  std::move(*json_data["user"]["referer"].get_ptr<const nlohmann::json::string_t*>());
    this->http_origin =  std::move(*json_data["user"]["origin"].get_ptr<const nlohmann::json::string_t*>());
    this->http_authorization =  std::move(*json_data["user"]["authorization"].get_ptr<const nlohmann::json::string_t*>());

    this->post = std::move(json_data["post"]);
    this->query_param = std::move(json_data["query_param"]);

    if (json_data.contains("cookie") && !json_data["cookie"].is_null())
    {
        for (auto& [key, val] : json_data["cookie"].items())
        {
            cookie.emplace(key, *val.get_ptr<const nlohmann::json::string_t*>());
        }
    }
}

