#ifndef H_TEGIA_TYPES_SNILS
#define H_TEGIA_TYPES_SNILS

#include "base.h"

namespace tegia {
namespace types {

class snils_t: public tegia::types::base_t
{
	protected:
		std::string _snils = "";

	public:
		snils_t();
		virtual ~snils_t() = default;
		std::string value() const override final;
		std::string hash() const override final;
		nlohmann::json json() const override final;

		//
		// Specific
		//

		int parse(const std::string &_email, const nlohmann::json &validate = nlohmann::json::object());
};


}	// END namespace types
}	// END namespace tegia



#endif