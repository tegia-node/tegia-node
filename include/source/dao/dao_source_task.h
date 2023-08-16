#ifndef H_DAO_SOURCE_TASK
#define H_DAO_SOURCE_TASK

////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// INCLUDES                                                                               //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

#include <tegia/dao/entity.h>


////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// DAO CLASS                                                                              //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

namespace SOURCE
{
	class TASK;

}	// END namespace SOURCE


namespace DAO { 
namespace SOURCE {

class TASK: public dao::Entity
{
	friend class ::SOURCE::TASK;

	protected:

		PROPDEFF2(long int, _id_source, 0, nullptr, nullptr)
		PROPDEFF2(std::string, uuid, "", nullptr, nullptr)
		PROPDEFF2(std::string, t_start, "", nullptr, nullptr)
		PROPDEFF2(std::string, t_end, "2030-01-01 00:00:00", nullptr, nullptr)
		PROPDEFF2(int, status, 0, nullptr, nullptr)
		PROPDEFF2(nlohmann::json, data, "{}", nullptr, nullptr)

	public:   

		TASK() : dao::Entity("source_tasks")
		{
			this->access->add("_id_source", &DAO::SOURCE::TASK::_id_source, this);
			this->access->add("uuid", &DAO::SOURCE::TASK::uuid, this);
			this->access->add("t_start", &DAO::SOURCE::TASK::t_start, this);
			this->access->add("t_end", &DAO::SOURCE::TASK::t_end, this);
			this->access->add("status", &DAO::SOURCE::TASK::status, this);
			this->access->add("data", &DAO::SOURCE::TASK::data, this);
		};

		virtual ~TASK() {    };


};  // end class SOURCE_TASK


}	// END namespace SOURCE
}	// END namespace DAO





#endif 

