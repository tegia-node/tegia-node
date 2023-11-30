#include <tegia/core/string.h>


////////////////////////////////////////////////////////////////////////////////////////////
/** 

*/   
////////////////////////////////////////////////////////////////////////////////////////////


namespace tegia {
namespace base64 {

std::string encode(const std::string data) 
{
	static constexpr char sEncodingTable[] = {
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
	'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
	'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
	'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
	'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
	'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
	'w', 'x', 'y', 'z', '0', '1', '2', '3',
	'4', '5', '6', '7', '8', '9', '+', '/'
	};

	size_t in_len = data.size();
	size_t out_len = 4 * ((in_len + 2) / 3);
	std::string ret(out_len, '\0');
	size_t i;
	char *p = const_cast<char*>(ret.c_str());

	for (i = 0; i < in_len - 2; i += 3) 
	{
		*p++ = sEncodingTable[(data[i] >> 2) & 0x3F];
		*p++ = sEncodingTable[((data[i] & 0x3) << 4) | ((int) (data[i + 1] & 0xF0) >> 4)];
		*p++ = sEncodingTable[((data[i + 1] & 0xF) << 2) | ((int) (data[i + 2] & 0xC0) >> 6)];
		*p++ = sEncodingTable[data[i + 2] & 0x3F];
	}

	if (i < in_len) 
	{
		*p++ = sEncodingTable[(data[i] >> 2) & 0x3F];
		if (i == (in_len - 1)) {
			*p++ = sEncodingTable[((data[i] & 0x3) << 4)];
			*p++ = '=';
		}
		else {
			*p++ = sEncodingTable[((data[i] & 0x3) << 4) | ((int) (data[i + 1] & 0xF0) >> 4)];
			*p++ = sEncodingTable[((data[i + 1] & 0xF) << 2)];
		}
		*p++ = '=';
	}

	return ret;
};



std::string decode(const std::string& input, std::string& out) 
{
	static constexpr unsigned char kDecodingTable[] = {
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63,
	52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64, 64,
	64,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
	15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64,
	64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
	41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64
	};

	size_t in_len = input.size();
	if (in_len % 4 != 0) return "Input data size is not a multiple of 4";

	size_t out_len = in_len / 4 * 3;
	if (input[in_len - 1] == '=') out_len--;
	if (input[in_len - 2] == '=') out_len--;

	out.resize(out_len);

	for (size_t i = 0, j = 0; i < in_len;) 
	{
		uint32_t a = input[i] == '=' ? 0 & i++ : kDecodingTable[static_cast<int>(input[i++])];
		uint32_t b = input[i] == '=' ? 0 & i++ : kDecodingTable[static_cast<int>(input[i++])];
		uint32_t c = input[i] == '=' ? 0 & i++ : kDecodingTable[static_cast<int>(input[i++])];
		uint32_t d = input[i] == '=' ? 0 & i++ : kDecodingTable[static_cast<int>(input[i++])];

		uint32_t triple = (a << 3 * 6) + (b << 2 * 6) + (c << 1 * 6) + (d << 0 * 6);

		if (j < out_len) out[j++] = (triple >> 2 * 8) & 0xFF;
		if (j < out_len) out[j++] = (triple >> 1 * 8) & 0xFF;
		if (j < out_len) out[j++] = (triple >> 0 * 8) & 0xFF;
	}

	return "";
}

}	// END base64 NAMESPACE
}	// END tegia  NAMESPACE


////////////////////////////////////////////////////////////////////////////////////////////
/** 

*/   
////////////////////////////////////////////////////////////////////////////////////////////


// https://en.cppreference.com/w/cpp/locale/codecvt

// utility wrapper to adapt locale-bound facets for wstring/wbuffer convert
template<class Facet>
struct deletable_facet : Facet
{
		template<class ...Args>
		deletable_facet(Args&& ...args) : Facet(std::forward<Args>(args)...) {}
		~deletable_facet() {}
};


char32_t _low(char32_t ch)
{
	union 
	{
		char32_t _ch;
		long long int _int;
	} _ch;
	_ch._ch = ch;

	switch(_ch._int)
	{
		//
		// ENG
		//

		case 65: return U'a';
		case 66: return U'b';
		case 67: return U'c';
		case 68: return U'd';
		case 69: return U'e';
		case 70: return U'f';
		case 71: return U'g';
		case 72: return U'h';
		case 73: return U'i';
		case 74: return U'j';
		case 75: return U'k';
		case 76: return U'l';
		case 77: return U'm';
		case 78: return U'n';
		case 79: return U'o';
		case 80: return U'p';
		case 81: return U'q';
		case 82: return U'r';
		case 83: return U's';
		case 84: return U't';
		case 85: return U'u';
		case 86: return U'v';
		case 87: return U'w';
		case 88: return U'x';
		case 89: return U'y';
		case 90: return U'z';

		// 
		// RUS
		//

		case 1040: return U'а';
		case 1041: return U'б';
		case 1042: return U'в';
		case 1043: return U'г';
		case 1044: return U'д';
		case 1045: return U'е';
		case 1046: return U'ж';
		case 1047: return U'з';
		case 1048: return U'и';
		case 1049: return U'й';
		case 1050: return U'к';
		case 1051: return U'л';
		case 1052: return U'м';
		case 1053: return U'н';
		case 1054: return U'о';
		case 1055: return U'п';
		case 1056: return U'р';
		case 1057: return U'с';
		case 1058: return U'т';
		case 1059: return U'у';
		case 1060: return U'ф';
		case 1061: return U'х';
		case 1062: return U'ц';
		case 1063: return U'ч';
		case 1064: return U'ш';
		case 1065: return U'щ';
		case 1066: return U'ъ';
		case 1067: return U'ы';
		case 1068: return U'ь';
		case 1069: return U'э';
		case 1070: return U'ю';
		case 1071: return U'я';

		case 1025: return U'ё';

		default:
		{
			// std::cout << _ch._int << std::endl;
			return ch;			
		}
		break;

	}

	return ch;
};


char32_t _up(char32_t ch)
{
	union 
	{
		char32_t _ch;
		long long int _int;
	} _ch;
	_ch._ch = ch;

	switch(_ch._int)
	{
		//
		// ENG
		//

		case 97:  return u'A';
		case 98:  return u'B';
		case 99:  return u'C';
		case 100: return u'D';
		case 101: return u'E';
		case 102: return u'F';
		case 103: return u'G';
		case 104: return u'H';
		case 105: return u'I';
		case 106: return u'J';
		case 107: return u'K';
		case 108: return u'L';
		case 109: return u'M';
		case 110: return u'N';
		case 111: return u'O';
		case 112: return u'P';
		case 113: return u'Q';
		case 114: return u'R';
		case 115: return u'S';
		case 116: return u'T';
		case 117: return u'U';
		case 118: return u'V';
		case 119: return u'W';
		case 120: return u'X';
		case 121: return u'Y';
		case 122: return u'Z';

		//
		// RUS
		//

		case 1072: return u'А';
		case 1073: return u'Б';
		case 1074: return u'В';
		case 1075: return u'Г';
		case 1076: return u'Д';
		case 1077: return u'Е';
		case 1078: return u'Ж';
		case 1079: return u'З';
		case 1080: return u'И';
		case 1081: return u'Й';
		case 1082: return u'К';
		case 1083: return u'Л';
		case 1084: return u'М';
		case 1085: return u'Н';
		case 1086: return u'О';
		case 1087: return u'П';
		case 1088: return u'Р';
		case 1089: return u'С';
		case 1090: return u'Т';
		case 1091: return u'У';
		case 1092: return u'Ф';
		case 1093: return u'Х';
		case 1094: return u'Ц';
		case 1095: return u'Ч';
		case 1096: return u'Ш';
		case 1097: return u'Щ';
		case 1098: return u'Ъ';
		case 1099: return u'Ы';
		case 1100: return u'Ь';
		case 1101: return u'Э';
		case 1102: return u'Ю';
		case 1103: return u'Я';

		case 1105: return u'Ё';

		default:
		{
			// std::cout << _ch._int << std::endl;
			return ch;
		}
		break;
	}
};


////////////////////////////////////////////////////////////////////////////////////////////
/** 

*/   
////////////////////////////////////////////////////////////////////////////////////////////

namespace tegia {
namespace string {

	/*
	std::u32string str_to_u32str(const std::string &utf8)
	{
		auto conv32 = std::wstring_convert<deletable_facet<std::codecvt_byname<char32_t, char, std::mbstate_t>>, char32_t>{};
		//auto conv32 = std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>{};
		return conv32.from_bytes(utf8);
	};
	*/



	std::string to_upper(const std::string &_str)
	{
		//  conv32;
		auto conv32 = std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>{};
		std::u32string u32str = conv32.from_bytes(_str);

		u32str.erase(std::remove(u32str.begin(), u32str.end(), '\n'), u32str.end());
		u32str.erase(std::remove(u32str.begin(), u32str.end(), '\r'), u32str.end());

		for(size_t i = 0; i < u32str.size(); ++i)		
		{
			u32str[i] = _up(u32str[i]);
		}

		return conv32.to_bytes(u32str);
	};


	std::u32string to_upper(const std::u32string &_str)
	{
		std::u32string u32str = _str;
		u32str.erase(std::remove(u32str.begin(), u32str.end(), '\n'), u32str.end());
		u32str.erase(std::remove(u32str.begin(), u32str.end(), '\r'), u32str.end());

		for(size_t i = 0; i < u32str.size(); ++i)		
		{
			u32str[i] = _up(u32str[i]);
		}

		return u32str;
	};


	std::string to_lower(const std::string &_str)
	{
		// std::wstring_convert<deletable_facet<std::codecvt<char32_t, char, std::mbstate_t>>, char32_t> conv32;
		auto conv32 = std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>{};
		std::u32string u32str = conv32.from_bytes(_str);

		u32str.erase(std::remove(u32str.begin(), u32str.end(), '\n'), u32str.end());
		u32str.erase(std::remove(u32str.begin(), u32str.end(), '\r'), u32str.end());

		for(size_t i = 0; i < u32str.size(); ++i)		
		{
			u32str[i] = _low(u32str[i]);
		}

		return conv32.to_bytes(u32str);
	};


	std::u32string to_lower(const std::u32string &_str)
	{
		std::u32string u32str = _str;
		u32str.erase(std::remove(u32str.begin(), u32str.end(), '\n'), u32str.end());
		u32str.erase(std::remove(u32str.begin(), u32str.end(), '\r'), u32str.end());

		for(size_t i = 0; i < u32str.size(); ++i)		
		{
			u32str[i] = _low(u32str[i]);
		}

		return u32str;
	};


	std::string iconv(const std::string &_source, const std::string &_from_encode, const std::string &_to_encode)
	{
		char *in = const_cast<char*>(_source.c_str());
		size_t inbytes = _source.length();

		char *out = new char[inbytes * 4];
		std::memset(out,0,inbytes * 4);
		char *result_to_out = out;
		size_t outbytes = sizeof(char) * (inbytes * 4);

		iconv_t conv = ::iconv_open(_to_encode.c_str(),_from_encode.c_str());

		if (conv == (iconv_t)-1) 
		{
			perror("iconv_open");
			exit(0);

			// TODO:
		}

		auto res = ::iconv(conv, (char **)&in, &inbytes, (char **)&out, &outbytes);
		if (res == (size_t)-1) 
		{
			perror("iconv");
			exit(0);

			// TODO:
		}
		
		std::string result(result_to_out,outbytes);
		delete[] result_to_out;

		::iconv_close(conv);

		return result;
	};



}	// END string NAMESPACE
}	// END tegia  NAMESPACE





























////////////////////////////////////////////////////////////////////////////////////////////
/** 

*/   
////////////////////////////////////////////////////////////////////////////////////////////



// Заменяет английские буквы похожими русскими для исправления ошибок ввода
char32_t _eng_to_rus(char32_t ch)
{
	if(ch == U'6') 
	{
		return U'б';
	}

	if(ch == U'P') 
	{
		return U'Р';
	}
	if(ch == U'p') 
	{
		return U'р';
	}

	if(ch == U'E') 
	{
		return U'Е';
	}
	if(ch == U'e') 
	{
		return U'е';
	}

	if(ch == U'C') 
	{
		return U'С';
	}
	if(ch == U'c') 
	{
		return U'с';
	}

	if(ch == U'A') 
	{
		return U'А';
	}
	if(ch == U'a') 
	{
		return U'а';
	}

	return ch;
};




////////////////////////////////////////////////////////////////////////////////////////////
/** 

*/   
////////////////////////////////////////////////////////////////////////////////////////////


static const std::string base64_chars = 
						 "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
						 "abcdefghijklmnopqrstuvwxyz"
						 "0123456789+/";


static inline bool is_base64(unsigned char c) {
	return (isalnum(c) || (c == '+') || (c == '/'));
}


namespace core {
namespace string {
			
	std::string clear_utf8mb4(const std::string &str)
	{
		std::string tmp = str;
		std::string sumbol = "\xF0";
		std::size_t found = tmp.find(sumbol);
		while (found != std::string::npos)
		{
			tmp.replace(found, sumbol.size() * 4," ");
			found = tmp.find(sumbol);
		}
		return tmp;
	};

	char get_rand_char() 
	{
		static std::string charset("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
		return charset[std::rand() % charset.size()];
	};

	std::string random(size_t n) 
	{
		std::srand(std::time(nullptr));
		char * rbuf = new char[n];
		std::generate(rbuf, rbuf+n, &get_rand_char);
		std::string _ret = std::string(rbuf, n);
		delete[] rbuf;
		return _ret;
	};

	std::string trim(const std::string &str)
	{
		std::string result = str;

		// Удаляем переносы строки

		size_t index = 0;
		while( (index = result.find('\n')) != std::string::npos )
		{
			result.erase(index, 1);
		}

		index = 0;
		while( (index = result.find('\r')) != std::string::npos )
		{
			result.erase(index, 1);
		}

		// Удаляем пробелы в начале и конце строки

		size_t strBegin=result.find_first_not_of(' ');
		size_t strEnd=result.find_last_not_of(' ');
		result.erase(strEnd+1, result.size() - strEnd);
		result.erase(0, strBegin);

		return result;
	};

	// ===========================================================================================================

	std::u32string str_to_u32str(const std::string &str)
	{
		std::wstring_convert<deletable_facet<std::codecvt<char32_t, char, std::mbstate_t>>, char32_t> conv32;
		return conv32.from_bytes(str);
	};

	std::string u32str_to_str(const std::u32string &u32str)
	{
		std::wstring_convert<deletable_facet<std::codecvt<char32_t, char, std::mbstate_t>>, char32_t> conv32;
		return conv32.to_bytes(u32str);
	};

	std::string normal_name(const std::string &name)
	{
		//std::cout << "   old [" << name << "]" << std::endl;
		
		std::wstring_convert<deletable_facet<std::codecvt<char32_t, char, std::mbstate_t>>, char32_t> conv32;
		std::u32string u32str = conv32.from_bytes(name);

		u32str.erase(std::remove(u32str.begin(), u32str.end(), '\n'), u32str.end());
		u32str.erase(std::remove(u32str.begin(), u32str.end(), '\r'), u32str.end());

		int state = 0;

		//  0 - начальное состояние - убираем пробелы в начале строки
		//  1 - состояние начала имени - нужно написать имя с прописной буквы
		//  2 - состояние середины имени - нужно писать строчные буквы
		//  3 - условная "середина" сложного имени тип Анна-Мария или Байрам Оглы
		//  4 - состояние окончания имени - нужно убрать концевые пробелы, если они есть

		int length = u32str.size();
		int i = 0;

		while(i < length)
		{
			switch(state)
			{
				case 0:
				{
					if(u32str[i] == U' ')
					{
						u32str.erase(0,1);
						i++;
					}
					else
					{
						state = 1;
					}
				}
				break;

				case 1:
				{
					// В написании имен встречаются варианты, когда русская юуква заменена (ошибочно или специально) на похожую английскую
					u32str[i] = _eng_to_rus(u32str[i]);
					u32str[i] = _up(u32str[i]);
					i++;
					state = 2;
				}
				break;

				case 2:
				{
					if(u32str[i] == U' ' || u32str[i] == U'-')
					{
						// Условная "середина" сложного имени тип Анна-Мария или Байрам Оглы
						state = 3;
					}
					else
					{
						u32str[i] = _eng_to_rus(u32str[i]);
						u32str[i] = _low(u32str[i]);
						i++;
					}
				}
				break;

				case 3:
				{
					// Находимся "в середине" имени, пока не получим новый символ
					if(u32str[i] == U' ' || u32str[i] == U'-')
					{
						i++;
					}
					else
					{
						state = 1;
					}
				}
				break;
			}
		}

		// Не забыть подчистить пробелы в конце имени
		while(true)
		{
			if(u32str.back() == U' ')
			{
				u32str.pop_back();
			}
			else
			{
				break;
			}
		}

		std::string name_ = conv32.to_bytes(u32str);
 
		//std::cout << "   new [" << name_ << "]" << std::endl;

		return name_;
	};



	std::string toupper(const std::string &name)
	{
		std::wstring_convert<deletable_facet<std::codecvt<char32_t, char, std::mbstate_t>>, char32_t> conv32;
		std::u32string u32str = conv32.from_bytes(name);

		u32str.erase(std::remove(u32str.begin(), u32str.end(), '\n'), u32str.end());
		u32str.erase(std::remove(u32str.begin(), u32str.end(), '\r'), u32str.end());

		for(size_t i = 0; i < u32str.size(); ++i)		
		{
			u32str[i] = _up(u32str[i]);
		}

		return conv32.to_bytes(u32str);
	};



	std::u32string toupper(const std::u32string &name)
	{
		std::u32string u32str = name;
		u32str.erase(std::remove(u32str.begin(), u32str.end(), '\n'), u32str.end());
		u32str.erase(std::remove(u32str.begin(), u32str.end(), '\r'), u32str.end());

		for(size_t i = 0; i < u32str.size(); ++i)		
		{
			u32str[i] = _up(u32str[i]);
		}

		return u32str;
	};



	std::string to_lower(const std::string &name)
	{
		std::wstring_convert<deletable_facet<std::codecvt<char32_t, char, std::mbstate_t>>, char32_t> conv32;
		std::u32string u32str = conv32.from_bytes(name);

		u32str.erase(std::remove(u32str.begin(), u32str.end(), '\n'), u32str.end());
		u32str.erase(std::remove(u32str.begin(), u32str.end(), '\r'), u32str.end());

		for(size_t i = 0; i < u32str.size(); ++i)		
		{
			u32str[i] = _low(u32str[i]);
		}

		return conv32.to_bytes(u32str);
	};



/*std::string iconv(const std::string &str, const std::string &to, const std::string &from)
{
	iconv_t cd;
	size_t k, f, t;
	int se;

	const char *code = "Вопрос!";
	const char* in = code;
	char buf[100];
	char* out = buf;

	cd = iconv_open("cp1251", "koi8-r");
	if( cd == (iconv_t)(-1) )
		err( 1, "iconv_open" );
	f = strlen(code);
	t = sizeof buf;
	memset( &buf, 0, sizeof buf );
	errno = 0;
	k = iconv(cd, &in, &f, &out, &t);
	se = errno;
	printf( "converted: %u,error=%d\n", (unsigned) k, se );

	printf("string: %s\n", buf);

	iconv_close(cd);

}*/


	static constexpr std::size_t CHUNK_SIZE = 64UL * 1024UL;

	/** Compress a STL string using zlib with given compression level and return
	 * the binary data. */
	std::string compress_string(std::string_view const str,	int compressionlevel)
	{
		z_stream zs;                        // z_stream is zlib's control structure
		memset(&zs, 0, sizeof(zs));

		if (deflateInit(&zs, compressionlevel) != Z_OK)
			throw(std::runtime_error("deflateInit failed while compressing."));

		zs.next_in = const_cast<Bytef*>(reinterpret_cast<const Bytef*>(str.data()));
		zs.avail_in = str.size();           // set the z_stream's input

		int ret;
		char outbuffer[CHUNK_SIZE];
		std::string outstring{};

		// retrieve the compressed bytes blockwise
		do {
			zs.next_out = reinterpret_cast<Bytef*>(outbuffer);
			zs.avail_out = CHUNK_SIZE;

			ret = deflate(&zs, Z_FINISH);

			if (outstring.size() < zs.total_out) {
				// append the block to the output string
				outstring.append(outbuffer,
								zs.total_out - outstring.size());
			}
		} while (ret == Z_OK);

		deflateEnd(&zs);

		if (ret != Z_STREAM_END) {          // an error occurred that was not EOF
			std::ostringstream oss;
			oss << "Exception during zlib compression: (" << ret << ") " << zs.msg;
			throw(std::runtime_error(oss.str()));
		}

		return outstring;
	}


	/** Decompress an STL string using zlib and return the original data. */
	std::string decompress_string(std::string_view const str)
	{
		z_stream zs;                        // z_stream is zlib's control structure
		memset(&zs, 0, sizeof(zs));

		if (inflateInit(&zs) != Z_OK)
			throw(std::runtime_error("inflateInit failed while decompressing."));

		zs.next_in = const_cast<Bytef*>(reinterpret_cast<const Bytef*>(str.data()));
		zs.avail_in = str.size();

		int ret;
		char outbuffer[CHUNK_SIZE];
		std::string outstring;

		// get the decompressed bytes blockwise using repeated calls to inflate
		do {
			zs.next_out = reinterpret_cast<Bytef*>(outbuffer);
			zs.avail_out = CHUNK_SIZE;

			ret = inflate(&zs, 0);

			if (outstring.size() < zs.total_out) {
				outstring.append(outbuffer,
								zs.total_out - outstring.size());
			}

		} while (ret == Z_OK);

		inflateEnd(&zs);

		if (ret != Z_STREAM_END) {          // an error occurred that was not EOF
			std::ostringstream oss;
			oss << "Exception during zlib decompression: (" << ret << ") "
				<< zs.msg;
			throw(std::runtime_error(oss.str()));
		}

		return outstring;
	}


std::string base64_encode(const std::string &str) {
	std::string ret;
	int i = 0;
	int j = 0;
	unsigned char char_array_3[3];
	unsigned char char_array_4[4];

	unsigned int in_len = str.length();
	unsigned char const* bytes_to_encode = reinterpret_cast<const unsigned char*>(str.c_str());

	while (in_len--) {
		char_array_3[i++] = *(bytes_to_encode++);
		if (i == 3) {
			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;

			for(i = 0; (i <4) ; i++)
				ret += base64_chars[char_array_4[i]];
			i = 0;
		}
	}

	if (i)
	{
		for(j = i; j < 3; j++)
			char_array_3[j] = '\0';

		char_array_4[0] = ( char_array_3[0] & 0xfc) >> 2;
		char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
		char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);

		for (j = 0; (j < i + 1); j++)
			ret += base64_chars[char_array_4[j]];

		while((i++ < 3))
			ret += '=';

	}

	return ret;

}

std::string base64_decode(std::string const& encoded_string) {
	int in_len = encoded_string.size();
	int i = 0;
	int j = 0;
	int in_ = 0;
	unsigned char char_array_4[4], char_array_3[3];
	std::string ret;

	while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
		char_array_4[i++] = encoded_string[in_]; in_++;
		if (i ==4) {
			for (i = 0; i <4; i++)
				char_array_4[i] = base64_chars.find(char_array_4[i]);

			char_array_3[0] = ( char_array_4[0] << 2       ) + ((char_array_4[1] & 0x30) >> 4);
			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
			char_array_3[2] = ((char_array_4[2] & 0x3) << 6) +   char_array_4[3];

			for (i = 0; (i < 3); i++)
				ret += char_array_3[i];
			i = 0;
		}
	}

	if (i) {
		for (j = 0; j < i; j++)
			char_array_4[j] = base64_chars.find(char_array_4[j]);

		char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
		char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);

		for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
	}

	return ret;
}




	}



}




