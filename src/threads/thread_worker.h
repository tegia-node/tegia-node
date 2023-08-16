#ifndef _TEGIA_THREAD_WORKER_H_
#define _TEGIA_THREAD_WORKER_H_
// --------------------------------------------------------------------

#include "thread_queue.h" 

namespace tegia {
namespace threads {

class worker
{
	friend class A2HTTP_Session;
	friend class A2Auth;

	protected:
		std::thread		thread;
		tegia::threads::queue * _queue;

	public:

		worker(tegia::threads::queue * _queue): thread(&worker::thread_fn, this)                                    
		{  
			this->_queue = _queue;
			std::cout << "[constructor] tid = " << core::cast<std::string>(this->thread.get_id()) << std::endl;
			// thread_context = std::make_shared<tegia::threads::context>();
		};

		~worker()
		{  
			this->thread.join();
		};

		std::string getTID()
		{
			return core::cast<std::string>(this->thread.get_id());
		};


		void thread_fn()
		{  
			while(true)
			{

				std::unique_lock<std::mutex> locker(this->_queue->mutex);
				// Ожидаем уведомления, и убедимся что это не ложное пробуждение
				// Поток должен проснуться если очередь не пустая либо он выключен

				this->_queue->cv.wait(locker);	


				bool flag = true;

				while(flag)
				{
					tegia::threads::task * _task = this->_queue->get_task();
					
					if(_task == nullptr)
					{
						flag = false;
						continue;
					}

					locker.unlock();
					thread_data->context = _task->_context;
					_task->_fn();
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