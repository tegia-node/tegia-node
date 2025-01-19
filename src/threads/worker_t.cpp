#include "worker_t.h" 


namespace tegia {
namespace threads {


//////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
//////////////////////////////////////////////////////////////////////////////////////////////////


worker_t::worker_t(tegia::threads::queue * _queue): _queue(_queue)
{

};


//////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
//////////////////////////////////////////////////////////////////////////////////////////////////


void worker_t::thread_fn()
{
	// std::cout << "worker_t::thread_fn()" << std::endl;

	this->_signal_fn();

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
			_task->fn();
			delete _task;

			locker.lock();   
		}
	}
};


} // END namespace threads
} // END namespace tegia

