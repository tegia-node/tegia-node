
#include <tegia2/tegia.h>
#include "node/node.h"

namespace tegia {
namespace actors {




} // namespace actors
} // namespace tegia



namespace tegia {
namespace message {

int send(const std::string &name, const std::string &action, nlohmann::json data)
{
	return tegia::node::node::instance()->send_message(name,action,std::move(data));
};


} // namespace message
} // namespace tegia


