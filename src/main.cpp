

#include <iostream>
#include <thread>
#include <stdlib.h>
#include <cstdio>
#include <signal.h>
#include <execinfo.h>
#include <typeinfo> 

// #include "platform/Node2.h" 
#include "node/node.h" 
#include <tegia/core/cast.h>


#pragma GCC visibility push(hidden)


void reportTrouble()
{
	void *callstack[128];
	int frames = backtrace(callstack, 128);
	char **strs=backtrace_symbols(callstack, frames);
	// тут выводим бэктрейс в файлик crash_report.txt
	// можно так же вывести и иную полезную инфу - версию ОС, программы, etc
	FILE *f = fopen("crash_report.txt", "w");
	if (f)
	{
		for(int i = 0; i < frames; ++i)
		{
			fprintf(f, "%s\n", strs[i]);
		}
		fclose(f);
	}
	free(strs);
};



void catchCrash(int signum)
{
	reportTrouble(); // отправляем краш-репорт
	signal(signum, SIG_DFL); // перепосылаем сигнал
	exit(3); //выходим из программы
};


int main(int argc, char* argv[])
{
	std::string command;
	std::string path;

	std::string config_path = "/etc/tegia/config.json";
	std::string mode = "default";

	if(argc == 1)
	{
		std::cout << _RED_TEXT_ << "Не заданы ключи запуска Платформы" << _BASE_TEXT_ << std::endl;		
		std::cout << "Запуск Платформы: tegia-node [КЛЮЧ] [АРГУМЕНТ]" << std::endl;
		std::cout << "По команде «tegia-node --help» можно получить дополнительную информацию" << std::endl;
		exit(0);
	}

	command = argv[1];
	bool flag = false;

	//
	// Проверка ключей
	//

	if("--help" == command)
	{
		std::cout << _GRAY_TEXT_ << "tegia llc @copyright 2018-2022" << _BASE_TEXT_ << std::endl;
		std::cout << "Запуск платформы: tegia-node [КЛЮЧ] [АРГУМЕНТ]" << std::endl;
		std::cout << " " << std::endl;
		std::cout << "  --local                 Указывает, что в текущей директории располагается" << std::endl;
		std::cout << "                          конфигурация роли tegia-node" << std::endl;
		std::cout << "  --config [directory]    Указывает [directory] где располагается конфигурация" << std::endl;
		std::cout << "                          роли tegia-node" << std::endl;
		exit(0);
	}

	if("--config" == command)
	{
		path = argv[2];

		std::filesystem::path _dir(path);
		if(std::filesystem::is_directory(_dir))
		{
			std::filesystem::current_path(_dir);
			flag = true;
		}
		else
		{
			std::cout << _RED_TEXT_ << path << " is not a directory" << _BASE_TEXT_ << std::endl;
			exit(0);
		}
	}

	if("--local" == command)
	{
		flag = true;
	}

	if(flag == false)
	{
		std::cout << _RED_TEXT_ << "Неверный ключ [" << command << "]" << _BASE_TEXT_ << std::endl;
		std::cout << "По команде «tegia-node --help» можно получить дополнительную информацию" << std::endl;
		exit(0);
	}

	//
	// Проверка конфигурации
	//

	std::filesystem::path file("./config.json");
	if(std::filesystem::is_regular_file(file))
	{
		config_path = "./config.json";
		mode = "custom";
	}
	else
	{
		std::cout << _RED_TEXT_ << "config file [" << file.string() << "] not found" << _BASE_TEXT_ << std::endl;
		exit(0);
	}

	//
	// Запуск Платформы
	//

	signal(SIGSEGV, catchCrash);
	signal(SIGINT, tegia::node::stop_node);
	
	try
	{
		auto node = tegia::node::instance();

		//
		// TODO: эффективное использование главного потока приложения
		//
		
		while(true)
		{
			sleep(100);
		}
	}

	catch (std::exception &e)
	{ 
		std::cout << "[" << e.what() << "]" <<std::endl;
	}
}
