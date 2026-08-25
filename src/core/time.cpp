#include <tegia/core/time.h>

#include <date/date.h>

#include <cctype>
#include <string_view>


namespace {

char ascii_lower(unsigned char ch)
{
	if(ch >= 'A' && ch <= 'Z')
	{
		return static_cast<char>(ch + ('a' - 'A'));
	}

	return static_cast<char>(ch);
}


bool ascii_iequals(std::string_view left, std::string_view right)
{
	if(left.size() != right.size())
	{
		return false;
	}

	for(std::size_t i = 0; i < left.size(); ++i)
	{
		if(ascii_lower(static_cast<unsigned char>(left[i])) !=
		   ascii_lower(static_cast<unsigned char>(right[i])))
		{
			return false;
		}
	}

	return true;
}


std::string normalize_rfc_utc_zone(const std::string &value)
{
	std::size_t end = value.size();
	while(end > 0 && std::isspace(static_cast<unsigned char>(value[end - 1])) != 0)
	{
		end--;
	}

	std::size_t zone_begin = end;
	while(zone_begin > 0 && std::isspace(static_cast<unsigned char>(value[zone_begin - 1])) == 0)
	{
		zone_begin--;
	}

	const std::string_view zone(value.data() + zone_begin, end - zone_begin);
	if(ascii_iequals(zone, "GMT") == false &&
	   ascii_iequals(zone, "UT") == false &&
	   ascii_iequals(zone, "UTC") == false)
	{
		return value;
	}

	// GMT, UT и UTC обозначают одно и то же абсолютное смещение.
	return value.substr(0, zone_begin) + "+0000";
}

}

//
//
//


namespace tegia {

struct time_t::time_impl
{
    date::sys_seconds value;
    bool valid = false;
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
	if(this->time->valid == false)
	{
		return "";
	}

	return date::format(format, this->time->value);
};


bool time_t::valid() const
{
	return this->time->valid;
};


int time_t::parse(const std::string& str)
{
	this->time->valid = false;

	auto try_parse = [this](const std::string &value, const char *format) -> bool
	{
		std::istringstream input(value);
		date::sys_seconds parsed_value;

		input >> date::parse(format, parsed_value);
		if(input.fail() == true)
		{
			return false;
		}

		// После даты допустимы только пробельные символы.
		char trailing = '\0';
		if(input >> trailing)
		{
			return false;
		}

		this->time->value = parsed_value;
		this->time->valid = true;
		return true;
	};

	// RFC 3339 / ISO 8601 в UTC или с положительным/отрицательным offset.
	if(try_parse(str, "%FT%TZ") == true ||
	   try_parse(str, "%FT%T%Ez") == true ||
	   try_parse(str, "%FT%T%z") == true)
	{
		return 0;
	}

	// RFC 2822: Wed, 27 May 2026 06:00:38 +0000 или ... GMT.
	const std::string rfc_value = normalize_rfc_utc_zone(str);
	if(try_parse(rfc_value, "%a, %d %b %Y %H:%M:%S %z") == true)
	{
		return 0;
	}

	return 400;
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
