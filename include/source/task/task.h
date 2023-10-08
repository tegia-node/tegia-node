#ifndef H_SOURCE_TASK
#define H_SOURCE_TASK

////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// INCLUDES                                                                               //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

#include <mutex>

#include <tegia/source/dao/dao_source_list.h>
#include <tegia/source/dao/dao_source_task.h>

#include <tegia/db/mysql/mysql.h>
#include <tegia/core/crypt.h>
#include <tegia/core/time.h>

////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// SOURCE TASK CLASS                                                                      //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

namespace SOURCE {
namespace STATUS {

const int START = 1;
const int END   = 2;
const int ERROR = 3;

}

class TASK
{
	protected:
		DAO::SOURCE::TASK * task = nullptr;
		DAO::SOURCE::LIST * source = nullptr;

		std::mutex task_lock;

	protected:

		virtual int _run(int id_source, nlohmann::json *responcedata)
		{
			//
			// Проверяем статус задачи
			//

			if(this->task != nullptr)
			{
				(*responcedata) = nlohmann::json();
				(*responcedata)["status"] = 200;
				(*responcedata)["data"]["message"] = "task is allready run";
				(*responcedata)["data"]["source"]["id"] = this->task->__id_source;	
				(*responcedata)["data"]["source"]["name"] = this->source->_name;
				(*responcedata)["data"]["task"]["_id"] = this->task->__id;
				(*responcedata)["data"]["task"]["uuid"] = this->task->_uuid;
				(*responcedata)["data"]["task"]["t_start"] = this->task->_t_start;
				(*responcedata)["data"]["task"]["data"] = this->task->_data;
				this->task_lock.unlock();
				return 400;
			}

			//
			// Загружаем данные об источнике
			//

			this->source = new DAO::SOURCE::LIST();

			auto source_res = tegia::mysql::read("osint_sources",this->source," _id = " + std::to_string(id_source));
			switch(source_res.code)
			{
				case 200:
				{

				}
				break;

				case 404:
				{
					(*responcedata) = nlohmann::json();
					(*responcedata)["status"] = "error";
					(*responcedata)["error"]["code"] = 404;
					(*responcedata)["error"]["reason"] = 2;
					(*responcedata)["error"]["message"] = "source [" + std::to_string(id_source) + "] not found";
					(*responcedata)["error"]["task"]["source_id"] = id_source;	
					this->task_lock.unlock();
					return 404;
				}
				break;
			}

			//
			// Инициализируем задачу
			//

			this->task = new DAO::SOURCE::TASK();

			this->task->__id_source = id_source;
			this->task->_uuid = tegia::random::uuid();
			this->task->_status = ::SOURCE::STATUS::START;
			this->task->_t_start = core::time::current();
			this->task->_data = this->source->_job_data;

			auto task_res = tegia::mysql::create("osint_sources",this->task);

			(*responcedata) = nlohmann::json();
			(*responcedata)["status"] = 200;
			(*responcedata)["data"]["message"] = "task is run";
			(*responcedata)["data"]["source"]["id"] = this->task->__id_source;	
			(*responcedata)["data"]["source"]["name"] = this->source->_name;
			(*responcedata)["data"]["task"]["_id"] = this->task->__id;
			(*responcedata)["data"]["task"]["uuid"] = this->task->_uuid;
			(*responcedata)["data"]["task"]["t_start"] = this->task->_t_start;
			(*responcedata)["data"]["task"]["data"] = this->task->_data;

			return 200;
		};	


		virtual int _status(int status, nlohmann::json &data)
		{
			this->task->_status = status;
			this->task->_data = data;
			auto task_res = tegia::mysql::update("osint_sources",this->task);

			this->source->_job_data = data;
			auto source_res = tegia::mysql::update("osint_sources",this->source);

			return 0;
		};


	public:
		TASK() { };
		virtual ~TASK() {    };

		virtual int run(int id_source, nlohmann::json * responcedata)
		{
			int res = 0;
			this->task_lock.lock();
			res = this->_run(id_source, responcedata);
			this->task_lock.unlock();
			return res;
		};	
		
		virtual int status(int status, nlohmann::json data = nlohmann::json())
		{
			int res = 0;
			this->task_lock.lock();
			res = this->_status(status, data);
			this->task_lock.unlock();
			return res;
		};

		virtual int end(int status, nlohmann::json data = nlohmann::json())
		{
			this->task_lock.lock();

			this->task->_t_end = core::time::current();
			this->task->_status = status;
			this->task->_data = data;
			auto task_res = tegia::mysql::update("osint_sources",this->task);
			delete this->task;
			this->task = nullptr;
			
			this->source->_job_data = data;
			auto source_res = tegia::mysql::update("osint_sources",this->source);
			delete this->source;
			this->source = nullptr;

			this->task_lock.unlock();

			return 0;
		};

		nlohmann::json json()
		{
			return this->task->json();
		};

};	// END class TASK


}	// END namespace SOURCE




#endif 

