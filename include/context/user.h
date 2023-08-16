#ifndef H_TEGIA_USER
#define H_TEGIA_USER
// --------------------------------------------------------------------

#include <string>
#include <iostream>
#include <bitset>

#include <tegia2/core/json.h>


namespace tegia {
namespace threads
{
	class _data;
}
}

namespace tegia {
class user
{
	friend class tegia::threads::_data;

	private:
		std::string _uuid;

		std::string _name;
		std::string _surname;
		std::string _patronymic;
		std::string _photo;

		int _sex;
		int _status;
		
		std::bitset<64> _roles{0};

		user();

	public:
		~user();
		std::string uuid();

};	// END CLASS user
}	// END NAMESPACE tegia 



// --------------------------------------------------------------------
#endif