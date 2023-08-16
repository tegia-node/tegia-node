#include "gumbo.h"
#include <string>
#include <vector>

#include <tegia/core/core.h>
#include <tegia/core/json.h>

namespace core { 


class html_parser
{
   private:
   	  GumboOutput* output;
      void search_for_links(GumboNode* node, nlohmann::json * links, const std::string &prefix);

   public:	
   	  html_parser(const std::string &contents);
   	  ~html_parser();


      GumboNode * root; 
      std::vector<GumboNode *> find_child(GumboNode *node, const std::string &find_tag);

      std::string getAttr(GumboNode *curr_node, const std::string &attr_name);
      std::vector<GumboNode *> find_child_x(GumboNode *curr_node, const std::string &xpath);



      std::vector<GumboNode *> xpath(GumboNode *node, const std::string &xpath);

      std::string inner_text(GumboNode *node);

     /**
          \brief Функция возвращает массив ссылок с соответствующим префиксом


      */
   	  nlohmann::json get_links(const std::string &prefix);

 	  

};

}



