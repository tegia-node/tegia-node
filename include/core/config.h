#ifndef _H_CORE_CONFIG_
#define _H_CORE_CONFIG_
// --------------------------------------------------------------------

// STL
	#include <iostream>
	#include <string>
	#include <vector>
	#include <fstream>
	#include <filesystem> 


	#include "json.h"


namespace core
{
	class config 
	{
		private:
			nlohmann::json jconf;

			std::string file_name;
			std::string log_dir;

		public: 
			config() {  };
			~config() {  };


			void load()
			{
				int status = 0;

				if(std::filesystem::exists("config.json"))
				{
					status = status + 1;
				}
					 
				if(std::filesystem::exists("/etc/tegia/config.json"))
				{
					//file_name = "/etc/tegia/config.json";	
					status = status + 2;
				}

				switch(status)
				{
					case 0:
					{
						// Не найден конфигурационный файл!!!
						std::cout << _ERR_TEXT_ << " Не найден файл конфигурации" << std::endl;
						exit(1);
					}
					break;

					case 1:
					{
						this->file_name = "config.json";
						this->log_dir = "./";

						this->jconf = core::json::file(this->file_name);
					}
					break;

					case 2:
					{
						this->file_name = "/etc/tegia/config.json"; 
						this->log_dir = "/var/log/tegia/";

						this->jconf = core::json::file(this->file_name);
					}
					break;

					case 3:
					{
						this->file_name = "config.json";
						this->log_dir = "./";

						this->jconf = core::json::file(this->file_name);
					}
					break;
				}
			};


			nlohmann::json & operator[] (const std::string & name)
			{
				if(this->jconf.contains(name) == true)
				{
					return this->jconf[name];
				}

				nlohmann::json tmp = nlohmann::json();
				return tmp; 
			};


			std::string & filename()
			{
				return this->file_name;
			}

			std::string & logdir()
			{
				return this->log_dir;
			}

			bool save()
			{
				std::ofstream ofs(this->file_name);
				ofs << this->jconf;
				ofs.close();

				//return core::json::save(this->jconf,this->file_name);
				return true;
			}
	};



}

#endif

