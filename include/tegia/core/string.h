#ifndef H_CORE_STRING
#define H_CORE_STRING

#include <string>
#include <sstream>
#include <ctime>
#include <algorithm>

#include <regex>
#include <locale>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <codecvt>

#include <iconv.h>
#include <zlib.h>

#include <cassert>

#include "fmt/core.h"

#include <tegia/core/facet.h>
#include <tegia/core/const.h>

////////////////////////////////////////////////////////////////////////////////////////////
/**

*/
////////////////////////////////////////////////////////////////////////////////////////////


namespace tegia {
namespace base64 {

std::string encode(const std::string data);
std::string decode(const std::string& input, std::string& out);

}	// END base64 NAMESPACE
}	// END tegia  NAMESPACE


////////////////////////////////////////////////////////////////////////////////////////////
/**

*/
////////////////////////////////////////////////////////////////////////////////////////////

namespace tegia {
namespace string {

	// Заменяет английские буквы похожими русскими для исправления ошибок ввода
	char32_t _eng_to_rus(char32_t ch);
	char32_t _low(char32_t ch);
	char32_t _up(char32_t ch);

}	// END string NAMESPACE
}	// END tegia  NAMESPACE


namespace tegia {
namespace string {

	inline std::u32string str_to_u32str(const std::string &utf8)
	{
		return std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>{}.from_bytes(utf8);
	};

	inline std::u32string to_u32str(const std::string &utf8)
	{
		return std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>{}.from_bytes(utf8);
	};


	inline std::string u32str_to_str(const std::u32string &utf32str)
	{
		return std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>{}.to_bytes(utf32str);
	};

	std::string to_upper(const std::string &_str);
	std::u32string to_upper(const std::u32string &_str);

	std::string to_lower(const std::string &_str);
	std::u32string to_lower(const std::u32string &_str);

	std::string iconv(const std::string &_source, const std::string &_from_encode, const std::string &_to_encode);

	std::vector<std::string> explode(std::string source, std::string separator, bool empty = true);

	std::string quote(const std::string &data);
	std::tuple<int,std::string> dequote(const std::string &data);

	//
	//
	//

	// std::string to_latin(const std::string &cyrillicName);



	//
	// Функция для вычисления расстояния Левенштейна между двумя строками
	//

	int levenshtein(const std::string& word1, const std::string& word2, bool u32 = false);

	//
	//
	//

	std::string to_fixed_length(int number, int length);
	std::string to_fixed_length(long int number, int length);
	std::string to_fixed_length(long long int number, int length);


}	// END string NAMESPACE
}	// END tegia  NAMESPACE




/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




//
// Перегрузка оператора << для std::u32string
//

std::ostream& operator<<(std::ostream& os, const std::u32string& str);


namespace tegia {
namespace u32string {

inline std::string to_str(const std::u32string &utf32str)
{
	return std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>{}.to_bytes(utf32str);
};

std::u32string to_latin(char32_t symbol);
std::u32string to_latin(const std::u32string &u32cyrillicName);

int levenshtein(const std::u32string& word1, const std::u32string& word2);

// ---------------------------------------------------------------------------------------------
/*
	@brief Проверяет к какому алфавиту относится символ (cyr, lat)
	@param symbol Символ для проверки
	@return 
		- 0, если символ не принадлежит алфавиту
		- 1, если lat символ
		- 2, если cyr символ
		- 3, если неизвестная ошибка
*/
// ---------------------------------------------------------------------------------------------

int alphabet(char32_t symbol);

// ---------------------------------------------------------------------------------------------
/*
	@brief Проверяет, что символ относится к заданному алфавиту
	@param symbol Символ для проверки.
	@param alphabet Строка, содержащая имя алфавита.
	@return 
		- 0, если символ не принадлежит алфавиту
		- 1, если символ принадлежит алфавиту
		- 2, если алфавит не определен
*/
// ---------------------------------------------------------------------------------------------

int is_alphabet(char32_t symbol, const std::string &alphabet);

// ---------------------------------------------------------------------------------------------
/*
	@brief Проверяет, что строка содержит символы заданного алфавита
	@param string Строка для проверки.
	@param alphabet Строка, содержащая имя алфавита.
	@return 
		- 0, если в строке нет символов из заданного алфавита
		- 1, если в строке только символы из заданного алфавита
		- 2, если алфавит не определен
		- 3, если в строке есть символы из другого алфавита
*/
// ---------------------------------------------------------------------------------------------

int check_alphabet(const std::u32string &string, const std::string &alphabet);

// ---------------------------------------------------------------------------------------------
/*
	@brief Преобразует имя из ВК в латиницу 
	@param name Строка с именем из ВК для преобразования
	@return Имя в латинице
*/
// ---------------------------------------------------------------------------------------------

std::u32string vk_name(std::u32string name);



}	// END u32string NAMESPACE
}	// END tegia  NAMESPACE


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


namespace tegia {
namespace string {
namespace match {

	//
	// Проверка корректности ОГРН
	//

	inline bool ogrn(const std::string &str)
	{
		const std::regex txt_regex(R"(^[0-9]{13}$)");
		return std::regex_match(str, txt_regex);		
	};

	//
	// Проверка корректности UUID
	//

	inline bool uuid(const std::string &str)
	{		
		const std::regex txt_regex(R"(^[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{12}$)");
		return std::regex_match(str, txt_regex);
	};

}	// END namespace match
}	// END namespace string
}	// END namespace tegia












char32_t _eng_to_rus(char32_t ch);

namespace core {
namespace string {

namespace match {
	
	inline bool ip(const std::string &str)
	{
		const std::regex txt_regex("^[0-9]{1,3}.[0-9]{1,3}.[0-9]{1,3}.[0-9]{1,3}$");
		return std::regex_match(str, txt_regex);
	};

	inline bool inn_rus_person(const std::string &str)
	{
		//
		// Проверка корректности ИНН физического лица - резидента РФ
		//
		const std::regex txt_regex("^[0-9]{11,12}$");
		return std::regex_match(str, txt_regex);
	};

	inline bool date(const std::string &str)
	{
		//
		// Проверка корректности даты в формате YYYY-MM-DD
		//
		const std::regex txt_regex(R"(^([0-9]{4})\-([0-9]{2})\-([0-9]{2})$)");
		return std::regex_match(str, txt_regex);
	};

	[[deprecated]] inline bool uuid(const std::string &str)
	{
		//
		// Проверка корректности UUID
		//
		
		const std::regex txt_regex(R"(^[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{12}$)");
		return std::regex_match(str, txt_regex);
	};
}
	

	std::string clear_utf8mb4(const std::string &str);
	char get_rand_char();
	std::string random(size_t n);

	[[deprecated("use tegia::string::base64_encode()")]]
	std::string base64_encode(const std::string &str);

	[[deprecated("use tegia::string::base64_decode()")]]
	std::string base64_decode(std::string const& s);

	std::string trim(const std::string &str);

	[[deprecated("use tegia::string::str_to_u32str()")]]
	std::u32string str_to_u32str(const std::string &str);

	[[deprecated("use tegia::string::u32str_to_str()")]]
	std::string u32str_to_str(const std::u32string &u32str);

	[[deprecated("use tegia::person::normal_name()")]]
	std::string normal_name(const std::string &name);

	[[deprecated("use tegia::string::to_upper()")]]
	std::string toupper(const std::string &name);

	[[deprecated("use tegia::string::to_upper()")]]
	std::u32string toupper(const std::u32string &name);
	
	[[deprecated("use tegia::string::to_lower()")]]
	std::string to_lower(const std::string &name);
	

	class format 
	{
		public:
			format()
			{      };

			~format()
			{ m_stream.str(std::string()); };

			template<class T>
			format& operator<< (const T& arg) 
			{
				m_stream << arg;
				return *this;
			}
		
			operator std::string() const 
			{
				//m_stream << "\n";
				return m_stream.str();
			}
		protected:
			std::stringstream m_stream;
	};


	std::string decompress_string(std::string_view str);
	std::string compress_string(std::string_view str, int compressionlevel = Z_BEST_COMPRESSION);


}	// END namespace string
}	// END namespace core




#endif