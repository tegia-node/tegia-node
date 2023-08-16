#include <iostream>
#include <string>
#include <filesystem>


#include "../../include/const.h"
#include "../../include/core/json.h"
#include "../../include/core/config.h"


int main(int argc, char* argv[])
{   
   /*for (int i = 0; i < argc; i++) {
        // Выводим список аргументов в цикле
        std::cout << "Argument " << i << " : [" << argv[i] << "]" << std::endl;
   }*/

   std::string command;
   std::string path;

   if(argc > 1)
   {
      command = argv[1];
      path = argv[2];

      std::cout << command << " " << path << std::endl;


   // =========================================================================================
      if("--add" == command)
      {
         std::filesystem::path file(path);

         if(std::filesystem::is_regular_file(file))
         {
            core::config conf;
            conf.load();

            std::cout << conf.filename() << std::endl;

         // Читаем имя конфигурации и ее параметры
            auto jconf = core::json::file(file.string());
            
            Json::Value tmp;
            tmp["conf_file"] = file.filename().string();
            tmp["isload"] = true;
            tmp["name"] = jconf["name"].asString();
            tmp["path"] = file.parent_path().parent_path().string();

            std::cout << core::cast<std::string>(tmp) << std::endl;

            std::cout << core::cast<std::string>(conf["applications"]) << std::endl;
            std::cout << core::cast<std::string>(conf["configurations"]) << std::endl;

            bool flag = false; 
            for(auto it = conf["configurations"].begin(); it != conf["configurations"].end(); it++ )
            {
               std::cout << (*it)["name"].asString() << std::endl;

               if(jconf["name"].asString() == (*it)["name"].asString())
               {
                  //(*it)["isload"] = true;
                  std::cout << "конфигурация " << tmp["name"].asString() << " уже сущестует" << std::endl;
                  flag = true;
                  break;
               }
            }

            if(flag == false)
            {
               conf["configurations"].append(tmp);
            }

            std::cout << core::cast<std::string>(conf["configurations"]) << std::endl;

            conf.save();
         }
         else
         {
            std::cout << "Укзан некорректный файл конфигурации" << std::endl;
         }
      }

   // =========================================================================================
      if("--remove" == command)
      {
          core::config conf;
          conf.load();

          for(int i = 0; i < conf["configurations"].size(); i++)
          {
             if(path == conf["configurations"][i]["name"].asString())
             {
                Json::Value tmp;
                conf["configurations"].removeIndex(i,&tmp);                   
             }
          }         

          conf.save();
      }

   // =========================================================================================
      if("--on" == command)
      {
          core::config conf;
          conf.load();

          for(auto it = conf["configurations"].begin(); it != conf["configurations"].end(); it++ )
          {
              if(path == (*it)["name"].asString())
              {
                 (*it)["isload"] = true;
                 break;
              }
          }

          conf.save();
      }

   // =========================================================================================
      if("--off" == command)
      {
          core::config conf;
          conf.load();

          for(auto it = conf["configurations"].begin(); it != conf["configurations"].end(); it++ )
          {
              if(path == (*it)["name"].asString())
              {
                 (*it)["isload"] = false;
                 break;
              }
          }

          conf.save();
      }  

   // =========================================================================================
   }

   return 0;
}
