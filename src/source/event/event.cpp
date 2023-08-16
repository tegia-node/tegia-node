#include <tegia/source/event/event.h>


namespace SOURCE {


EVENT_V3::EVENT_V3(
	int _status,
	const std::string &_t_create,
	long long int _type_code,
	long long int _type_source,
	const std::string &_emitter_uuid,
	long long int _emitter_type, 
	const std::string &_init_uuid,
	const std::string &_commit_actor,
	const std::string &_commit_action
):	status(_status),
	t_create(_t_create),
	type({_type_code,_type_source}),
	emitter({_emitter_uuid,_emitter_type,_init_uuid}),
	commit({_commit_actor,_commit_action})
{
	this->uuid = tegia::random::uuid();
};


EVENT_V3::EVENT_V3(const nlohmann::json &event)
{
	//
	// TODO: валидировать
	//

	this->uuid = event["event"]["uuid"].get<std::string>();
	this->status = event["event"]["status"].get<int>();

	this->t_create = event["event"]["data"]["t_create"].get<std::string>();

	this->type.code = event["event"]["data"]["type"]["code"].get<long long int>();
	this->type.source = event["event"]["data"]["type"]["source"].get<long long int>();

	this->emitter.uuid = event["event"]["data"]["emitter"]["uuid"].get<std::string>();
	this->emitter.type = event["event"]["data"]["emitter"]["type"].get<long long int>();
	this->emitter.init = event["event"]["data"]["emitter"]["init_uuid"].get<std::string>();

	this->commit.actor = event["event"]["commit"]["actor"].get<std::string>();
	this->commit.action = event["event"]["commit"]["action"].get<std::string>();

	this->data = event["event"]["data"]["data"];
	this->patch = event["event"]["data"]["patch"];
};


EVENT_V3::EVENT_V3(const EVENT_V3 &ref_event)
{
	//
	//
	//
	
	this->status = ref_event.status;
	this->t_create = ref_event.t_create;
	this->type = ref_event.type;
	this->emitter = ref_event.emitter;
	this->commit = ref_event.commit;
	this->data = ref_event.data;
	this->patch = ref_event.patch;

	/*
		При копировании события генерируем новый uuid события и устанавливаем emitter.init
	*/

	this->uuid = tegia::random::uuid();
	this->emitter.init = ref_event.uuid;
};


EVENT_V3::~EVENT_V3(){};


nlohmann::json EVENT_V3::json()
{
	nlohmann::json event;

	event["event"]["uuid"] = this->uuid;
	event["event"]["status"] = this->status;

	event["event"]["data"]["schema"] = "v3";
	event["event"]["data"]["t_create"] = this->t_create;

	event["event"]["data"]["type"]["code"] = this->type.code;
	event["event"]["data"]["type"]["source"] = this->type.source;

	event["event"]["data"]["emitter"]["uuid"] = this->emitter.uuid;
	event["event"]["data"]["emitter"]["type"] = this->emitter.type;
	event["event"]["data"]["emitter"]["init_uuid"] = this->emitter.init;

	event["event"]["commit"]["actor"] = this->commit.actor;
	event["event"]["commit"]["action"] = this->commit.action;

	event["event"]["data"]["data"] = this->data;
	event["event"]["data"]["patch"] = this->patch;

	return std::move(event);
};



bool EVENT_V3::create(const std::string &_db,const std::string &_table)
{
	auto event = std::make_shared<SOURCE::DAO::EVENT>(_table);

	auto data = this->json();

	event->uuid( this->uuid );
	event->data( data["event"]["data"] );
	event->status(10);
	event->hash( tegia::crypt::MD5(data["event"]["data"].dump()) );
		
	auto res = tegia::mysql::create(_db, event.get());
	switch(res.code)
	{
		case 200:
		case DB_ERROR + ER_DUP_ENTRY:
		{
			return true;
		}
		break;

		default:
		{
			std::cout << _ERR_TEXT_ << "[" << res.code << "] " << res.info << std::endl;
			std::cout << data.dump() << std::endl;
			exit(0);
		}
		break;
	}

	return false;
};



void EVENT_V3::make_commit(const int _status)
{
	this->status = _status;

	nlohmann::json msg_data = nlohmann::json::object();
	msg_data["event"]["status"] = _status;
	msg_data["event"]["uuid"] = this->uuid;

	tegia::message::send(this->commit.actor,this->commit.action, msg_data);
};



bool EVENT_V3::save_status(int _status,const std::string &_db,const std::string &_table)
{
	std::string query = "UPDATE `" + _table + "` SET `status` = " + core::cast<std::string>(_status) + " WHERE `uuid` = '" + this->uuid + "';";
	auto res = tegia::mysql::query(_db,query);
	delete res;

	// TODO: CHECK RESULT

	return true;
};


}	// END namespace SOURCE
