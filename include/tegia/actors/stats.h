#ifndef H_TEGIA_ACTORS_STATS
#define H_TEGIA_ACTORS_STATS

#include <cstddef>

namespace tegia {
namespace actors {

struct mailbox_stats_t
{
	bool found = false;
	bool has_mailbox = false;
	std::size_t active = 0;
	std::size_t queued = 0;
	std::size_t running = 0;
	std::size_t max_inflight = 0;
	std::size_t max_queue_size = 0;
};

} // namespace actors
} // namespace tegia

#endif
