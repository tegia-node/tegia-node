#ifndef H_TEGIA_TYPES_GENDER
#define H_TEGIA_TYPES_GENDER

#include "base.h"

namespace tegia {
namespace types {

class gender_t: public tegia::types::base_t
{
	protected:
		std::string _gender = "";

	public:
		gender_t();
		virtual ~gender_t() = default;
		std::string value() const override final;
		std::string hash() const override final;
		nlohmann::json json() const override final;
		nlohmann::json graph() const;
		
		//
		// Specific
		//

		int parse(const std::string &_gender, const nlohmann::json &validate = nlohmann::json::object());
};


}	// END namespace types
}	// END namespace tegia



#endif