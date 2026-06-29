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
			const std::string &name)
			: actor_t(type, name)
		{
			this->status = 200;
		}

		protected:
			tegia::worker::task_t task;
			tegia::actors::addr_t init_addr;
			tegia::actors::addr_t commit_addr;
			std::mutex run_mutex;

		static constexpr int INIT_READY_DEFERRED = 102;

		int notify_ready(int status = 100)
		{
			return tegia::message::send(
				this->init_addr.actor,
				this->init_addr.action,
				{
					{ "worker", this->name },
					{ "task", "" },
					{ "status", status }
				}
			);
		}

			template<typename TYPE>
			int run(const std::shared_ptr<message_t> &message)
			{
				std::lock_guard<std::mutex> guard(this->run_mutex);
				this->task.init(message->data);
				return static_cast<TYPE *>(this)->run(message);
			}

			template<typename TYPE>
			int init(const std::shared_ptr<message_t> &message)
			{
				std::string manager = "";
				if (message->data.contains("manager") && message->data["manager"].is_string())
				{
					manager = message->data["manager"].get<std::string>();
				}
				else if (
					message->data.contains("manager") &&
					message->data["manager"].is_object() &&
					message->data["manager"].contains("actor") &&
					message->data["manager"]["actor"].is_string()
				)
				{
					manager = message->data["manager"]["actor"].get<std::string>();
				}

				if (message->data.contains("init") && message->data["init"].is_string())
				{
					this->init_addr.actor = manager;
					this->init_addr.action = message->data["init"].get<std::string>();
				}
				else if (
					message->data.contains("manager") &&
					message->data["manager"].is_object() &&
					message->data["manager"].contains("action") &&
					message->data["manager"]["action"].is_string()
				)
				{
					this->init_addr.actor = manager;
					this->init_addr.action = message->data["manager"]["action"].get<std::string>();
				}

				if (message->data.contains("commit") && message->data["commit"].is_string())
				{
					this->commit_addr.actor = manager;
					this->commit_addr.action = message->data["commit"].get<std::string>();
				}
				else if(this->init_addr.actor.empty() == false)
				{
					this->commit_addr.actor = this->init_addr.actor;
					this->commit_addr.action = "/workers/commit";
				}

			int _status = static_cast<TYPE *>(this)->init(message);

			if(_status != INIT_READY_DEFERRED)
			{
				this->notify_ready();
			}

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
