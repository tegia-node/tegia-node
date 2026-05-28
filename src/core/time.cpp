#include <tegia/core/time.h>
#include <tegia/core/const.h>

#include <date/date.h>

//
//
//


namespace tegia {

struct time_t::time_impl
{
    date::sys_seconds value;
};


time_t::time_t()
    : time(new time_impl())
{}

time_t::~time_t()
{
    delete time;
}


std::string time_t::format(const std::string &format)
{
	return date::format(format, this->time->value);
};


int time_t::parse(const std::string& str)
{
	///////////////////////////////////////////////////////////////////////
	/*
		ISO 8601
		2026-05-27T06:00:38Z или с offset
	*/
	///////////////////////////////////////////////////////////////////////


	if (str.find('T') != std::string::npos &&
       (str.find('Z') != std::string::npos || 
	    str.find('+') != std::string::npos))
    {
        std::istringstream in(str);
        date::sys_seconds tp;

        in >> date::parse("%FT%TZ", tp);

        if (in.fail() == false)
		{
			this->time->value = tp;
			return 0;
		}

        in.clear();
        in.str(str);

        in >> date::parse("%FT%T%z", tp);

        if (in.fail() == false)
		{
			this->time->value = tp;
			return 0;
		}

        in.clear();
        in.str(str);

        in >> date::parse("%FT%T%Ez", tp);

        if (in.fail() == false)
		{
			this->time->value = tp;
			return 0;
		}
    }


	///////////////////////////////////////////////////////////////////////
	/*
		RFC 2822
		Wed, 27 May 2026 06:00:38 +0000
	*/
	///////////////////////////////////////////////////////////////////////


    if (str.find(',') != std::string::npos &&
        str.find(':') != std::string::npos)
    {
        std::istringstream in(str);
        date::sys_seconds tp;

        in >> date::parse("%a, %d %b %Y %H:%M:%S %z", tp);

        if (in.fail() == false)
		{
			this->time->value = tp;
			return 0;
		}
    }


	///////////////////////////////////////////////////////////////////////
	/*
		ERROR
		not found pattern
	*/
	///////////////////////////////////////////////////////////////////////


	std::cout << _ERR_TEXT_ << "Failed to parse time string: " << str << std::endl;
	exit(0);
};


}



//
//
//


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