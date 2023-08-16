#ifndef H_TEGIA_THREAD_WORKER
#define H_TEGIA_THREAD_WORKER
// --------------------------------------------------------------------

#include "data.h" 
#include "queue.h" 
//#include "pool.h"

#include <tegia/core/cast.h>
#include <tegia2/core/json.h>

#include "../node/config.h"


namespace tegia {
namespace threads {

class worker
{
	protected:
		std::thread		thread;
		tegia::threads::queue * _queue;
		std::function<void(const std::string &)> _init_callback;

	public:

		std::string tid;

		worker(tegia::threads::queue * _queue, const nlohmann::json &config, std::function<void(const std::string &)> _init_callback): thread(&worker::thread_fn, this, config)                                    
		{  
			this->_init_callback = _init_callback;
			this->_queue = _queue;
			this->tid = core::cast<std::string>(this->thread.get_id());
		};


		~worker()
		{  
			this->thread.join();
		};


		void thread_fn(const nlohmann::json config)
		{
			//
			// INIT THREAD DATA
			//

			::tegia::threads::data->init(config);
			this->_init_callback(this->tid);

			//
			// YHREAD LOOP
			//

			while(true)
			{

				std::unique_lock<std::mutex> locker(this->_queue->mutex);
				// Ожидаем уведомления, и убедимся что это не ложное пробуждение
				// Поток должен проснуться если очередь не пустая либо он выключен

				this->_queue->cv.wait(locker);	


				bool flag = true;

				while(flag)
				{
					tegia::threads::task * _task = this->_queue->get();
					
					if(_task == nullptr)
					{
						flag = false;
						continue;
					}

					locker.unlock();
					// thread_data->context = _task->_context;
					_task->fn();
					delete _task;

					locker.lock();   
				}   
			}
		};

}; // class worker

}  // namespace threads
}  // namespace tegia



// --------------------------------------------------------------------
#endif