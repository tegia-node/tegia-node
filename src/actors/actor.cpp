#include <tegia/actors/actor.h>
#include <tegia/tegia.h>
#include <tegia/core.h>


namespace tegia {
namespace actors {

actor_t::actor_t(
	const std::string &type, 
	const std::string &name):type(type),name(name)
{
	this->ws = tegia::threads::user()->ws();
};


} // namespace actors
} // namespace tegia
