#ifndef H_TEGIA_TYPES_PERSON
#define H_TEGIA_TYPES_PERSON

#include "base.h"

namespace tegia {
namespace types {

class person_t: public tegia::types::base_t
{
	protected:
		std::string _uuid = "";
		std::string _inn = "";
		std::string _surname = "";
		std::string _name = "";
		std::string _patronymic = "";
		std::string _t_birth = "";
		int _gender = 0;
		
		bool is_full = false;
		bool is_verified = false;

	public:
		person_t();
		virtual ~person_t() = default;
		std::string value() const override;
		std::string hash() const override;
		nlohmann::json json() const override;
		nlohmann::json graph() const;
		
		//
		// Common
		//

		static std::string _normal(const std::string &name);
		static std::string normal(const std::string &name, int part = 0);
		static int gender(const std::string &name, const std::string &patronymic);
		
		//
		// Specific
		//

		int parse(const std::string &value, const nlohmann::json &validate = nlohmann::json::object());
		int parse(const nlohmann::json &data, const nlohmann::json &validate = nlohmann::json::object());

		std::string uuid();
		std::string inn();
		std::string surname();
		std::string name();
		std::string patronymic();
		std::string t_birth();
};


}	// END namespace types
}	// END namespace tegia


#endif
