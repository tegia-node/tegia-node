#ifndef H_TEGIA_IDENTIFIERS_PERSON_1000000
#define H_TEGIA_IDENTIFIERS_PERSON_1000000

#include "base.h"


namespace tegia {
namespace identifiers {

class i1000000: public base_t
{
	public:
		i1000000();
		~i1000000(){};

	protected:
		nlohmann::json _data;

	private:

}; // END class i1000000

//
//
//

i1000000::i1000000(): base_t(1000000)
{

};

}  // END namespace identifiers
}  // END namespace tegia

#endif