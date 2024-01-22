#ifndef H_TEGIA_TYPES_PERSON
#define H_TEGIA_TYPES_PERSON

#include "base.h"

namespace tegia {
namespace types {

class person_t: public tegia::types::base_t
{
	protected:
		std::string _surname = "";
		std::string _name = "";
		std::string _patronymic = "";
		std::string _t_birth = "";
		int _gender = 0;

	public:
		person_t();
		virtual ~person_t() = default;
		std::string value() const override final;
		std::string hash() const override final;
		nlohmann::json json() const override final;

		//
		// Common
		//

		static std::string normal(const std::string &name);
		
		//
		// Specific
		//

		int parse(const std::string &value, const nlohmann::json &validate = nlohmann::json::object());
};


}	// END namespace types
}	// END namespace tegia


#endif
