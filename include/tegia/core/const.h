#ifndef H_CORE_CONST
#define H_CORE_CONST

//
// Управляющие последовательности для вывода цветного текста в консоль
// https://habr.com/ru/post/119436/ 
//
	
#define _BOLD_TEXT_				"\033[1m"

#define _BASE_TEXT_				"\033[0m"

#define _RED_TEXT_				"\033[31m"
#define _GREEN_TEXT_			"\033[32m"
#define _GREEN_					"\033[32m"

#define _YELLOW_				"\033[33m"		// желтый цвет знаков

#define _BLUE_TEXT_				"\033[34m"		// 
#define _BLUE_					"\033[34m"		// 
#define _BBLUE_					"\033[1;34m"	// 

#define _CYAN_TEXT_				"\033[36m"		// цвет морской волны знаков
#define _CYAN_					"\033[36m"		// цвет морской волны знаков
#define _BCYAN_					"\033[1;36m"		// цвет морской волны знаков
#define _GRAY_TEXT_				"\033[37m"

/*   
\033[30    чёрный цвет знаков
\033[35    фиолетовый цвет знаков
*/

#define _OK_TEXT_  "\033[32m[OK] \033[0m "
#define _ERR_TEXT_ "\033[31m[ERR]\033[0m "


#endif