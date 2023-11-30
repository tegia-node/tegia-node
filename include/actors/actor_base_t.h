#ifndef H_TEGIA_ACTORS_BASE
#define H_TEGIA_ACTORS_BASE

#include <tegia2/core/json.h>
#include <atomic>


#define _action_func_bind_(_function_) std::bind(_function_, std::placeholders::_1, std::placeholders::_2)
#define ADD_ACTION(_name_, _function_) actor_type->add_action( _name_, _action_func_bind_(_function_) )


namespace tegia {
namespace actors {


class actor_base
{
	protected:
		std::string type;
		std::string name; 

	public:

		actor_base(
			const std::string &_type,
			const std::string &_name,
			nlohmann::json init_data): type(_type),name(_name)
		{	
			std::cout << _OK_TEXT_ << "CREATE ACTOR" << std::endl;
			std::cout << "      type   = " << _type << std::endl;
			std::cout << "      name   = " << _name << std::endl;
		};


		virtual ~actor_base(){ };

		actor_base(actor_base const&) = delete;
		actor_base(actor_base&&) noexcept = delete;
		actor_base& operator = (actor_base const&) = delete;
		actor_base& operator = (actor_base&&) noexcept = delete;

		std::string get_type()
		{
			return this->type;
		};


		std::string get_name()
		{
			return this->name;
		};
};


} // namespace actors
} // namespace tegia

#endif