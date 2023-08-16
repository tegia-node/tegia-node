#ifndef H_DAO_SOURCE_LIST
#define H_DAO_SOURCE_LIST

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

class LIST: public dao::Entity
{
	friend class ::SOURCE::TASK;

	protected:

		PROPDEFF2(unsigned long int, _id_owner, 0, nullptr, nullptr)
		PROPDEFF2(std::string, type, "", nullptr, nullptr)
		PROPDEFF2(std::string, name, "", nullptr, nullptr) 
		PROPDEFF2(std::string, url, "", nullptr, nullptr)
		PROPDEFF2(std::string, description, "{}", nullptr, nullptr)
		PROPDEFF2(bool, is_fl, false, nullptr, nullptr)
		PROPDEFF2(bool, is_ul, false, nullptr, nullptr)
		PROPDEFF2(bool, is_obj, false, nullptr, nullptr) 
		PROPDEFF2(bool, is_stat, false, nullptr, nullptr)
		PROPDEFF2(int, status, 0, nullptr, nullptr)
		PROPDEFF2(nlohmann::json, job_data, "{}", nullptr, nullptr)

	public:   

		LIST() : dao::Entity("source_list")
		{
			this->access->add("_id_owner",		&DAO::SOURCE::LIST::_id_owner, this);
			this->access->add("type",			&DAO::SOURCE::LIST::type, this);
			this->access->add("name",			&DAO::SOURCE::LIST::name, this);
			this->access->add("url",			&DAO::SOURCE::LIST::url, this);
			this->access->add("description",	&DAO::SOURCE::LIST::description, this);
			this->access->add("is_fl",			&DAO::SOURCE::LIST::is_fl, this);
			this->access->add("is_ul",			&DAO::SOURCE::LIST::is_ul, this);
			this->access->add("is_obj",			&DAO::SOURCE::LIST::is_obj, this);
			this->access->add("is_stat",		&DAO::SOURCE::LIST::is_stat, this);
			this->access->add("status",			&DAO::SOURCE::LIST::status, this);
			this->access->add("job_data",		&DAO::SOURCE::LIST::job_data, this);
		};

		virtual ~LIST() {    };


};  // end class LIST

}	// END namespace SOURCE
}	// END namespace DAO


#endif 

