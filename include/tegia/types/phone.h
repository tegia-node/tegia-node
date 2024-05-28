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
		std::string value() const override;
		std::string hash() const override;
		nlohmann::json json() const override;
		nlohmann::json graph() const;
		
		//
		// Specific
		//

		int parse(const std::string &value, const nlohmann::json &validate = nlohmann::json::object());
};


}	// END namespace contacts
}	// END namespace tegia


#endif