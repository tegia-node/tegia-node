#ifndef H_TEGIA_ACTOR_ADDR
#define H_TEGIA_ACTOR_ADDR

#include <iostream>
#include <atomic>
#include <tegia/core.h>


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// БАЗОВЫЙ КЛАСС АКТОРА
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


namespace tegia {
namespace actors {

//
// Шаблонный базовый класс
//

struct addr_t
{
    bool is_valid = true;
    std::string actor = "";
    std::string action = "";
};

//
//
//

} // namespace actors
} // namespace tegia


#endif

