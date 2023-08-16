#include <tegia/const.h>
#include <tegia/core/core.h>



namespace core { 


////////////////////////////////////////////////////////////////////////////////////////////
/**
	\brief
	\detail
	
*/   
////////////////////////////////////////////////////////////////////////////////////////////

std::string read_from_file(const std::string& path)
{
	std::string contents;
	std::ifstream is{path, std::ifstream::binary};

	if (is) 
	{
		// get length of file:
		is.seekg (0, is.end);
		auto length = is.tellg();
		is.seekg (0, is.beg);
		contents.resize(length);

		is.read(&contents[0], length);
		if (!is) 
		{
			is.close();
			return {};
		}
	} 
	else 
	{
		std::cerr << _RED_TEXT_ << "file " << path << " not found" << _BASE_TEXT_ << std::endl;
	}

	is.close();
	return contents;
};

////////////////////////////////////////////////////////////////////////////////////////////
/**
	\brief  Генериует глобальный уникальный идентификатор 
	\detail
	
*/   
////////////////////////////////////////////////////////////////////////////////////////////

std::string generateUUID()
{
	uuid_t out;
	uuid_generate(out);

	if(uuid_is_null(out) == 1)
	{
		return "";
	}
	else
	{
		char *str = new char[37];
		uuid_unparse_lower(out, str);
		std::string uuid_str = (const char*) str;
		delete[] str;

		return uuid_str;
	}
};


////////////////////////////////////////////////////////////////////////////////////////////
/**
	\brief  
	\detail
	
*/   
////////////////////////////////////////////////////////////////////////////////////////////


unsigned int timestamp(const std::string &str, const std::string &format)
{

	//std::chrono::system_clock::time_point string_to_time_point(const std::string &str)


	int yyyy = 0;
	int mm  = 0;
	int dd  = 0;
	int HH  = 0;
	int MM  = 0;
	int SS  = 0;
	int fff = 0;

	//char scanf_format[] = "%4d.%2d.%2d-%2d.%2d.%2d.%3d";

	sscanf(str.c_str(), format.c_str(), &yyyy, &mm, &dd, &HH, &MM, &SS, &fff);

	tm ttm = tm();
	ttm.tm_year = yyyy - 1900; // Year since 1900
	ttm.tm_mon = mm - 1; // Month since January 
	ttm.tm_mday = dd; // Day of the month [1-31]
	ttm.tm_hour = HH; // Hour of the day [00-23]
	ttm.tm_min = MM;
	ttm.tm_sec = SS;

	time_t ttime_t = mktime(&ttm);


	//system_clock::time_point time_point_result = std::chrono::system_clock::from_time_t(ttime_t);
	//int unix_timestamp = std::chrono::seconds(std::time(NULL));
	//time_point_result += std::chrono::milliseconds(fff);
	//return time_point_result;

	return (int) ttime_t;

};
	


} // END namespace core

