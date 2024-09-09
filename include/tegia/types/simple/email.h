#ifndef H_TEGIA_TYPES_SIMPLE_EMAIL
#define H_TEGIA_TYPES_SIMPLE_EMAIL

#include "simple.h"

namespace tegia {
namespace types {

class email_t: public tegia::types::simple_t
{
	public:
		email_t();
		virtual ~email_t() = default;

		virtual int parse(
			const std::string &value, 
			const nlohmann::json &validate = nlohmann::json::object()
		) override final;
};


}	// END namespace types
}	// END namespace tegia



#endif