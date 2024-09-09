#ifndef H_TEGIA_TYPES_SIMPLE_GENDER
#define H_TEGIA_TYPES_SIMPLE_GENDER

#include "simple.h"

namespace tegia {
namespace types {

class gender_t: public tegia::types::simple_t
{
	public:
		gender_t();
		virtual ~gender_t() = default;

		virtual int parse(
			const std::string &_gender, 
			const nlohmann::json &validate = nlohmann::json::object()
		) override final;
};


}	// END namespace types
}	// END namespace tegia



#endif