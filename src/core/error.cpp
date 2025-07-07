#include <format>
#include <tegia/core/error.h>

//
#undef _LOG_LEVEL_
#define _LOG_LEVEL_ _LOG_WARNING_
#include <tegia/context/log.h>
//


namespace tegia {

error_t::error_t(
	const std::string &type,
	const std::string &code,
	nlohmann::json info
):
	uuid(tegia::random::uuid()),
	type(type),
	code(code),
	info(info),
	user(tegia::threads::user()->uuid())
{

};


void error_t::log()
{
	LERROR(
		this->uuid + "\n"
		"type = " + this->type + "\n"
		"code = " + this->code + "\n"
		"user = " + this->user + "\n" + this->info.dump())
};


void error_t::print()
{
	std::cout << _ERR_TEXT_ << this->uuid << std::endl;
	std::cout << "type = " << this->type << std::endl;
	std::cout << "code = " << this->code << std::endl;
	std::cout << "user = " << this->user << std::endl;
	std::cout << this->info.dump() << std::endl;
};


}



namespace tegia {
namespace log {

event_t::event_t():
	uuid(tegia::random::uuid()),
	user(tegia::threads::user()->uuid())
{

};

event_t::event_t(const std::string &level):
	uuid(tegia::random::uuid()),
	user(tegia::threads::user()->uuid()),
	level(level)
{

};

std::string event_t::log()
{
	return std::format("UUID: {}\nUSER: {}\nDATA: ", this->uuid, this->user) + this->info; 
};

}
}





namespace tegia {
namespace errors {

tegia::log::event_t open_file(std::error_code ec, const std::string &filename)
{
	tegia::log::event_t event;
	event.level = "error";
	event.info = std::format(
			"UUID:{} File '{}' is not open. Reason: [{}] {}",
			event.uuid,
			filename,
			ec.value(),
			ec.message()
		);
	LLERROR(1, event.info);
	return std::move(event);
};

tegia::log::event_t mysql(int code, const std::string &connection, const std::string &query, nlohmann::json additional)
{
	tegia::log::event_t event;
	event.level = "error";

	event._data["code"] = code;
	event._data["connection"] = connection;
	event._data["query"] = query;

	if(additional.size() != 0)
	{
		event._data["additional"] = additional;
	}

	event.info = std::format(
			"UUID:{}, {}",
			event.uuid,
			event._data.dump()
		);
		
	return std::move(event);
};

} // END namespace errors
} // END namespace tegia

