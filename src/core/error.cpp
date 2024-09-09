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
	user(tegia::context::user()->uuid())
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

