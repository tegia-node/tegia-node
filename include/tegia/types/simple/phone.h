#ifndef H_TEGIA_TYPES_SIMPLE_PHONE
#define H_TEGIA_TYPES_SIMPLE_PHONE

#include "simple.h"

namespace tegia {
namespace types {


class phone_t: public tegia::types::simple_t
{
	public:
		phone_t();
		virtual ~phone_t() = default;
		
		virtual int parse(
			const std::string &value, 
			const nlohmann::json &validate = nlohmann::json::object()
		);
};


}	// END namespace contacts
}	// END namespace tegia


#endif