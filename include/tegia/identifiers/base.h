#ifndef H_TEGIA_IDENTIFIERS_PERSON_BASE
#define H_TEGIA_IDENTIFIERS_PERSON_BASE

#include <tegia/tegia.h>


namespace tegia {
namespace identifiers {

class base_t
{
	public:
		base_t(const long long int type);
		~base_t(){};

	protected:
		long long int _type = 0;
		std::string _value;

	private:

}; // END class base_t

//
//
//

base_t::base_t(const long long int type): _type(type)
{

};


}  // END namespace identifiers
}  // END namespace tegia

#endif