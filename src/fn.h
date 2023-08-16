#ifndef H_TEGIA_FN
#define H_TEGIA_FN
// ---------------------------------------------------------------------------------------------------

#include <string>
#include <tegia2/context/context.h>

// LOGGER
#include "node/logger.h"
#define _LOG_LEVEL_ _LOG_NOTICE_
#include "node/log.h"
// LOGGER

std::string fn(int i)
{
	std::cout << "run task " << i << "; tid = [" << tegia::context::tid() << "] user = [" << tegia::context::user()->uuid() << "]" << std::endl;

	auto res = tegia::mysql::query("models2","SELECT count(*) FROM `entities`;");
	std::cout << "entities count = " << res->get("::count(*)",0) << std::endl;
	delete res;

	auto res2 = tegia::mysql::query("models","SELECT count(*) FROM `events_model`;");
	std::cout << "events_model count = " << res->get("::count(*)",0) << std::endl;
	delete res2;

	// std::this_thread::sleep_for(1s);

	/*
	auto conf = tegia::context::config("mvd");

	if(conf == nullptr)
	{
		std::cout << "mvd = nullptr" << std::endl;
	}
	else
	{
		std::cout << (*conf) << std::endl;
	}
	*/

	LDEBUG(tegia::context::tid());
	
	return "ok";
};

// ---------------------------------------------------------------------------------------------------
#endif
