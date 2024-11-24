// -------------------------------------------------------------------------------------- //
//                                      INCLUDES                                          //
// -------------------------------------------------------------------------------------- //

#include "logger.h"
#include <tegia/core/const.h>

// -------------------------------------------------------------------------------------- //
//                                        CLASS                                           //
// -------------------------------------------------------------------------------------- //


namespace tegia
{

logger* logger::_self = NULL;

/////////////////////////////////////////////////////////////////////////////////////////////

logger* logger::instance(const std::string &logdir, int log_level)
{
	if(!_self)
	{
		_self = new logger(logdir, log_level);
	}
	return _self;
};


logger::logger(const std::string &logdir, int log_level)
{ 
    std::string logFilePath = logdir + "main.log";
    this->flog_all.open(logFilePath, std::ios::in | std::ios::app | std::ios::binary);
    if (!this->flog_all.is_open()) // если файл не открыт
    {
        std::cout << _ERR_TEXT_ << "Файл main.log не может быть открыт!" << std::endl;
        std::exit(EXIT_FAILURE);
    }
}; 

/////////////////////////////////////////////////////////////////////////////////////////
logger::~logger()
{ 
	this->flog_all.close(); 
};



void logger::writer(
	const int level,
	const std::chrono::high_resolution_clock::time_point now,
	const std::string &filename, 
	const std::string &function, 
	const int line,			
	const std::string &message
){

	std::string _err_text = ""; 
	auto ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now).time_since_epoch().count();
			
	_err_text = "[" + std::to_string(ms) + "]";

	switch(level)
	{
		case _LOG_DEBUG_:
			_err_text = _err_text + " [ debug ] ";
		break;
		case _LOG_NOTICE_:
			_err_text = _err_text + " [ notice ] ";
		break;
		case _LOG_WARNING_:
			_err_text = _err_text + " [ warning ] ";
		break;
		case _LOG_ERROR_:
			_err_text = _err_text + " [ error ] ";
		break;
		case _LOG_CRITICAL_:
			_err_text = _err_text + " [ critical ] ";
		break;
	}

	_err_text = _err_text + "[function: " + function + "() in " + filename + ":" + std::to_string(line) + "]\n";

	this->log_lock.lock();
	this->flog_all << _err_text << message << "\n" << std::endl;
	this->flog_all.flush();
	this->log_lock.unlock();     
};


} // end namespace tegia





