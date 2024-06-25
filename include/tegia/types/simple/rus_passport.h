#ifndef H_TEGIA_TYPES_SIMPLE_RUS_PASSPORT
#define H_TEGIA_TYPES_SIMPLE_RUS_PASSPORT

#include "simple.h"

namespace tegia {
namespace types {

class rus_passport_t: public tegia::types::simple_t
{
	public:
		rus_passport_t();
		virtual ~rus_passport_t() = default;
		
		virtual int parse(
			const std::string &value, 
			const nlohmann::json &validate = nlohmann::json::object()
		);
};


}	// END namespace types
}	// END namespace tegia



#endif