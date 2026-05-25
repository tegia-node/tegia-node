#ifndef H_TEGIA_ACTORS_WORKER
#define H_TEGIA_ACTORS_WORKER

#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <functional>
#include <array>

#include <tegia/tegia.h>
#include <tegia/core/http.h>
#include <tegia/db/mysql/mysql.h>

#include <tegia/actors/worker/task.h>


namespace tegia {
namespace actors {


class worker_t : public actor_t
{
	template <typename actor_type, typename Enable> friend class type_t;

	protected:
		worker_t(
			const std::string &type,
			const std::string &name,
			tegia::mysql::addr_t db)
			: db(db), actor_t(type, name)
		{
			this->status = 200;
		}

	protected:
		tegia::worker::task_t task;
		tegia::actors::addr_t manager;
		tegia::mysql::addr_t db;

		template<typename TYPE>
		int run(const std::shared_ptr<message_t> &message)
		{
			this->task.init(message->data);
			return static_cast<TYPE *>(this)->run(message);
		}

		template<typename TYPE>
		int init(const std::shared_ptr<message_t> &message)
		{
			if (message == nullptr || !message->data.is_object()) {
				return 400;
			}

			if (message->data.contains("manager") && message->data["manager"].is_string()) {
				this->manager.actor = message->data["manager"].get<std::string>();
			} else if (message->data.contains("manager") && message->data["manager"].is_object() &&
				message->data["manager"].contains("actor") && message->data["manager"]["actor"].is_string()) {
				this->manager.actor = message->data["manager"]["actor"].get<std::string>();
			} else {
				return 400;
			}

			if (message->data.contains("init") && message->data["init"].is_string()) {
				this->manager.action = message->data["init"].get<std::string>();
			} else if (message->data.contains("manager") && message->data["manager"].is_object() &&
				message->data["manager"].contains("init_action") && message->data["manager"]["init_action"].is_string()) {
				this->manager.action = message->data["manager"]["init_action"].get<std::string>();
			} else if (message->data.contains("manager") && message->data["manager"].is_object() &&
				message->data["manager"].contains("action") && message->data["manager"]["action"].is_string()) {
				this->manager.action = message->data["manager"]["action"].get<std::string>();
			} else {
				return 400;
			}

			int _status = static_cast<TYPE *>(this)->init(message);
			(void)_status;

			tegia::message::send(
				this->manager.actor,
				this->manager.action,
				{
					{ "worker", this->name },
					{ "task", "" },
					{ "status", 100 }
				}
			);

			return 200;
		}

	private:

};




template<typename actor_type>
class type_t<actor_type, std::enable_if_t<std::is_base_of_v<tegia::actors::worker_t, actor_type>>> : public type_base_t
{
	protected:

	public:
		type_t(const std::string &type): type_base_t(type)
		{
			this->add_action(
				"/run",
				"",
				static_cast<tegia::actors::action_fn_ptr>(&tegia::actors::worker_t::template run<actor_type>),
				tegia::user::roles(ROLES::SESSION::SYSTEM, ROLES::SESSION::USER)
			);

			this->add_action(
				"/init",
				"",
				static_cast<tegia::actors::action_fn_ptr>(&tegia::actors::worker_t::template init<actor_type>),
				tegia::user::roles(ROLES::SESSION::SYSTEM)
			);
		};

		actor_t * create_actor(const std::string &name) override
		{
			return new actor_type(name);
		};
};


} // END namespace actors
} // END namespace tegia


#endif
