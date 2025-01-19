#ifndef H_TEGIA_CORE_ERROR
#define H_TEGIA_CORE_ERROR

//
//
//

#include <iostream>

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

}



#endif