#ifndef H_TEGIA_TYPES_EMAIL
#define H_TEGIA_TYPES_EMAIL

#include "base.h"

namespace tegia {
namespace types {

class email_t: public tegia::types::base_t
{
	protected:
		std::string _email = "";

	public:
		email_t();
		virtual ~email_t() = default;
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