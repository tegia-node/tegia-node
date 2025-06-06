#ifndef H_TEGIA_CORE_ERROR
#define H_TEGIA_CORE_ERROR

//
//
//

#include <iostream>
#include <format>

#include <tegia/core/crypt.h>
#include <tegia/core.h>

//
//
//

namespace tegia {

class error_t
{
	public:
		error_t(
			const std::string &type,
			const std::string &code,
			nlohmann::json info
		);

		~error_t() = default;

		std::string uuid;
		std::string type;
		std::string code;
		std::string user;
		nlohmann::json info;

		void log();
		void print();
};

} // END namespace tegia

//
//
//



namespace tegia {
namespace log {

class event_t
{
	public:
		event_t();
		event_t(const std::string &level);
		~event_t() = default;

		std::string uuid;
		int status;

		std::string level;
		std::string user;
		std::string info;

		nlohmann::json _data;
		std::string log();

	private:

};

} // END namespace log
} // END namespace tegia


namespace tegia {
namespace errors {

tegia::log::event_t open_file(std::error_code ec, const std::string &filename);
tegia::log::event_t mysql(int code, const std::string &connection, const std::string &query, nlohmann::json additional = nullptr);

} // END namespace errors
} // END namespace tegia


#endif