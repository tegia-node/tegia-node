#ifndef H_TEGIA_TYPES_RUS_PASSPORT
#define H_TEGIA_TYPES_RUS_PASSPORT

#include "base.h"

namespace tegia {
namespace types {

class rus_passport_t: public tegia::types::base_t
{
	protected:
		std::string _rus_passport = "";

	public:
		rus_passport_t();
		virtual ~rus_passport_t() = default;
		std::string value() const override;
		std::string hash() const override;
		nlohmann::json json() const override;

		//
		// Specific
		//

		int parse(const std::string &_rus_passport, const nlohmann::json &validate = nlohmann::json::object());
};


}	// END namespace types
}	// END namespace tegia



#endif