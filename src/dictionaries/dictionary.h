#ifndef H_TEGIA_DICTIONARIES_DICTIONARY
#define H_TEGIA_DICTIONARIES_DICTIONARY

#include <string>
#include <tegia/core/json.h>

namespace tegia {
namespace dictionaries {

class catalog_t;

class dictionary_t
{
	friend class catalog_t;

	protected:
		nlohmann::json _data;
		std::string filename;

	public:
		dictionary_t() = default;
		~dictionary_t() = default;
};

}
}


#endif