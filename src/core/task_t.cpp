#include <format>
#include <tegia/core/task_t.h>


///////////////////////////////////////////////////////////////////////////////////
/*

*/
///////////////////////////////////////////////////////////////////////////////////


namespace tegia {

int task_t::status(int status)
{
	std::string query = std::format(
		"UPDATE `{}` SET `status` = {}, `result` = '{}', `error` = '{}' WHERE `hash` = '{}'",
		this->table,
		status,
		tegia::mysql::strip(this->result.dump()),
		tegia::mysql::strip(this->error.dump()),
		this->hash
	);

	auto res = tegia::mysql::query(this->connection,query);

	// std::cout << "resutl = " << res->code << std::endl;
	// std::cout << "query  = " << query << std::endl;

	delete res;

	this->_status = status;
	return status;
};	

}


///////////////////////////////////////////////////////////////////////////////////
/*

*/
///////////////////////////////////////////////////////////////////////////////////


namespace tegia {

int task_t::status()
{
	return this->_status;
};
	
}


///////////////////////////////////////////////////////////////////////////////////
/*

*/
///////////////////////////////////////////////////////////////////////////////////


namespace tegia {

nlohmann::json task_t::json()
{
	nlohmann::json tmp;
	tmp["category"] = this->category;
	tmp["type"] = this->type;
	tmp["status"] = this->_status;
	tmp["priority"] = this->priority;
	tmp["t_create"] = this->t_create;
	tmp["data"] = this->data;
	tmp["error"] = this->error;
	tmp["result"] = this->result;
	tmp["callback"] = this->callback;
	tmp["wsid"] = this->wsid;
	tmp["hash"] = this->hash;
	return std::move(tmp);
};

}


///////////////////////////////////////////////////////////////////////////////////
/*

*/
///////////////////////////////////////////////////////////////////////////////////


namespace tegia {

int task_t::load(nlohmann::json &jtask)
{
	this->category = jtask["category"].get<std::string>();
	this->hash = jtask["hash"].get<std::string>();
	this->_status = jtask["status"].get<int>();
	this->data = jtask["data"];
	this->error = jtask["error"];
	this->result = jtask["result"];

	if(jtask.contains("callback") == true)
	{
		this->callback = jtask["callback"];
	}

	return 0;
};
	
}


///////////////////////////////////////////////////////////////////////////////////
/*

*/
///////////////////////////////////////////////////////////////////////////////////

