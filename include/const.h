#ifndef H_CONST
#define H_CONST


// Определение типов полей данных
   #define TYPE_INT      1
   #define TYPE_DOUBLE   2
   #define TYPE_STRING   3
   #define TYPE_TEXT     4

   //#define TYPE_DATETIME 5

  
// Приоритет выполнения обработчика  
   #define _PHIGHT_     0 
   #define _PMEDIUM_    1 
   #define _PLOW_       2 


// Базовые типы MySQL
   #define TYPE_TINYINT1      1
   #define TYPE_BOOLEAN       1
   #define TYPE_INT11         3
   #define TYPE_BIGINT20      8
   #define TYPE_DATETIME     12

   // #define MYSQL_TYPE_TEXT        252
   #define TYPE_LONGTEXT    252
   #define TYPE_VARCHAR255  253

   #define TYPE_SET         254


   //  Значения свойства _authorized структуры t_path
   #define AUTH_NO 0
   #define AUTH_YES 1
   #define AUTH_ANY 2


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