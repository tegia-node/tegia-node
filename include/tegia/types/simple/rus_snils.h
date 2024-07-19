#ifndef H_TEGIA_TYPES_SIMPLE_RUS_SNILS
#define H_TEGIA_TYPES_SIMPLE_RUS_SNILS

#include "simple.h"

namespace tegia {
namespace types {

class rus_snils_t: public tegia::types::simple_t
{
	public:
		rus_snils_t();
		virtual ~rus_snils_t() = default;

		virtual int parse(
			const std::string &value, 
			const nlohmann::json &validate = nlohmann::json::object()
		);
};


}	// END namespace types
}	// END namespace tegia



#endif