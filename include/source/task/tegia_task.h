#ifndef H_TEGIA_TASK
#define H_TEGIA_TASK

#include <tegia/tegia.h>
#include <tegia/core/core.h>


namespace tegia {

class task
{
	public:

		//
		// FIELDS
		//

		int status = 0;
		std::string uuid = "";	// идентификатор задачи
		std::string type = "";	// тип задачи
		nlohmann::json data = nullptr;
		nlohmann::json callback = nullptr;
		nlohmann::json error = nullptr;

		//
		// FUNCTONS
		//

		task(nlohmann::json * task_data)
		{
			// TODO: verify
			this->status = (*task_data)["task"]["status"].get<int>();
			this->uuid = (*task_data)["task"]["uuid"].get<std::string>();
			this->type = (*task_data)["task"]["type"].get<std::string>();
			this->data = (*task_data)["task"]["data"];
			this->callback = (*task_data)["task"]["callback"];

			if((*task_data).contains("error") == true)
			{
				error = (*task_data)["error"];
			}
		};

		
		nlohmann::json json()
		{
			nlohmann::json task;
			task["task"]["uuid"] = this->uuid;
			task["task"]["status"] = this->status;
			task["task"]["type"] = this->type;
			task["task"]["data"] = this->data;
			task["task"]["callback"] = this->callback;

			if(error != nullptr)
			{
				task["task"]["error"] = this->error;
			}

			return std::move(task);
		};


		void end()
		{
			std::cout << _YELLOW_ << "TASK END" << _BASE_TEXT_ << std::endl;
			
			tegia::message::send(
				this->callback["actor"].get<std::string>(),
				this->callback["action"].get<std::string>(),
				this->json());			
		};

		~task(){};

	private:

};


class tasks_map
{
	public:
		tasks_map(){};
		~tasks_map(){};

		bool add(tegia::task * _task)
		{
			this->mutex.lock();
			

			this->mutex.unlock();
			return true;
		};

	private:

		std::mutex mutex;
		std::unordered_map<std::string, tegia::task *> _tasks;

};

}	// END namespace tegia
 

#endif