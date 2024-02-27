#ifndef H_TEGIA_TYPES_RUS_INN
#define H_TEGIA_TYPES_RUS_INN

#include "base.h"

namespace tegia {
namespace types {

class rus_inn_t: public tegia::types::base_t
{
	protected:
		std::string _rus_inn = "";

	public:
		rus_inn_t();
		virtual ~rus_inn_t() = default;
		std::string value() const override final;
		std::string hash() const override final;
		nlohmann::json json() const override final;

		//
		// Specific
		//

		int parse(const std::string &_rus_inn, const nlohmann::json &validate = nlohmann::json::object());
		int code();
};


}	// END namespace types
}	// END namespace tegia



#endif