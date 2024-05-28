#ifndef H_TEGIA_TYPES_VK
#define H_TEGIA_TYPES_VK

#include "base.h"

namespace tegia {
namespace types {

class vk_t: public tegia::types::base_t
{
	protected:
		std::string _link = "";
		long long int _id = 0;

	public:
		vk_t();
		virtual ~vk_t() = default;
		std::string value() const override;
		std::string hash() const override;
		nlohmann::json json() const override;
		nlohmann::json graph() const;
		
		//
		// Specific
		//

		int parse(const std::string &value, const nlohmann::json &validate = nlohmann::json::object());
		bool set_id(long long int _id);
};


}	// END namespace types
}	// END namespace tegia


#endif
