#ifndef H_TEGIA_TASKS
#define H_TEGIA_TASKS


#include <tegia/core/json.h>
#include <tegia/db/mysql/mysql.h>

namespace tegia {

class task_t
{
	public:
		task_t() = default;
		~task_t() = default;

		std::string hash;
		std::string wsid;
		std::string table;
		std::string connection;

		std::string category;
		std::string type;
		std::string random;

		long long int priority = 0;
		std::string t_create;

		nlohmann::json data;
		nlohmann::json result;
		nlohmann::json error;
		nlohmann::json callback;

		int status(int status);
		int status();
		int load(nlohmann::json &jtask);
		nlohmann::json json();

	private:	
		int _status = 0;
};

}


#endif
