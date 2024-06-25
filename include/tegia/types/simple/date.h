#ifndef H_TEGIA_TYPES_SIMPLE_DATE
#define H_TEGIA_TYPES_SIMPLE_DATE

#include "simple.h"

namespace tegia {
namespace types {

class date_t: public tegia::types::simple_t
{
	public:
		date_t();
		virtual ~date_t() = default;

		virtual int parse(
			const std::string &value, 
			const nlohmann::json &validate = nlohmann::json::object()
		) override final;
};


}	// END namespace types
}	// END namespace tegia


#endif
