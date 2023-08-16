#ifndef H_DB_DAO_ENTITY2
#define H_DB_DAO_ENTITY2

// -------------------------------------------------------------------------------------- //
//                                      INCLUDES                                          //
// -------------------------------------------------------------------------------------- //

//	STL 
	#include <string>
	#include <iostream>
	#include <sstream>
	#include <map>
	#include <functional>


namespace dao { 

template<class OBJ>
class _access
{
	protected:
		std::map<std::string, std::function<std::string(void)> > _getters;
		std::map<std::string, std::function<std::string(void)> > _setters;

	public:
		template<typename T>
		void _add(const std::string &_name, T *t)
		{
			//
			// getter
			//

			std::function<std::string(void)> fn_getter = [=]()
			{
				return core::cast<std::string>(*t);
			};

			_getters.insert({_name,fn_getter});

			//
			// setter
			//


		};


};

class Entity2 
{
	protected:
		std::string table;

	public:
		Entity2(const std::string& _table):table(_table)
		{

		};

		virtual ~Entity2()
		{
			
		};



};	// END class Entity
}	// END namespace dao

#endif 



