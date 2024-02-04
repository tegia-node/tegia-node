

#include <iostream>
#include <thread>
#include <stdlib.h>
#include <cstdio>
#include <signal.h>
#include <execinfo.h>
#include <typeinfo> 
#include <filesystem>

#include <tegia/core/const.h>
#include "node/node.h"


int main(int argc, char* argv[])
{
	
	std::string command;
	std::string path;

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
		std::cout << "  --local                 Указывает, что конфигурация роли tegia-node" << std::endl;
		std::cout << "                          располагается в текущей директории" << std::endl;
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
	if(!std::filesystem::is_regular_file(file))
	{
		std::cout << _RED_TEXT_ << "config file [" << file.string() << "] not found" << _BASE_TEXT_ << std::endl;
		exit(0);
	}


	try
	{
		auto node = tegia::node::node::instance();
		node->run();

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
