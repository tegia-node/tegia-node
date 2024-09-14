#ifndef H_TEGIA_DICTIONARIES_INDEX
#define H_TEGIA_DICTIONARIES_INDEX

#include <string>
#include <unordered_map>

#include <tegia/core/json.h>


namespace tegia {
namespace dictionaries {

class catalog_t;

class index_t
{
	friend class catalog_t;

	protected:
		std::unordered_map<std::string, nlohmann::json *> _index;

	public:
		index_t() = default;
		~index_t() = default;

		std::tuple<bool,nlohmann::json> find(const std::string &index);
};


index_t * index(const std::string &dictionary, const std::string &index);

} // END namespace dictionaries
} // END namespace tegia

#endif