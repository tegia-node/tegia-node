#ifndef H_TEGIA_ACTORS_MESSAGE_T
#define H_TEGIA_ACTORS_MESSAGE_T
// ========================================================================================

#include <tegia2/core/json.h>

class message_t
{
	public:
		nlohmann::json data;

	private:
		std::string uuid;
};


// ========================================================================================
#endif