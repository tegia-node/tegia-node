#ifndef _TEGIA_THREAD_CONTEXT_H_
#define _TEGIA_THREAD_CONTEXT_H_
// --------------------------------------------------------------------

#include <thread>

#include <tegia/core/core.h>
#include <tegia/core/cast.h>
#include <tegia2/core/json.h>


////////////////////////////////////////////////////////////////////////////
//
// USER
//
////////////////////////////////////////////////////////////////////////////


namespace ROLES {

const int _ADMIN_ = 63;
const int _USER_ = 62;

} // END namespace ROLES





namespace tegia {
namespace threads {

class pool;
class worker;
class data;


////////////////////////////////////////////////////////////////////////////
//
// CONTEXT
//
////////////////////////////////////////////////////////////////////////////


class context
{
	private:

	public:

		context(){};
		~context(){};
};

typedef std::shared_ptr<tegia::threads::context> context_ptr;


////////////////////////////////////////////////////////////////////////////
//
// DATA
//
////////////////////////////////////////////////////////////////////////////


class data
{
	private:
		context_ptr context;

	public:

		data()
		{  
			this->context = std::make_shared<tegia::threads::context>();
			this->tid = core::cast<std::string>(std::this_thread::get_id());
		};

		~data()
		{  };

		
		bool init_user(
			int status,
			const std::string &uuid,	// uuid пользователя
			const std::string &name,
			const std::string &surname,
			const std::string &patronymic,
			int sex,
			const std::string &photo,
			unsigned long long int roles
		)
		{
			this->context->jwt.status = status;
			this->context->jwt.uuid = uuid;
			this->context->jwt.name = name;
			this->context->jwt.surname = surname;
			this->context->jwt.patronymic = patronymic;
			this->context->jwt.sex = sex;
			this->context->jwt.photo = photo;
			this->context->jwt._roles = std::bitset<64>(roles);
			return true;
		};

		std::string get_user_uuid()
		{
			return this->context->jwt.uuid;
		};
		

		std::string get_jwt()
		{
			return this->context->jwt.token;
		};

		nlohmann::json json()
		{
			nlohmann::json tmp;
			tmp["tid"] = this->tid;
			tmp["jwt"] = this->context->jwt.json();
			return tmp;
		};

		bool is_role(const int _role)
		{
			return this->context->jwt._roles[_role];
		};

		int count = 0;
		std::string tid = "";

}; // class data

}  // namespace threads
}  // namespace tegia


// --------------------------------------------------------------------
#endif