#ifndef H_TEGIA_IDENTIFIERS_PERSON_2643021
#define H_TEGIA_IDENTIFIERS_PERSON_2643021

#include "base.h"


namespace tegia {
namespace identifiers {

struct person_t
{

};

struct document_t
{

};

class i2643021: public base_t
{
	public:
		i2643021();
		~i2643021(){};

	protected:
		nlohmann::json _data;

	private:

}; // END class i2643021

//
//
//

i2643021::i2643021(): base_t(2643021)
{

};

}  // END namespace identifiers
}  // END namespace tegia

#endif