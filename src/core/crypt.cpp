#include <tegia/core/crypt.h>
#include <random>
#include <chrono>

/////////////////////////////////////////////////////////////////////////////////
//
// Rundom
//
/////////////////////////////////////////////////////////////////////////////////


namespace tegia {
namespace random {


std::string string(int lenght)
{
	char symbols[63] = "abcdefghijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	std::mt19937 rnd(std::chrono::steady_clock::now().time_since_epoch().count());
	std::uniform_int_distribution<int> distrib(0, 61);

	char * _random = new char[lenght];
	std::memset(_random, 0, lenght);

	for(int i = 0; i < lenght; i++)
	{
		/*
		int _rnd = distrib(rnd);
		char s = symbols[_rnd];
		std::cout << i << " [" << _rnd << "] [" << s << "]" << std::endl;
		_random[i] = s;
		*/
		_random[i] = symbols[distrib(rnd)];
	}
	std::string random('0',lenght);
	random.assign(&_random[0],&_random[0] + lenght);

	delete [] _random;
	return random;
};


std::string password(int lenght)
{
	srand( std::time(nullptr)); //инициализируем генератор случайных чисел
	char *pass = new char[lenght + 1]; //выделяем память для строки пароля
	for (int i = 0; i < lenght; ++ i) 
	{
		switch(rand() % 3) //генерируем случайное число от 0 до 2
		{
			case 0: //если ноль
			{
				pass[i] = rand() % 10 + '0'; //вставляем в пароль случайную цифру
			}
			break;
			
			case 1: //если единица
			{
				pass[i] = rand() % 26 + 'A'; //вставляем случайную большую букву
			}	
			break;

			case 2: //если двойка
			{
				pass[i] = rand() % 26 + 'a'; //вставляем случайную маленькую букву
			}
			break;
		}
	}
	pass[lenght+1] = '\0';
	std::string password(pass);
	delete[] pass;

	return password;
};



std::string uuid()
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


}	// END namespace random
}	// END namespace tegia


/////////////////////////////////////////////////////////////////////////////////
//
// MD5
//
/////////////////////////////////////////////////////////////////////////////////

namespace tegia {
namespace crypt {

std::string MD5(const std::string &str)
{
	unsigned char digest[16];
	const char* string = str.c_str();
	MD5_CTX context;
	MD5_Init(&context);
	MD5_Update(&context, string, strlen(string));
	MD5_Final(digest, &context);

	char md5string[33];
	for(int i = 0; i < 16; ++i)
		sprintf(&md5string[i*2], "%02x", (unsigned int)digest[i]);

	return (const char*) md5string;
};

}	// END namespace crypt
}	// END namespace tegia

