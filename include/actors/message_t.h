#ifndef H_TEGIA_ACTORS_MESSAGE_T
#define H_TEGIA_ACTORS_MESSAGE_T
// ========================================================================================

#include <tegia2/core/json.h>
#include <tegia2/core/crypt.h>

class message_t
{
	public:
		nlohmann::json data;
		nlohmann::json http;

		message_t(nlohmann::json _data):
			data(_data),
			uuid(tegia::random::uuid()){};

		message_t():
			data(nlohmann::json::object()),
			uuid(tegia::random::uuid()){};

		virtual ~message_t() = default;

	private:
		std::string uuid;
};


// ========================================================================================
#endif