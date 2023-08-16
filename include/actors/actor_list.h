#ifndef H_TEGIA_PLATFORM_ACTORS_LIST
#define H_TEGIA_PLATFORM_ACTORS_LIST

#include <shared_mutex>

#include <nlohmann/json-schema.hpp>

#include <tegia/const.h>
#include <tegia/core/core.h>

#include <tegia/node/logger.h>
#include <tegia/node/config.h>

#include <tegia/actors/message_t.h>
#include <tegia/actors/routers_map.h>
#include <tegia/actors/actor_base.h>

//#define DEVELOPER_VERSION
#if defined(DEVELOPER_VERSION)
#define DOUT(text) std::cout << text << std::endl;     
#else
#define DOUT(text)     
#endif

#define EOUT(text) DOUT(_ERR_TEXT_ << __func__  << ": " << text)
#define NOUT(text) DOUT(_OK_TEXT_ << __func__  << ": " << text)

struct actor_desc_t
{
	tegia::actors::actor_base *actor;
	std::shared_ptr<tegia::actors::router_base> router;
	std::string node_uuid{};
	bool is_local = true;
	bool share = false;
};

namespace tegia {
namespace actors {

using actors_map  = std::unordered_map <std::string, actor_desc_t, 
				    tegia::common::case_insensitive_unordered_map::hash, tegia::common::case_insensitive_unordered_map::equal_to>;
using routers_map = std::unordered_map <std::string, std::shared_ptr<tegia::actors::router_base>, 
					tegia::common::case_insensitive_unordered_map::hash, tegia::common::case_insensitive_unordered_map::equal_to>;				   

class list final
{
	private:
		actors_map  _actors{};
		routers_map _routers{};

		std::mutex actor_list_mutex{};
		mutable std::shared_mutex actor_mutex{};

		void load_configuration(
			const std::string &name, 
			const std::string &file);

	public:
		list() = default;
		~list();

		list(list const&) = delete;
		list(list&&) noexcept = delete;
		list& operator = (list const&) = delete;
		list& operator = (list&&) noexcept = delete;

		void init();

		void load_configurations();

		bool load_actor_type(
			const std::string &path, 
			const std::string &actor_type);

		tegia::actors::actor_base * create_actor(
			const std::string &actor_type,
			const std::string &actor_name, 
			bool share,
			nlohmann::json &data);

		tegia::actors::actor_base * get_actor(const std::string &actor_name);

		bool delete_actor(const std::string &actor_name);

		int send_message(
			const std::string &actor_name, 
			const std::string &action_name, 
			const std::shared_ptr<message_t> &message,
			int priority = _PLOW_);

		int send_message(
			const std::string &actor_name, 
			const std::string &action_name, 
			nlohmann::json data,
			int priority = _PLOW_);

		std::string find_pattern(
			const std::string &actor_name, 
			const std::string &action_name);

		nlohmann::json shared_local_actors() const noexcept;

		void add_client_node_actors(std::string node_uuid, nlohmann::json node_actor_list) noexcept;
		void remove_client_node_actors(nlohmann::json node_actor_list) noexcept;
		
};

}	// namespace actors
}  // namespace tegia


#endif