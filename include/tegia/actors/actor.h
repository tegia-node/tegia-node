#ifndef H_TEGIA_ACTOR_BASE
#define H_TEGIA_ACTOR_BASE

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

class map_t;
template <typename actor_type, typename Enable> class type_t;

//
// Шаблонный базовый класс
//

class actor_t
{
	friend class map_t;
	template <typename actor_type, typename Enable> friend class type_t;

	protected:
		std::string name = "";
		std::string type = "";
		std::string ws = "";
		int status = 200;

		std::atomic<int> messages{0};

		actor_t(const std::string &type,const std::string &name);
		virtual ~actor_t() = default;

	public:
		actor_t(actor_t const&) = delete;
		actor_t(actor_t &&) noexcept = delete;
		actor_t& operator = (actor_t const&) = delete;
		actor_t& operator = (actor_t &&) noexcept = delete;
};

//
//
//

using action_fn_ptr = int (actor_t::*)(const std::shared_ptr<message_t> &);

//
//
//

} // namespace actors
} // namespace tegia


#endif

