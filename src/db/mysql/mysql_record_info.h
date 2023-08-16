#ifndef H_TEGIA_DB_MYSQL_RECORD_INFO
#define H_TEGIA_DB_MYSQL_RECORD_INFO

////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// INCLUDES                                                                               //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

//	STL 
	#include <string>

////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// RECORD INFO CLASS                                                                      //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

namespace tegia {
namespace mysql {

class record_info
{
	public:
		std::string name;
		std::string table;
		std::string def;
		int type;
		unsigned int length;
		unsigned int max_length;
		unsigned int flags;
		unsigned int decimals;

		record_info(
			std::string name, 
			std::string table, 
			std::string def, 
			int type, 
			unsigned int length, 
			unsigned int max_length, 
			unsigned int flags, 
			unsigned int decimals):
			name(name),
			table(table),
			def(def),
			type(type),
			length(length),
			max_length(max_length),
			flags(flags),
			decimals(decimals)
		{	};
		
		virtual ~record_info() { };
};


/** -----------------------------------------------------------------------------------------------
		\brief    Структура для рассчета хеша для pair.
		\details  Структура позволяет использовать pair в качестве ключа в unordered_multimap. 
							Подробнее тут: http://stackoverflow.com/questions/20590656/error-for-hash-function-of-pair-of-ints
		\warning  Используемая функция объединений хешей весьма неоптимальна. Со временем нужно
							доработать функцию вычисления хеша для pair. 
							Подробнее тут: http://stackoverflow.com/questions/5889238/why-is-xor-the-default-way-to-combine-hashes        
*/
struct pairhash
{
	public:
		template <typename T, typename U>
		std::size_t operator()(const std::pair<T, U> &x) const
		{
			return std::hash<T>()(x.first) ^ std::hash<U>()(x.second);
		}
};




}   // end namespace mysql
}   // end namespace tegia


#endif 



