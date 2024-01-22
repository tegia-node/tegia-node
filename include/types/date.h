#ifndef H_TEGIA_TYPES_DATE
#define H_TEGIA_TYPES_DATE

#include "base.h"

namespace tegia {
namespace types {

class date_t: public tegia::types::base_t
{
	protected:
		std::string _date = "";

	public:
		date_t();
		virtual ~date_t() = default;
		std::string value() const override final;
		std::string hash() const override final;
		nlohmann::json json() const override final;

		//
		// Specific
		//

		int parse(const std::string &value, const nlohmann::json &validate = nlohmann::json::object());
};


}	// END namespace types
}	// END namespace tegia


#endif
