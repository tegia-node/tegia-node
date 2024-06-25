#ifndef H_TEGIA_TYPES_SIMPLE_VK
#define H_TEGIA_TYPES_SIMPLE_VK

#include "simple.h"

namespace tegia {
namespace types {

class vk_t: public tegia::types::simple_t
{
	public:
		vk_t();
		virtual ~vk_t() = default;

		virtual int parse(
			const std::string &value, 
			const nlohmann::json &validate = nlohmann::json::object()
		);

		int set_id(long long int value);
};


}	// END namespace types
}	// END namespace tegia


#endif
