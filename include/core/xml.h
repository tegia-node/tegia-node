#ifndef H_K_XML
#define H_K_XML
// -------------------------------------------------------------------------------------- //
//                                      INCLUDES                                          //
// -------------------------------------------------------------------------------------- //

//   #include "k_http.h"
 
   #include <libxml/xmlexports.h>
   #include <libxml/xmlreader.h>
   #include <libxml/list.h>
   #include <libxml/xpath.h>
   #include <libxml/xpathInternals.h>
   #include <libxml/HTMLparser.h>
   
   #include <iostream>
   #include <memory>
   #include <math.h>

   #include <malloc.h>
   #include <time.h>

   #include <cstring>
   #include <cstdio>
   #include <cstdlib>
 
   using namespace std; 

   
   #include <string>
   #include <iostream>
   #include <map>
   #include <vector>      
   
   #include <curl/curl.h>




// -------------------------------------------------------------------------------------- //
//                                       DEFINES                                          //
// -------------------------------------------------------------------------------------- //



// -------------------------------------------------------------------------------------- //
//                                      STRUCTURES                                        //
// -------------------------------------------------------------------------------------- //



// -------------------------------------------------------------------------------------- //
//                                        XML CLASS                                       //
// -------------------------------------------------------------------------------------- //

namespace core
{

class xml
{
  protected:   

    std::string data;                      //код веб-страницы
    xmlXPathObjectPtr  XPathObj;
    xmlDoc *doc;
    xmlXPathContextPtr XPathContext;  

    bool convert(std::string charset);     
    bool initialize(std::string charset);

  public: 

    xml(); 
    ~xml();

    static xml* loadFile(std::string filename, string charset = "UTF-8");
    static xml* loadString(std::string str, string charset = "UTF-8");
    static xml* loadSString(std::string str, string charset = "UTF-8");
    
    bool streamFile(const std::string &filename);

 // читает данные из указанного локального файла
    bool loadFromFile(std::string filename, string charset = "UTF-8");
 // читает данные по протоколу http   
 // bool http(string url, string charset);
 // читает данные из строки
    bool loadFromString(std::string str, string charset = "UTF-8");
    bool loadFromSString(const std::string &str, const string &charset = "UTF-8");

    //bool init(string url, string charset, int local);
    bool free(void);
    
    vector<string> select(std::string XPath, int type) const;
    std::string    select1(std::string XPath, int type) const;
    vector<string> select_name(std::string XPath) const;
    vector<string> select_text(std::string XPath) const;     //применение XPath + замена всех тэгов на пробелы
    vector<string> select_html(std::string XPath) const;     //применение XPath без замены тэгов на пробелы

    bool add_namespace(const std::string &name, const std::string &url);


    std::string dump(void);
};

/*
class k_xml: public k_http
{
  protected:    
    xmlXPathObjectPtr  XPathObj;
    htmlDocPtr htmlDoc;
    xmlXPathContextPtr XPathContext;  

    bool convert(string charset);     
    
  public: 

    k_xml(); 
    ~k_xml();
    
 // читает данные из указанного локального файла
    bool file(string filename, string charset);
 // читает данные по протоколу http   
    bool http(string url, string charset);
 // читает данные из строки
    bool loadString(string str, string charset);

    bool initialize(string charset);


    //bool init(string url, string charset, int local);
    bool free(void);
    
    vector<string> select(string XPath, int type);
    vector<string> select_text(string XPath);
    vector<string> select_html(string XPath);
};
*/

}

#endif