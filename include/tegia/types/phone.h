#ifndef H_TEGIA_TYPES_PHONE
#define H_TEGIA_TYPES_PHONE

#include "base.h"

namespace tegia {
namespace types {


class phone_t: public tegia::types::base_t
{
	protected:
		long long int _category = 0;
		std::string _phone = "";

	public:

		phone_t();
		virtual ~phone_t() = default;
		std::string value() const override final;
		std::string hash() const override final;
		nlohmann::json json() const override final;

		//
		// Specific
		//

		int parse(const std::string &_gender, const nlohmann::json &validate = nlohmann::json::object());
};


}	// END namespace contacts
}	// END namespace tegia


#endif