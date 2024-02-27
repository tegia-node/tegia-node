#include <tegia/core/time.h>



namespace core {
namespace time {


////////////////////////////////////////////////////////////////////////////////////////////
/** 
	\brief ФУНКЦИЯ находит число дней в указанном месяце

*/
////////////////////////////////////////////////////////////////////////////////////////////

int get_count_days(const std::string &year, const std::string &month)
{
	std::string _month = month;

	if(month.size() == 1)
	{
		_month = "0" + month;
	}

	// std::cout << "[" << year << "] [" << _month << "]" << std::endl;

	std::tm tm_start = core::time::str_to_tm(year + "-" + _month + "-01 00:00:00", "%Y-%m-%d %H:%M:%S");
	std::tm tm_end   = core::time::str_to_tm(year + "-" + _month + "-01 00:00:00", "%Y-%m-%d %H:%M:%S");

	tm_end.tm_mday = tm_start.tm_mday + 30;
	std::mktime(&tm_end);

	// std::cout << "tm_start = " << core::time::tm_to_str(tm_start, "%Y-%m-%d") << std::endl;
	// std::cout << "tm_end = " << core::time::tm_to_str(tm_end, "%Y-%m-%d") << std::endl;

	// std::cout << "tm_end = " << core::time::tm_to_str(tm_end, "%Y-%m-%d") << std::endl;
	// std::cout << "tm_end.tm_mday = " << tm_end.tm_mday << std::endl;

	if(tm_end.tm_mday == 31) 
	{
		return 31;
	}

	if(tm_end.tm_mday == 1) 
	{
		return 30;
	}

	if(tm_end.tm_mday == 2) 
	{
		return 29;
	}

	if(tm_end.tm_mday == 3) 
	{
		return 28;
	}

	return 0;
}; 


}	// END namespace time
}	// END namespace core