#ifndef H_TEGIA_THREAD_WORKER
#define H_TEGIA_THREAD_WORKER

#include "queue.h" 


namespace tegia {
class context2;
}	// END namespace tegia


namespace tegia {
namespace threads {

class worker
{
	protected:
		std::thread		thread;
		tegia::threads::queue * _queue;
		std::function<void(const std::string &)> _init_callback;
		bool is_work = true;

	public:

		std::string tid;


		worker(
			tegia::threads::queue * _queue, 
			std::function<::tegia::context2 const * (const std::string &)> _thread_init): thread(&worker::thread_fn, this, _thread_init)                                    
		{  
			this->_queue = _queue;
			this->tid = core::cast<std::string>(this->thread.get_id());
		};


		~worker()
		{  
			this->thread.join();
		};


		void thread_fn(std::function<::tegia::context2 const * (const std::string &)> _thread_init)
		{
			//
			// INIT THREAD DATA
			//

			::tegia::context2 const * context = _thread_init(this->tid);

			//
			// THREAD LOOP
			//

			while(this->is_work)
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
					_task->fn(context);
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