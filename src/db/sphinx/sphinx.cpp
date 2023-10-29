
#include <tegia/db/sphinx/sphinx.h>

namespace tegia {
namespace sphinx {

	/////////////////////////////////////////////////////////////////////////////////////////////
	/**
	 *	\brief		Выполняет экранирование строки для SQL-запроса
	 *	\authors	Горячев Игорь 
	 *	\details
	 */
	/////////////////////////////////////////////////////////////////////////////////////////////

	/*
		$from = array ( '\\', '(',')','|','-','!','@','~','"','&', '/', '^', '$', '=', "'", "\x00", "\n", "\r", "\x1a" );
		$to   = array ( '\\\\', '\\\(','\\\)','\\\|','\\\-','\\\!','\\\@','\\\~','\\\"', '\\\&', '\\\/', '\\\^', '\\\$', '\\\=', "\\'", "\\x00", "\\n", "\\r", "\\x1a" );
	*/

	std::string escape(const std::string &str)
	{
		std::string _str;
		_str.reserve( 3 * str.length() );

		bool flag = true;
		int state = 1;
		int index = 0;
		long long int length = str.length();

		while(flag)
		{
			switch(state)
			{
				case 0:
				{
					flag = false;
				}
				break;

				case 1:
				{
					if(index == length)
					{
						state = 0;
						break;
					}

					if(str[index] == '(')
					{
						_str = _str + R"(\\\()";
						index++;
						break;
					}

					if(str[index] == ')')
					{
						_str = _str + R"(\\\))";
						index++;
						break;
					}

					if(str[index] == '/')
					{
						_str = _str + R"(\\\/)";
						index++;
						break;
					}

					if(str[index] == '-')
					{
						_str = _str + R"(\\\-)";
						index++;
						break;
					}

					if(str[index] == '"')
					{
						_str = _str + R"(\\\")";
						index++;
						break;
					}

					if(str[index] == '@')
					{
						_str = _str + R"(\\\@)";
						index++;
						break;
					}

					_str = _str + str[index];
					index++;

				}
				break;
			}
		}

		return _str;
	};

}	// END namespace sphinx
}	// END namespace tegia
