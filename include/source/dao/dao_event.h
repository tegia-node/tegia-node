#ifndef H_SOURCE_DAO_EVENT
#define H_SOURCE_DAO_EVENT

#include <tegia/dao/entity.h>

////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// DAO CLASS                                                                              //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

namespace SOURCE {
namespace DAO { 

class EVENT: public dao::Entity
{     
	protected:
		
		PROPDEFF2(std::string, uuid, "", nullptr, nullptr)
		PROPDEFF2(nlohmann::json, data, "null", nullptr, nullptr)
		PROPDEFF2(int, status, 0, nullptr, nullptr)
		PROPDEFF2(std::string, hash, "", nullptr, nullptr)

	public:   

		EVENT(const std::string &table_name = "events_source") : dao::Entity(table_name)
		{
			this->access->add("uuid", &SOURCE::DAO::EVENT::uuid, this);
			this->access->add("data", &SOURCE::DAO::EVENT::data, this);
			this->access->add("status", &SOURCE::DAO::EVENT::status, this);
			this->access->add("hash", &SOURCE::DAO::EVENT::hash, this);
		};

		virtual ~EVENT() {    };

		std::string get_sql_values() { return "no realization"; }

};	// END class EVENT


}   // END namespace DAO
}	// END namespace SOURCE

#endif 

