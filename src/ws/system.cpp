#include <tegia/ws/system.h>
#include <tegia/db/mysql/mysql.h>

//
//
//

namespace tegia {
namespace ws {


////////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
////////////////////////////////////////////////////////////////////////////////////////////////////


system_t::system_t(
	const std::string &connection, 
	const std::string &table, 
	const std::string &name)
	:connection(connection), table(table), name(name)
{

};


////////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
////////////////////////////////////////////////////////////////////////////////////////////////////


system_t::~system_t(){};


////////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
////////////////////////////////////////////////////////////////////////////////////////////////////


int system_t::init(nlohmann::json &data)
{
	if(data.is_array() == true)
	{
		nlohmann::json __data;
		__data["members"] = data;
		__data["title"] = this->wsid;
		__data["prefix"] = "";
		__data["wsid"] = "";
		__data["version"] = 1;
		data = __data;
	}

	std::cout << data << std::endl;

	//
	// PARSE DATA
	//

	this->title = data.value<std::string>("title","");
	this->prefix = data.value<std::string>("prefix",""); // data["prefix"].get<std::string>();
	this->wsid = data.value<std::string>("wsid",""); // data["wsid"].get<std::string>();
	this->version = data["version"].get<int>();

	for(auto it = data["members"].begin(); it != data["members"].end(); ++it)
	{
		member_t member;
		member.uuid = (*it)["uuid"].get<std::string>();
		member.roles = (*it)["roles"].get<unsigned long long int>();
		member.key = "";
		member.t_add = "";
		member.status = 0;

		if( it->contains("key") == true)
		{
			member.key = (*it)["key"].get<std::string>();
		}

		if( it->contains("t_add") == true)
		{
			member.t_add = (*it)["t_add"].get<std::string>();
		}

		if( it->contains("status") == true)
		{
			member.status = (*it)["status"].get<int>();
		}
		
		this->_members.insert({ member.uuid, member });
	}

	//
	//
	//

	return 0;
};


////////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
////////////////////////////////////////////////////////////////////////////////////////////////////


std::string system_t::dump()
{
	nlohmann::json _data = nlohmann::json::object();
	for(auto it = this->_members.begin(); it != this->_members.end(); ++it)
	{
		_data["members"].push_back( 
			{
				{ "uuid",   it->second.uuid  },
				{ "roles",  it->second.roles },
				{ "key",    it->second.key   },
				{ "t_add",  it->second.t_add },
				{ "status", it->second.status }
			});
	}

	_data["title"] = this->title;
	_data["version"] = this->version;
	_data["prefix"] = this->prefix;
	_data["wsid"] = this->wsid;

	return _data.dump();
};


////////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
////////////////////////////////////////////////////////////////////////////////////////////////////


int system_t::commit()
{
	std::string query = std::format(
		"UPDATE `{}` SET `members` = '{}' WHERE `name` = '{}';",
		this->table,
		tegia::mysql::strip(this->dump()),
		this->name
	);

	auto res = tegia::mysql::query(this->connection,query);

	// TODO: ERROR
	
	delete res;

	return 0;
};


////////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
////////////////////////////////////////////////////////////////////////////////////////////////////


}  // END namespace ws
}  // END namespace tegia



