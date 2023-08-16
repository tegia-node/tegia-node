#include <tegia/core/cast.h>

// https://en.cppreference.com/w/cpp/locale/codecvt

// utility wrapper to adapt locale-bound facets for wstring/wbuffer convert

/*
template<class Facet>
struct deletable_facet : Facet
{
	template<class ...Args>
	deletable_facet(Args&& ...args) : Facet(std::forward<Args>(args)...) {}
	~deletable_facet() {}
};
*/



//////////////////////////////////////////////////////////////////////////////////////////
//
//
//  std::string  ->  { TO }
//
//
//////////////////////////////////////////////////////////////////////////////////////////

namespace core { 

template<> 
std::string cast<std::string,const std::string &>(const std::string &value)
{
	return value;
};

template<> 
std::string cast<std::string,std::string>(std::string value)
{
	return value;
};

template<>
int cast<int,const std::string &>(const std::string &value)
{
	return std::stoi(value);

};

template<>
long int cast<long int,const std::string &>(const std::string &value)
{
	return std::stol(value);

};

template<>
unsigned long int cast<unsigned long int,const std::string &>(const std::string &value)
{
	return std::stoul(value);

};

template<>
long long int cast<long long int,const std::string &>(const std::string &value)
{
	return std::stoll(value);

};

template<>
unsigned long long int cast<unsigned long long int,const std::string &>(const std::string &value)
{
	return std::stoull(value);

};

template<> 
nlohmann::json cast<nlohmann::json,const std::string &>(const std::string &value)
{
	return nlohmann::json::parse(value);
};

}

//////////////////////////////////////////////////////////////////////////////////////////
//
//
//  { IN }  ->  string
//
//
//////////////////////////////////////////////////////////////////////////////////////////

namespace core
{

template<> 
std::string cast<std::string,nlohmann::json>(nlohmann::json value)
{
	return value.dump();	
};

}

//////////////////////////////////////////////////////////////////////////////////////////
//
//
//  { IN }  ->  u32string
//
//
//////////////////////////////////////////////////////////////////////////////////////////


namespace core {

template<> 
std::u32string cast<std::u32string,const std::string &>(const std::string &value)
{
	std::wstring_convert<deletable_facet<std::codecvt<char32_t, char, std::mbstate_t>>, char32_t> conv32;
	return conv32.from_bytes(value);
};

template<> 
std::u32string cast<std::u32string,std::string>(std::string value)
{
	std::wstring_convert<deletable_facet<std::codecvt<char32_t, char, std::mbstate_t>>, char32_t> conv32;
	return conv32.from_bytes(value);
};

template<> 
std::u32string cast<std::u32string,int &>(int &value)
{
	std::wstring_convert<deletable_facet<std::codecvt<char32_t, char, std::mbstate_t>>, char32_t> conv32;
	return conv32.from_bytes( std::to_string(value) );
};

template<> 
std::u32string cast<std::u32string,int>(int value)
{
	std::wstring_convert<deletable_facet<std::codecvt<char32_t, char, std::mbstate_t>>, char32_t> conv32;
	return conv32.from_bytes( std::to_string(value) );
};

template<> 
std::u32string cast<std::u32string,unsigned int &>(unsigned int &value)
{
	std::wstring_convert<deletable_facet<std::codecvt<char32_t, char, std::mbstate_t>>, char32_t> conv32;
	return conv32.from_bytes( std::to_string(value) );
};

template<> 
std::u32string cast<std::u32string,unsigned int>(unsigned int value)
{
	std::wstring_convert<deletable_facet<std::codecvt<char32_t, char, std::mbstate_t>>, char32_t> conv32;
	return conv32.from_bytes( std::to_string(value) );
};

template<> 
std::u32string cast<std::u32string,long int &>(long int &value)
{
	std::wstring_convert<deletable_facet<std::codecvt<char32_t, char, std::mbstate_t>>, char32_t> conv32;
	return conv32.from_bytes( std::to_string(value) );
};

template<> 
std::u32string cast<std::u32string,long int>(long int value)
{
	std::wstring_convert<deletable_facet<std::codecvt<char32_t, char, std::mbstate_t>>, char32_t> conv32;
	return conv32.from_bytes( std::to_string(value) );
};

template<> 
std::u32string cast<std::u32string,unsigned long int &>(unsigned long int &value)
{
	std::wstring_convert<deletable_facet<std::codecvt<char32_t, char, std::mbstate_t>>, char32_t> conv32;
	return conv32.from_bytes( std::to_string(value) );
};

template<> 
std::u32string cast<std::u32string,unsigned long int>(unsigned long int value)
{
	std::wstring_convert<deletable_facet<std::codecvt<char32_t, char, std::mbstate_t>>, char32_t> conv32;
	return conv32.from_bytes( std::to_string(value) );
};

template<> 
std::u32string cast<std::u32string,long long int &>(long long int &value)
{
	// std::cout << "[long long int &]  ->  [std::u32string] " << value << std::endl;
	std::wstring_convert<deletable_facet<std::codecvt<char32_t, char, std::mbstate_t>>, char32_t> conv32;
	return conv32.from_bytes( std::to_string(value) );
};

template<> 
std::u32string cast<std::u32string,long long int>(long long int value)
{
	// std::cout << "[long long int]  ->  [std::u32string] " << value << std::endl;
	std::wstring_convert<deletable_facet<std::codecvt<char32_t, char, std::mbstate_t>>, char32_t> conv32;
	return conv32.from_bytes( std::to_string(value) );
};

template<> 
std::u32string cast<std::u32string,unsigned long long int &>(unsigned long long int &value)
{
	std::wstring_convert<deletable_facet<std::codecvt<char32_t, char, std::mbstate_t>>, char32_t> conv32;
	return conv32.from_bytes( std::to_string(value) );
};

template<> 
std::u32string cast<std::u32string,unsigned long long int>(unsigned long long int value)
{
	std::wstring_convert<deletable_facet<std::codecvt<char32_t, char, std::mbstate_t>>, char32_t> conv32;
	return conv32.from_bytes( std::to_string(value) );
};

}  // end namespace core


//////////////////////////////////////////////////////////////////////////////////////////
//
//
//  u32string  ->  { TO }
//
//
//////////////////////////////////////////////////////////////////////////////////////////


namespace core { 

template<> 
std::string cast<std::string,const std::u32string &>(const std::u32string &u32str)
{
	std::wstring_convert<deletable_facet<std::codecvt<char32_t, char, std::mbstate_t>>, char32_t> conv32;
	return conv32.to_bytes(u32str);
};

template<> 
std::string cast<std::string,std::u32string>(std::u32string u32str)
{
	std::wstring_convert<deletable_facet<std::codecvt<char32_t, char, std::mbstate_t>>, char32_t> conv32;
	return conv32.to_bytes(u32str);
};

template<> 
int cast<int,const std::u32string &>(const std::u32string &u32str)
{
	std::wstring_convert<deletable_facet<std::codecvt<char32_t, char, std::mbstate_t>>, char32_t> conv32;
	return std::stoi(conv32.to_bytes(u32str));
};

template<> 
long int cast<long int,const std::u32string &>(const std::u32string &u32str)
{
	std::wstring_convert<deletable_facet<std::codecvt<char32_t, char, std::mbstate_t>>, char32_t> conv32;
	return std::stol(conv32.to_bytes(u32str));
};

template<> 
unsigned long int cast<unsigned long int,const std::u32string &>(const std::u32string &u32str)
{
	std::wstring_convert<deletable_facet<std::codecvt<char32_t, char, std::mbstate_t>>, char32_t> conv32;
	return std::stoul(conv32.to_bytes(u32str));
};

template<> 
long long int cast<long long int,const std::u32string &>(const std::u32string &u32str)
{
	std::wstring_convert<deletable_facet<std::codecvt<char32_t, char, std::mbstate_t>>, char32_t> conv32;
	return std::stoll(conv32.to_bytes(u32str));
};

template<> 
unsigned long long int cast<unsigned long long int,const std::u32string &>(const std::u32string &u32str)
{
	std::wstring_convert<deletable_facet<std::codecvt<char32_t, char, std::mbstate_t>>, char32_t> conv32;
	return std::stoull(conv32.to_bytes(u32str));
};

}  // end namespace core















//////////////////////////////////////////////////////////////////////////////////////////
//
//
//  other
//
//
//////////////////////////////////////////////////////////////////////////////////////////


namespace core { 


////////////////////////////////////////////////////////////////////////////
// float   to float
template<>
inline int cast(float value)
{
	return (int) value;
};


////////////////////////////////////////////////////////////////////////////
// double   to double
template<>
inline int cast(double value)
{
	return (int) value;
};


////////////////////////////////////////////////////////////////////////////
template<>
inline double cast(std::string value)
{
	if(value == "")
	{
		return 0.0;
	}
	else
	{
		// положим, что from_val и to_val - это, соответственно преобразуемое
		double to_val;
		// значение и приемник результата преобразования
		std::ostringstream o_str;
		o_str << value;

		std::istringstream i_str(o_str.str());
		i_str >> to_val;

		return to_val;
	}
};


template<>
inline std::string cast(bool value)
{
	if(value == true)
	{
		return "true";
	}
	else
	{
		return "false";
	}
};


template<>
inline bool cast(const std::string &value)
{
	if(value == "true")
	{
		return true;
	}
	else
	{
		return false;
	}
};


}

