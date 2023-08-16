//
// Макросы для вывода отладочной информации на экран
//

#if _DEBUG_PRINT_MODE_ == false
#define _DEBUG_PRINT_(text)
#endif

#if _DEBUG_PRINT_MODE_ == true
#define _DEBUG_PRINT_(text) std::cout << text << std::endl;
#endif
