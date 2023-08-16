#include <string>
#include <vector>
#include <iostream>
#include <cstring>


#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#include <libxml/HTMLparser.h>
#include <libxml/HTMLtree.h>

// VENDORS 
   #include <nlohmann/json.hpp>

   #include <tegia/const.h>

namespace core { 
namespace html { 



struct node
{
   xmlNodePtr libxml_node;

   node();
   node(xmlNodePtr libxml_node);
   std::string get_attr(const std::string &name);
   std::string get_text();
   std::string get_name();
   int child_count();

   node child();
   //std::string dump();
};



struct nodes
{
   xmlXPathContextPtr xpathCtx;
   xmlXPathObjectPtr xpathObj;

   //xmlNodeSetPtr libxml_nodes;

   ~nodes();

   //xmlNodePtr & operator[] (const int index);
   node operator[] (const int index);
   int count();
};



class parser
{
   private:

   public:	

   xmlDocPtr html;

      parser();
      parser(const std::string &content);
      ~parser();

      void parseXML(const std::string &content);
      nodes xpath(const std::string &xpath, nlohmann::json ns = nlohmann::json());

};

} // END namespace html
} // END namespace core



