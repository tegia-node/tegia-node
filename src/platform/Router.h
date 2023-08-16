#ifndef H_ROUTER
#define H_ROUTER
// -------------------------------------------------------------------------------------- //
//                                        INFO                                            //
// -------------------------------------------------------------------------------------- //

// --- http://forum.vingrad.ru/forum/topic-332106.html
// --- http://ru.cppreference.com/w/cpp/utility/functional/function

// -------------------------------------------------------------------------------------- //
//                                      INCLUDES                                          //
// -------------------------------------------------------------------------------------- //

#include <string>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <sstream>

#include <list>

#include <functional>

   //#include "cstring.h"
   #include "../../include/core/core.h"
   #include "../../include/core/json.h"

// -------------------------------------------------------------------------------------- //
//                                       DEFINES                                          //
// -------------------------------------------------------------------------------------- //


// -------------------------------------------------------------------------------------- //
//                                      STRUCTURES                                        //
// -------------------------------------------------------------------------------------- //


template<class _FN, class... _ARGS>
struct RouteParam
{ }; 


struct size_fn
{
    template <class _FN, class... _ARGS>
    constexpr decltype(auto) operator () (RouteParam<_FN, _ARGS...> & param)
    {
        return param.size();
    }
};


// -------------------------------------------------------------------------------------- //
//                                     ROUTER CLASS                                       //
// -------------------------------------------------------------------------------------- //

   template<class _Key, class _FN, class... _ARGS>
   class Router
   {
      protected:
      // ----------------------------- PROTECTED VARIABLES ------------------------------- //
         typedef std::map<_Key, std::function<_FN(_ARGS...)> > route_func_map_t;
         route_func_map_t mapping; 


         std::function<_FN(_ARGS...)> default_function;

      // ----------------------------- PROTECTED FUNCTIONS ------------------------------- //       


      public:
      // ------------------------------ PUBLIC VARIABLES --------------------------------- //


    
      // ------------------------------- PUBLIC FUNCTIONS -------------------------------- //       
        
         Router()
         { };


         ~Router()
         { 
            this->mapping.clear();
         };

         void add(_Key key, std::function<_FN(_ARGS...)> route_func )
         {
            this->mapping.insert ( 
               std::pair <_Key, std::function<_FN(_ARGS...)> > (key, route_func ) );
         };

         void add_default_function(std::function<_FN(_ARGS...)> route_func )
         {
            this->default_function = route_func;
         };
 
         
         _FN route(_Key key, _ARGS... _args)
         {
            auto it = this->mapping.find(key);
            if(it != this->mapping.end())
            {
               return it->second(_args...);
            }
            else
            {
               return this->default_function(_args...);
            }
         };



         _FN routemask(std::string key, _ARGS... _args)
         {
           for(auto it = mapping.begin(); it != mapping.end(); it++)
           {
              auto res = this->compare(it->first, key);
              if(res.first == true)
              {
                 //std::cout << "res.second: " << core::cast<std::string>(res.second) << std::endl;
                 return it->second(_args...);
              }
           }
           return this->default_function(_args...);
         };


         std::pair< std::function<_FN(_ARGS...)> , Json::Value> routemask2(std::string key)
         {
           for(auto it = mapping.begin(); it != mapping.end(); it++)
           {
              auto res = this->compare(it->first, key);
              if(res.first == true)
              {
                 //std::cout << "res.second: " << core::cast<std::string>(res.second) << std::endl;
                 return std::make_pair(it->second,res.second);
              }
           }
           return std::make_pair(this->default_function, core::cast<Json::Value>("{}"));
         };





         std::string routeApp(std::string method, std::string key, _ARGS... _args)
         {
           for(auto it = mapping.begin(); it != mapping.end(); ++it)
           {
             //std::string method = it->first.first;
             //std::string pattern = it->first.second;

             //cout<<it->first<<endl;

             if(it->first.first == method && this->compare(it->first.second, key))
             {
               return it->second(_args...);
             }
           }
           return this->default_function(_args...);
         };



         std::pair<bool,Json::Value> compare(std::string pattern, std::string path)
         {
            Json::Value root;
            //cout<<"pattern = "<<pattern<<endl;
            //cout<<"path = "<<path<<endl;

            std::vector<std::string> arr_pattern = core::explode(pattern,"/",false);
            std::vector<std::string> arr_path = core::explode(path,"/",false);

            if(arr_pattern.size() != arr_path.size())
            {
              //cout<<"route not found"<<endl<<endl;
              return std::pair<bool,Json::Value>(false,root);
            }

            // state = 0  - начальное состояние
            // state = 1  - пока вектора равны
            // state = 2  - вектора не равны

            int state = 0;
   
            for(int i = 0; i < arr_pattern.size(); i++)
            {
               if(arr_pattern[i] == arr_path[i])
               {
                 state = 1;
               }
               else
               {
                 // Обрабатываем ситуацию с шаблонными параметрами
                    std::string start_pattern = arr_pattern[i].substr(0,1);

                    //
                    //std::cout << "start_pattern = " << start_pattern << std::endl;
                  
                    if(start_pattern == "{")
                    {
                       state = 1;
                       // Формируем пару "имя = значение"
                       root[arr_pattern[i].substr(1, arr_pattern[i].size()-2)] = arr_path[i];

                       //std::cout << "==============================\n";
                       //std::cout << "name = " << arr_pattern[i].substr(1, arr_pattern[i].size()-2) 
                       //          << "; value = " << arr_path[i] << std::endl;
                    }
                    else
                    {
                       state = 2;
                       //cout<<"route not found"<<endl<<endl;
                       return std::pair<bool,Json::Value>(false,root);
                    }
               }
            }
            return std::pair<bool,Json::Value>(true,root);
         };


   };

   
#endif