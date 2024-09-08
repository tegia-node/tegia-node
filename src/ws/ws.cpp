#include <tegia/tegia.h>
#include <tegia/core/const.h>
#include <tegia/ws/ws.h>
#include <tegia/context/context.h>

namespace tegia {
namespace actors {


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


ws_t::ws_t(
	const std::string &type, 
	const std::string &name,
	const std::string &connection,
	const std::string &table,
	unsigned long long int creators)
:actor_t(type,name),connection(connection),table(table),creators(creators)
{
	std::cout << "[RUN] create tegia::actors::ws_t " << name << std::endl;
	this->_router = new tegia::app::router_t(this->name);

	auto query = "SELECT * FROM `" + table + "` WHERE `name` = '" + name + "'";
	auto res = tegia::mysql::query(connection,query);

	//
	// ERROR
	//

	if(res->code != 200)
	{
		std::cout << _ERR_TEXT_ << "[" << res->code << "] LOAD WS " << this->name << std::endl;
		delete res;
		this->status = 500;
		return;
	}

	//
	// NOT FOUND
	//

	if(res->count() == 0)
	{
		delete res;
		this->status = 404;

		std::cout << _ERR_TEXT_ << "[404] LOAD WS " << this->name << " NOT FOUND" << std::endl;
		std::cout << "status = " << this->status << std::endl;
		std::cout << "type   = " << this->type << std::endl;
		std::cout << "name   = " << this->name << std::endl;

		if(this->type == "USER::PERSONAL")
		{
			auto pos = this->name.find(tegia::context::user()->uuid());
			if(pos != std::string::npos)
			{
				this->status = this->_create();
				return;
			}
		}
		else
		{
			this->status = this->_create();
			return;
		}

		return;
	}

	//
	// DATA
	//

	this->data = nlohmann::json::parse(res->get(table + "::data",0));

	//
	// MEMBERS
	//

	auto _members = nlohmann::json::parse(res->get(table + "::members",0));
	for(auto it = _members.begin(); it != _members.end(); ++it)
	{
		this->members.insert({ (*it)["uuid"].get<std::string>(),(*it)["roles"].get<long long unsigned int>() });
		std::cout << it->dump() << std::endl;
	}

	this->ws = this->name; // res->get(table + "::wsid",0);
	this->status = 200;

	delete res;

	std::cout << "[END] create tegia::actors::ws_t " << name << std::endl;
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


ws_t::~ws_t()
{
	if(this->_router != nullptr)
	{
		delete this->_router;
	}
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


int ws_t::commit()
{
	std::string query = "UPDATE `" + this->table + "` SET `data` = '" + tegia::mysql::strip(this->data.dump()) + "' WHERE `name` = '" + this->name + "';";
	auto res = tegia::mysql::query(this->connection,query);

	// TODO

	delete res;
	return 200;
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


int ws_t::_create()
{
	std::cout << "[RUN] CREATE WS" << std::endl;

	auto user = tegia::context::user();
	auto match = user->_roles.to_ullong() & this->creators;

	std::cout << "user->_roles   = " << user->_roles.to_ullong() << std::endl;
	std::cout << "this->creators = " << this->creators << std::endl;
	std::cout << "match          = " << match << std::endl;

	if(match == 0)
	{
		std::cout << _ERR_TEXT_ << "You do not have access rights" << std::endl;
		return 403;
	}

	this->ws = this->name;

	nlohmann::json members;
	nlohmann::json member;

	std::string uuid = user->uuid();
	unsigned long long int roles = tegia::user::roles(ROLES::WS::OWNER,ROLES::WS::ADMIN,ROLES::WS::MEMBER);

	member["uuid"] = uuid;
	member["roles"] = roles;
	members.push_back(member);
	this->members.insert({ uuid, roles });

	//
	//
	//

	{
		std::string query = "INSERT INTO `" + this->table + "` (`name`,`data`,`members`) VALUES ("
			"'" + this->name + "',"
			"'{}',"
			"'" + tegia::mysql::strip(members.dump()) + "');";

		auto res = tegia::mysql::query(this->connection,query);
		delete res;
	}

	std::cout << "[END] CREATE WS" << std::endl;
	return 100;
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


int ws_t::router(const std::shared_ptr<message_t> &message)
{
	std::cout << _YELLOW_ << "ws_t::router" << _BASE_TEXT_ << std::endl;
	std::cout << "tid = " << tegia::context::tid() << std::endl;
	tegia::context::user()->print();

	//
	// SET USER ROLES
	//

	long long int role = 0;
	auto user = tegia::context::user();
	std::string uuid = user->uuid();

	std::cout << "roles old = " << user->_roles.to_ullong() << std::endl;

	auto pos = this->members.find(uuid);
	if(pos != this->members.end())
	{
		user->_roles = std::bitset<64>{user->_roles.to_ullong() | pos->second};
	}

	std::cout << "roles new = " << user->_roles.to_ullong() << std::endl;

	//
	// 
	//

	user->_ws = this->name;	
	tegia::context::user()->print();

	// std::cout << _OK_TEXT_ << "USER '" << uuid << "' IS MEMBER WS" << std::endl;

	//
	// INIT CONTEXT
	//

	// [ TODO ]

	//
	// RESOLVE URL
	//

	std::string method = message->http["request"]["request_method"].get<std::string>();
	std::string action = message->http["action"].get<std::string>();

	auto [res,params] = _router->match(
		10,
		method,
		action,
		&message->http["request"]["post"]);

	switch(res)
	{
		case 200:
		{
			message->http["response"]["status"] = 200;
			message->http["response"]["type"] = "application/json";

			message->data = params["data"];
			message->callback.add(
				params["actor"].get<std::string>(),
				params["action"].get<std::string>()
			);
		}
		break;

		case 403:
		{
			std::cout << _ERR_TEXT_ << "[403] " << method << " " << action << std::endl;

			message->http["response"]["status"] = 403;
			message->http["response"]["type"] = "application/json";
			return 403;
		}
		break;

		case 404:
		{
			std::cout << _ERR_TEXT_ << "not found " << method << " " << action << std::endl; 

			message->http["response"]["status"] = 404;
			message->http["response"]["type"] = "application/json";

			return 404;
		}
		break;
	}

	// std::cout << "res    = " << res << std::endl;
	// std::cout << "params = " << params << std::endl;

	// tegia::message::send
	
	return 200;
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


int ws_t::member_add(const std::shared_ptr<message_t> &message)
{
	std::cout << _YELLOW_ << "ws_t::member_add" << _BASE_TEXT_ << std::endl;
	return 200;
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


int ws_t::member_remove(const std::shared_ptr<message_t> &message)
{
	std::cout << _YELLOW_ << "ws_t::member_remove" << _BASE_TEXT_ << std::endl;
	return 200;
};



}	// END namespace actors
}	// END namespace tegia
