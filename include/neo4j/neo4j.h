#ifndef H_DB_NEO4J
#define H_DB_NEO4J

////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// INCLUDES                                                                               //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

//	STL
	#include <iostream>
	#include <string>

	#include <tegia/core/http2.h>

//	VENDORS 
	#include <nlohmann/json.hpp>


////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// NAMESPACE dao::neo4j                                                                   //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////


namespace dao { 
namespace neo4j {

//
// code   |   title
// -----------------------------------------------------------------------------------------
// 1000   |   Neo.ClientError.Schema.ConstraintValidationFailed
//

struct status
{
	int code;
	std::string title;
	std::string message; 
};

struct result
{
	long int gid;
	struct status status;
};




struct result query(const std::string &connection, const nlohmann::json &query)
{
   struct result res;
   res.status.code = 0;
   res.status.title = "";
   res.status.message = "";
   res.gid = 0;

   auto http = new core::http2();

   // ------------------------------------------------------------------------
   // Устанавливаем заголовки для выполнения запроса
   // ------------------------------------------------------------------------

   auto headers = http->default_post_request();
   headers["headers"].push_back("Accept: application/json;charset=UTF-8");
   headers["headers"].push_back("Content-Type: application/json");
   headers["basic"]["user"] = "neo4j";
   headers["basic"]["password"] = "P@$$w0rd";

   // ------------------------------------------------------------------------
   // Выполняем запрос
   // ------------------------------------------------------------------------

   nlohmann::json data;   
   int status = http->post("http://localhost:7474/db/neo4j/tx/commit", query.dump(), headers);

   switch(status)
   {
      case 200:
      {
         data = nlohmann::json::parse(http->buffer); 
      }
      break;

      default:
      {
         std::cout << "status = " << status << std::endl;
         std::cout << http->buffer << std::endl;

         exit(0);
      }
      break;
   }
   delete http;

   // ------------------------------------------------------------------------
   // Определяем состояние ответа neo4j
   // ------------------------------------------------------------------------


   // TODO 

   return res;
}




struct result create(const std::string &connection, const std::string &labels, const nlohmann::json &props)
{
   struct result res;
   res.status.code = 0;
   res.status.title = "";
   res.status.message = "";
   res.gid = 0;

   auto http = new core::http2();

   // ------------------------------------------------------------------------
   // Устанавливаем заголовки для выполнения запроса
   // ------------------------------------------------------------------------

   auto headers = http->default_post_request();
   headers["headers"].push_back("Accept: application/json;charset=UTF-8");
   headers["headers"].push_back("Content-Type: application/json");
   headers["basic"]["user"] = "neo4j";
   headers["basic"]["password"] = "P@$$w0rd";

   // ------------------------------------------------------------------------
   // Формируем запрос к neo4j
   // ------------------------------------------------------------------------

   nlohmann::json jtmp;
   nlohmann::json jdata;

   jtmp["parameters"]["props"] = props;
   jtmp["statement"] = "CREATE (n" + labels + " $props) RETURN ID(n)";
   jdata["statements"].push_back(jtmp);

   // ------------------------------------------------------------------------
   // Выполняем запрос
   // ------------------------------------------------------------------------

   nlohmann::json data;   
   int status = http->post("http://localhost:7474/db/neo4j/tx/commit", jdata.dump(), headers);

   switch(status)
   {
      case 200:
      {
         data = nlohmann::json::parse(http->buffer); 
      }
      break;

      default:
      {
         std::cout << "status = " << status << std::endl;
         std::cout << http->buffer << std::endl;

         exit(0);
      }
      break;
   }
   delete http;

   // ------------------------------------------------------------------------
   // Определяем состояние ответа neo4j
   // ------------------------------------------------------------------------

   int flag = 0;
   if(data["results"].size() > 0 && data["errors"].size() == 0)
   {
      flag = 10;
   }

   if(data["results"].size() > 0 && data["errors"].size() > 0)
   {
      flag = 11;
   }

   if(data["results"].size() == 0 && data["errors"].size() > 0)
   {
      flag = 1;
   }

   // ------------------------------------------------------------------------
   // Обрабатываем ответ
   // ------------------------------------------------------------------------

   switch(flag)
   {
      case 0:
      {
         std::cout << "flag = 0" << std::endl;
         std::cout << data.dump() << std::endl;
         exit(0);
      }
      break;

      case 1:
      {
         res.status.title   = data["errors"][0]["code"].get<std::string>();
         res.status.message = data["errors"][0]["message"].get<std::string>();
         
         if(res.status.title == "Neo.ClientError.Schema.ConstraintValidationFailed")
         {
            res.status.code = 1000;
            std::string message = res.status.message;
            const std::regex date_regex("Node\\([0-9]+\\)");
            std::smatch date_match;
 
            if(std::regex_search(message, date_match, date_regex) == true)
            {
               std::string _str(date_match[0]);
               res.gid = core::cast<long int>(_str.substr(5,_str.size()-6));
            }
            else
            {
               std::cout << message << std::endl;
               std::cout << "     (null)" << std::endl;
               exit(0);
            }
         }
      }
      break;

      case 10:
      {
         nlohmann::json::json_pointer ptr("/results/0/data/0/row/0");
         if(data[ptr].is_null() == false)
         {
            res.gid = data[ptr].get<long int>();
         }  
      }
      break;

      case 11:
      {
         std::cout << "flag = 0" << std::endl;
         std::cout << data.dump() << std::endl;
         exit(0);
      }
      break;
   }

   return res;
};


}  // end namespace neo4j
}  // end namespace dao




#endif
