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
	

unsigned long long int actor_t::roles(const std::string &uuid)
{
	return 0;
};


} // namespace actors
} // namespace tegia
