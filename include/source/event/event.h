#ifndef H_EVENT
#define H_EVENT

#include <tegia/tegia.h>
#include <tegia/core/cast.h>
#include <tegia/source/dao/dao_event.h>


namespace SOURCE {


struct EVENT_TYPE
{
	long long int code;
	long long int source;
};

struct EVENT_EMITTER
{
	std::string uuid;
	long long int type;
	std::string init;
};


struct EVENT_COMMIT
{
	std::string actor;
	std::string action;
};


class EVENT_V3
{
	public:

		std::string uuid;
		int status;
		std::string   t_create;
		EVENT_TYPE type;
		EVENT_EMITTER emitter;
		EVENT_COMMIT commit;
		nlohmann::json data = nlohmann::json::object();
		nlohmann::json patch = nlohmann::json::object();

		//
		// Создание события из отдельных данных
		//

		EVENT_V3(
			int _status,
			const std::string &_t_create,
			long long int _type_code,
			long long int _type_source,
			const std::string &_emitter_uuid,
			long long int _emitter_type, 
			const std::string &_init_uuid,
			const std::string &_commit_actor,
			const std::string &_commit_action);

		//
		// Создание события из "полного" JSON
		//

		EVENT_V3(const nlohmann::json &event);
		EVENT_V3(const EVENT_V3 &ref_event);

		~EVENT_V3();

		nlohmann::json json();
		bool create(const std::string &_db,const std::string &_table);
		void make_commit(const int _status);
		bool save_status(int _status,const std::string &_db,const std::string &_table);
};






class [[deprecated]] EVENT
{
	public:
		EVENT(const std::string &db, const std::string &table): _db(db), _table(table)
		{
			this->_event = new SOURCE::DAO::EVENT(this->_table);
		};

		~EVENT()
		{
			delete this->_event;
		};

		void commit(const std::string actor, const std::string action)
		{
			this->_jevent["event"]["commit"]["actor"] = actor;
			this->_jevent["event"]["commit"]["action"] = action;
		};

		void data(nlohmann::json data)
		{
			this->_jevent["event"]["data"] = data;
		};

		int emit(bool save, bool emit)
		{
			long long int event_type = this->_jevent["event"]["data"]["emitter"]["type"].get<long long int>();

			std::string uuid = tegia::random::uuid();

			this->_jevent["event"]["uuid"] = uuid;
			this->_jevent["event"]["status"] = 10;

			//
			// SAVE
			//

			if(save == true)
			{
				this->_event->uuid( uuid );
				this->_event->data( this->_jevent["event"]["data"] );
				this->_event->status(10);
				this->_event->hash( tegia::crypt::MD5(this->_jevent["event"]["data"].dump()) );
				
				auto res = tegia::mysql::create(this->_db, this->_event);
				switch(res.code)
				{
					case 200:
					case DB_ERROR + ER_DUP_ENTRY:
					{
						// REM: ok
					}
					break;

					default:
					{
						std::cout << _ERR_TEXT_ << "[" << res.code << "] " << res.info << std::endl;
						std::cout << this->_jevent.dump() << std::endl;
						exit(0);
					}
					break;
				}
			}

			//
			// EMIT
			//

			if(emit == true)
			{
				switch(event_type)
				{
					case 200: break;
					default: tegia::message::send("models/events", "/emit", this->_jevent); break;
				}
				
			}

			return 0;
		};

	private:
		std::string _db;
		std::string _table;
		nlohmann::json _jevent;

		SOURCE::DAO::EVENT * _event = nullptr;

};	// END class EVENT

}	// END namespace SOURCE









[[deprecated]]
inline int emit_event(const std::string &db, const std::string &table, nlohmann::json &data, nlohmann::json &commit, bool emit = true)
{
	//
	// Сгенерировать тело события
	//

	nlohmann::json _event;
	std::string uuid = tegia::random::uuid();

	_event["event"]["uuid"] = uuid;
	_event["event"]["status"] = 10;
	_event["event"]["data"] = data;
	_event["event"]["commit"] = commit;

	//
	// Записать событие в "локальную" БД и Отправить событие в models
	//

	auto event = std::make_shared<SOURCE::DAO::EVENT>(table);
	event->uuid( uuid );
	event->data( _event["event"]["data"] );
	event->status(10);
	event->hash( tegia::crypt::MD5(data.dump()) );

	{
		auto res = tegia::mysql::create(db, event.get());
		switch(res.code)
		{
			case 200:
			case DB_ERROR + ER_DUP_ENTRY:
			{
				// REM: ok

				if(emit == true)
				{
					tegia::message::send("models/events", "/emit", _event);
				}
			}
			break;

			default:
			{
				std::cout << _ERR_TEXT_ << "[" << res.code << "] " << res.info << std::endl;
				std::cout << _event.dump() << std::endl;
				exit(0);
			}
			break;
		}
	}

	return 0;
};


#endif 

