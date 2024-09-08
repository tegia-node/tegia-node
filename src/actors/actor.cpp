#include <tegia/tegia.h>
#include <tegia/actors/actor.h>
#include <tegia/context/context.h>

namespace tegia {
namespace actors {

actor_t::actor_t(
	const std::string &type, 
	const std::string &name):type(type),name(name)
{
	this->ws = tegia::context::user()->ws();
};


} // namespace actors
} // namespace tegia
