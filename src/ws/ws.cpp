#include <tegia/tegia.h>
#include <tegia/core/const.h>
#include <tegia/ws/ws.h>
#include <tegia/core/time.h>

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
: actor_t(type,name),
connection(connection),
table(table),
creators(creators)
{
	std::cout << "[RUN] create tegia::actors::ws_t " << name << std::endl;

	this->_router = new tegia::app::router_t(this->name);
	this->_system = new tegia::ws::system_t(connection,table,name);


	auto query = "SELECT * FROM `" + table + "` WHERE `name` = '" + name + "'";
	auto res = tegia::mysql::query(connection,query);

	//
	// ERROR
	//

	if(res->code != 200)
	{
		std::cout << _ERR_TEXT_ << "[" << res->code << "] LOAD WS " << this->name << std::endl;
		std::cout << "connection = " << connection << std::endl;
		std::cout << "query      = " << query << std::endl;
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
			auto pos = this->name.find(tegia::threads::user()->uuid());
			if(pos != std::string::npos)
			{
				this->status = this->_create();
				return;
			}
		}
		else
		{
			auto user = tegia::threads::user();
			auto match = user->_roles.to_ullong() & this->creators;

			std::cout << "user->_roles   = " << user->_roles.to_ullong() << std::endl;
			std::cout << "this->creators = " << this->creators << std::endl;
			std::cout << "match          = " << match << std::endl;

			if(match == 0)
			{
				std::cout << _ERR_TEXT_ << "You do not have access rights" << std::endl;
				this->status = 403;
				return;
			}

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
	// SYSTEM
	//

	auto __system = nlohmann::json::parse(res->get(table + "::members",0));
	this->_system->init(__system);
	this->_system->commit();
	this->ws = this->name;
	this->status = 200;

	delete res;

	//
	// ADD ROUTERS
	//

	this->_router->add("POST", "/member/add",
	nlohmann::json::parse(R"({
		"actor": "{/this}",
		"action":"/member/add",
		"mapping": 
		{
			"": "/post/data"
		}
	})"));
	

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
		delete this->_system;
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


unsigned long long int ws_t::roles(const std::string &uuid)
{
	std::cout << _YELLOW_ << "RUN ws_t::roles()" << _BASE_TEXT_ << std::endl;
	
	auto pos = this->_system->_members.find(uuid);
	if(pos != this->_system->_members.end())
	{
		return pos->second.roles;
	}

	return 0;
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


int ws_t::_create()
{
	std::cout << "[RUN] CREATE WS" << std::endl;

	this->ws = this->name;
	auto user = tegia::threads::user();
	std::string uuid = user->uuid();
	unsigned long long int roles = tegia::user::roles(ROLES::WS::OWNER,ROLES::WS::ADMIN,ROLES::WS::MEMBER);

	tegia::ws::member_t _member;
	_member.uuid = uuid;
	_member.roles = roles;
	_member.key = "";
	_member.t_add = core::time::current();
	_member.status = 200;
	this->_system->_members.insert({_member.uuid,_member});

	//
	//
	//

	{
		std::string query = "INSERT INTO `" + this->table + "` (`name`,`data`,`members`) VALUES ("
			"'" + this->name + "',"
			"'{}',"
			"'" + tegia::mysql::strip(this->_system->dump()) + "');";

		auto res = tegia::mysql::query(this->connection,query);
		delete res;
	}

	//
	// ADD ROUTERS
	//

	this->_router->add("POST", "/member/add",
	nlohmann::json::parse(R"({
		"actor": "{/this}",
		"action":"/member/add",
		"mapping": 
		{
			"": "/post/data"
		}
	})"));	


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
	std::cout << "tid = " << tegia::threads::tid() << std::endl;
	tegia::threads::user()->print();

	//
	// SET USER ROLES
	//

	unsigned long long int role = 0;
	auto user = tegia::threads::user();
	std::string uuid = user->uuid();

	std::cout << "roles old = " << user->_roles.to_ullong() << std::endl;

	auto pos = this->_system->_members.find(uuid);
	if(pos != this->_system->_members.end())
	{
		user->_roles = std::bitset<64>{user->_roles.to_ullong() | pos->second.roles};
	}

	std::cout << "roles new = " << user->_roles.to_ullong() << std::endl;

	//
	// 
	//

	user->_ws = this->name;	
	tegia::threads::user()->print();

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
			std::cout <<_YELLOW_ << "params" << _BASE_TEXT_ << std::endl;
			std::cout << params << std::endl;
			std::cout <<_YELLOW_ << "_router->print()" << _BASE_TEXT_ << std::endl;
			_router->print();

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


int ws_t::init(const std::shared_ptr<message_t> &message)
{
	std::cout << _YELLOW_ << "ws_t::init" << _BASE_TEXT_ << std::endl;

	std::cout << _YELLOW_ << std::endl;
	std::cout << "==============================================================" << std::endl;
	std::cout << message->data << std::endl;
	std::cout << "==============================================================" << std::endl;
	std::cout << _BASE_TEXT_ << std::endl;


	this->_system->title = message->data["title"].get<std::string>();
	this->_system->prefix = message->data["prefix"].get<std::string>();
	this->_system->wsid = message->data["wsid"].get<std::string>();

	this->_system->commit();

	return 200;
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*

	{
		"user": "<uuid>"
	}

*/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


int ws_t::member_add(const std::shared_ptr<message_t> &message)
{
	std::cout << _YELLOW_ << "ws_t::member_add" << _BASE_TEXT_ << std::endl;

	//
	// Добавляем участника с ролью "приглашенный"
	//

	this->_system->_mutex.lock();

	tegia::ws::member_t _member;
	_member.uuid = message->data["user"].get<std::string>();
	_member.roles = tegia::user::roles(ROLES::WS::CANDIDAT);
	_member.key = tegia::random::string(15);
	_member.t_add = core::time::current();
	
	this->_system->_members.insert({_member.uuid,_member});
	this->_system->commit();

	this->_system->_mutex.unlock();

	//
	//
	//
	
	auto msg = tegia::message::init();
	msg->data["key"] = _member.key;
	msg->data["title"] = this->_system->title;
	msg->data["prefix"] = this->_system->prefix;
	msg->data["wsid"] = this->_system->wsid;

	tegia::message::send("user/" + _member.uuid, "/ws/add", msg);

	return 200;
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	{
		"user": "<uuid>",
		"key": "<string>"
	}

*/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


int ws_t::member_accept(const std::shared_ptr<message_t> &message)
{
	std::cout << _YELLOW_ << "ws_t::member_accept" << _BASE_TEXT_ << std::endl;
	std::cout << message->data << std::endl;

	std::string user = message->data["user"].get<std::string>();
	std::string key  = message->data["key"].get<std::string>();

	this->_system->_mutex.lock();

	auto pos = this->_system->_members.find(user);
	if(pos == this->_system->_members.end())
	{
		return 404;
	}

	if(key == pos->second.key)
	{
		this->_system->_members[user].roles = tegia::user::roles(ROLES::WS::MEMBER);
		this->_system->_members[user].key = "";
		this->_system->commit();
		this->_system->_mutex.unlock();
		return 200;
	}

	this->_system->_mutex.unlock();
	return 400;
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
