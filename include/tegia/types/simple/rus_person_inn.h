#ifndef H_TEGIA_TYPES_SIMPLE_RUS_PERSON_INN
#define H_TEGIA_TYPES_SIMPLE_RUS_PERSON_INN

#include "simple.h"

namespace tegia {
namespace types {

class rus_person_inn_t: public tegia::types::simple_t
{
	public:
		rus_person_inn_t();
		virtual ~rus_person_inn_t() = default;

		virtual int parse(
			const std::string &value, 
			const nlohmann::json &validate = nlohmann::json::object()
		) override final;
};


}	// END namespace types
}	// END namespace tegia



#endif