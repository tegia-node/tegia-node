#ifndef H_TEGIA_PLATFORM_ACTOR_BASE
#define H_TEGIA_PLATFORM_ACTOR_BASE

#include <tegia/tegia.h>
#include <atomic>

#define _action_func_bind2_(_function_) std::bind(_function_, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
#define ACTION_ROUTE2(_name_, _function_) ro->add( _name_, _action_func_bind2_(_function_) )

#define _INIT_ 

namespace tegia {
namespace actors {

class actor_base
{
	protected:
		std::string type;
		std::string configuration;
		std::string name; 
		nlohmann::json init_data;

	public:

		std::atomic<int> countMessage{0};

		actor_base(
			const std::string &type,
			const std::string &name,
			nlohmann::json &init_data): type(type),name(name),init_data(init_data)
		{		};

		virtual ~actor_base(){ };

		std::string get_type()
		{
			return this->type;
		};

		std::string get_name()
		{
			return this->name;
		};

		int get_count_message()
		{
			return this->countMessage.load();
		};
};

} // namespace actors
} // namespace tegia

#endif