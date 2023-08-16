   #include <stddef.h>

   #include <string>
   #include <iostream>
   #include <fstream>
   #include <iconv.h>
   #include <vector> 

// Обработка исключений
   #include <stdexcept>

   #include <tegia/core/xml.h>
   //#include "k_log.h"


//inline std::string replace(std::string text, std::string s, std::string d)
//{
//  for(unsigned index=0; index=text.find(s, index), index!=std::string::npos;)
//  {
//    text.replace(index, s.length(), d);
//    index+=d.length();
//  }
//  return text;
//}

namespace core
{

/////////////////////////////////////////////////////////////////////////////////////////////
xml::xml()
{ 
	this->doc = nullptr;
	this->XPathContext = nullptr;
};

/////////////////////////////////////////////////////////////////////////////////////////////
xml::~xml()
{ 
	if(this->XPathContext != nullptr)
	{
		xmlXPathFreeContext(this->XPathContext);
		this->XPathContext = nullptr;
	}
	
	if(this->doc != nullptr)
	{
		xmlFreeDoc(this->doc);
		this->doc = nullptr;
	}
};





/////////////////////////////////////////////////////////////////////////////////////////////
bool xml::streamFile(const std::string &filename) 
{
	// http://stackoverflow.com/questions/36210661/in-libxml2-c-with-vs-2015-xmldoc-is-not-bool-but-why-does-it-say-bool
	//xmlDoc *doc = nullptr;   
	this->doc = xmlReadFile(filename.c_str(), "UTF-8", XML_PARSE_NOWARNING | XML_PARSE_NOERROR);
	if(this->doc != nullptr)
	{
		this->XPathContext = xmlXPathNewContext(this->doc);
		if(this->XPathContext != nullptr)
		{
			std::cout << "error XPathContext from " << filename << std::endl;
			return true;
		}
		else
		{
			return false;
		}
		
		//xmlNode *root_element = xmlDocGetRootElement(doc);
		//std::cout << root_element->name << std::endl;
	}

	return false;

	//xmlFreeDoc(doc);       // free document
	//xmlCleanupParser();    // Free globals
}


/////////////////////////////////////////////////////////////////////////////////////////////

bool xml::add_namespace(const std::string &name, const std::string &url)
{
	xmlXPathRegisterNs(this->XPathContext,  BAD_CAST name.c_str(), BAD_CAST url.c_str());
};

/////////////////////////////////////////////////////////////////////////////////////////////
bool xml::loadFromFile(std::string filename, std::string charset)
{
	//cout<<filename<<endl;

	ifstream file;
	file.open(filename.c_str(),ios::binary );
	if(!file.is_open())
	{
		// TODO: Файл не открыт
		//   пишем в лог
		//   бросаем исключение
		// -- k_log *loger = k_log::inst("main.log",_LOG_INFO_);
		// -- loger->msg(_LOG_ERROR_ , "file " + filename + " not found!",__LOCATION__);
		//cout<<"error"<<endl;
		//throw invalid_argument("Файл не найден");
	}
	

	// get length of file:
	file.seekg (0, ios::end);
	int length = file.tellg();
	file.seekg (0, ios::beg);
	
	// allocate memory:  return true;

	char *buffer = new char [length + 1];
	memset(buffer,0, (length + 1) * sizeof(*buffer) );
	
	// read data as a block:
	file.read (buffer,length);
	file.close(); 

	this->data = string(buffer);

	delete[] buffer;
	//cout<<this->data<<endl;  

	this->initialize(charset);
};

/////////////////////////////////////////////////////////////////////////////////////////////
xml* xml::loadFile(std::string filename, std::string charset)
{
	xml *oxml = new xml();
	//if(xml->loadFromFile(filename,charset))
	if(oxml->streamFile(filename))
	{
		return oxml;
	} 
	else
	{
		delete oxml;
		return nullptr;
	}
};

/////////////////////////////////////////////////////////////////////////////////////////////

/*
bool xml::http(string url, string charset)
{
	this->get(url);
	this->initialize(charset);

	//k_log *loger = k_log::inst();
	//loger->msg(this->data);
	//loger->free();
};
*/

/////////////////////////////////////////////////////////////////////////////////////////////
bool xml::loadFromString(std::string str, std::string charset)
{
	this->data = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" + str;
	this->initialize(charset);  
};

/////////////////////////////////////////////////////////////////////////////////////////////
bool xml::loadFromSString(const std::string &str, const std::string &charset)
{
	this->data = str;
	this->initialize(charset);  
};

/////////////////////////////////////////////////////////////////////////////////////////////
xml* xml::loadString(std::string str, std::string charset)
{
	xml *oxml = new xml();
	oxml->loadFromString(str,charset); 
	return oxml; 
};

/////////////////////////////////////////////////////////////////////////////////////////////
xml* xml::loadSString(std::string str, std::string charset)
{
	xml *oxml = new xml();
	oxml->loadFromString("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" + str,charset); 
	return oxml; 
};


/////////////////////////////////////////////////////////////////////////////////////////////
bool xml::initialize(std::string charset)
{
	this->convert(charset); 
	// this->doc = htmlReadMemory(this->data.c_str(),this->data.length(),"","UTF-8", XML_PARSE_NOWARNING | XML_PARSE_NOERROR);
	this->doc = xmlReadMemory(this->data.c_str(),this->data.length(),"","UTF-8", XML_PARSE_NOWARNING | XML_PARSE_NOERROR);
	this->XPathContext = xmlXPathNewContext(this->doc);
};

/////////////////////////////////////////////////////////////////////////////////////////////
bool xml::free(void)
{
	if(this->XPathContext != nullptr)
	{
		xmlXPathFreeContext(this->XPathContext);
		this->XPathContext = nullptr;
	}
	
	if(this->doc != nullptr)
	{
		xmlFreeDoc(this->doc);
		this->doc = nullptr;
	}
	//xmlCleanupParser();    ВЫЗЫВАТЬ В ГЛАВНОМ ПОТОКЕ ПРИ ЗАВЕРШЕНИИ РАБОТЫ
	this->data.clear();
};


/////////////////////////////////////////////////////////////////////////////////////////////
vector<string> xml::select(std::string XPath, int type) const
{
	switch(type)
	{
		case 1:
		{
			return this->select_text(XPath);
		}
		break;

		case 2:
			return this->select_text(XPath);
		break;

		case 3:
			return this->select_text(XPath);
		break;

		case 253:
		case 4:
			return this->select_html(XPath);
		break;

		case 5:
			return this->select_text(XPath);
		break;
	}
};


/////////////////////////////////////////////////////////////////////////////////////////////
std::string xml::select1(std::string XPath, int type) const
{
   std::vector<std::string> result;

   switch(type)
   {
     case 1:
       result = this->select_text(XPath);
     break;

     case 2:
       result = this->select_text(XPath);
     break;

     case 3:
       result = this->select_text(XPath);
     break;

     case 253:
     case 4:
       result = this->select_html(XPath);
     break;

     case 5:
       result = this->select_text(XPath);
     break;
   }

   if(result.size() == 1)
   {
      return result[0];
   }
   else
   {
      return "";
   }
   
};

/////////////////////////////////////////////////////////////////////////////////////////////
vector<string> xml::select_name(std::string XPath) const
{

  vector<string> values;

  xmlXPathObjectPtr XPathObj;
  XPathObj = nullptr;

  if(this->XPathContext == nullptr)
  {
     std::cout << "this->XPathContext == nullptr " << XPath << std::endl;
  }

  XPathObj = xmlXPathEvalExpression(BAD_CAST XPath.c_str(), this->XPathContext);
  if(XPathObj == 0)
  {
    // TODO: Неверный XPath
    //   записать в лог
    //   бросить исключение

    // -- k_log *loger = k_log::inst("main.log",_LOG_INFO_);
    // -- loger->msg(_LOG_ERROR_ , "XPATH " + XPath + " is not valid!",__LOCATION__);
    throw invalid_argument("некорректное XPath-выражение");
  }

  if(xmlXPathNodeSetIsEmpty(XPathObj->nodesetval))
  {
     return values; 
  }
      
  int num = XPathObj->nodesetval->nodeNr;  
       
  for(int i=0;i<num;i++)
  {       
    values.push_back( (char *) XPathObj->nodesetval->nodeTab[i]->name );
    //xmlFree(text);
  } 

  xmlXPathFreeObject(XPathObj);

  return values;
};

/////////////////////////////////////////////////////////////////////////////////////////////
std::vector<std::string> xml::select_text(std::string XPath) const
{

  //cout<<XPath<<endl;

  std::vector<std::string> values;

  xmlXPathObjectPtr XPathObj;
  XPathObj = nullptr;

  XPathObj = xmlXPathEvalExpression(BAD_CAST XPath.c_str(), this->XPathContext);
  
  // http://kapo-cpp.blogspot.ru/2008/02/working-with-xml-on-linux.html

  // -- xmlError *Err = xmlGetLastError();
  // -- cout<<"code = "<<Err->code<<"; message = "<<Err->message<<"; str1 = "<<Err->str1<<"; str2 = "<<Err->str2<<"; str3 = "<<Err->str3<<endl;

  if(XPathObj == 0)
  {
    // TODO: Неверный XPath
    //   записать в лог
    //   бросить исключение

    // --- k_log *loger = k_log::inst("main.log",_LOG_INFO_);
    // --- loger->msg(_LOG_ERROR_ , "XPATH " + XPath + " is not valid!",__LOCATION__);
    throw invalid_argument("некорректное XPath-выражение");
  }

  //cout<<"dddddd"<<endl;

  if(xmlXPathNodeSetIsEmpty(XPathObj->nodesetval))
  {
     return values; 
  }
      
  int num = XPathObj->nodesetval->nodeNr;  

  //cout<<num<<endl;
       
  for(int i=0;i<num;i++)
  {       
    xmlChar* text =  xmlNodeGetContent(XPathObj->nodesetval->nodeTab[i]);
    values.push_back( (char *) text );

    //cout<<XPathObj->nodesetval->nodeTab[i]->name<<endl;
    xmlFree(text);
  } 

  xmlXPathFreeObject(XPathObj);

  return values;
};

/////////////////////////////////////////////////////////////////////////////////////////////
std::vector<std::string> xml::select_html(std::string XPath) const
{
  // --- k_log *loger = k_log::inst("main.log",_LOG_INFO_);

  std::vector<std::string> values;

  xmlXPathObjectPtr XPathObj = xmlXPathEvalExpression(BAD_CAST XPath.c_str(), this->XPathContext);

  if(XPathObj == nullptr)
  {
    // --- loger->msg(_LOG_ERROR_ , "XPATH " + XPath + " is not valid!",__LOCATION__);
    throw invalid_argument("некорректное XPath-выражение");
  }

  if(xmlXPathNodeSetIsEmpty(XPathObj->nodesetval))
  {
     //throw invalid_argument("не выбрано ни одно значение");
     return values; 
  }

  int num = XPathObj->nodesetval->nodeNr;  
  //cout<<"num: "<<num<<endl;
       
  for(int i=0;i<num;i++)
  {       
    xmlBufferPtr psBuf;
    psBuf = xmlBufferCreate();
    xmlNodeDump(psBuf, this->doc, XPathObj->nodesetval->nodeTab[i], 1, 0);
 
    string stext = (char *) xmlBufferContent(psBuf) ;
    //loger->msg(_LOG_ERROR_ , "this is ok" + stext,__LOCATION__);
/*
Oбработка результата для случая, когда извлекаемое содержимое является значением какого-либо атрибута(т.е. вроде бы не должно содержать тэгов).
В таком случае XPath-выражение оканчивается на /@AttrName и результат имеет вид AttrName="ссылка"
*/
  
    int p = XPath.rfind("@");
    if (p != string::npos)
    {
      if ( (p > XPath.rfind("/")) && ( (XPath.rfind("]") == string::npos) || ((XPath.rfind("]") != string::npos) && (p > XPath.rfind("]")) ) ) )   //XPath оканчивается на "/@AttrName"
      {
        stext.erase(0, stext.find("\"") + 1);
        stext.erase(stext.rfind("\""));
      }
      else  //В остальных случаях в результате будут содержаться обрамляющие тэги, которые нужно убрать
      {
        stext.erase(0, stext.find(">") + 1);
        if (stext.length() != 0)
          stext.erase(stext.rfind("<"));
      }
    }
    else
    {
        stext.erase(0, stext.find(">") + 1);
        if (stext.length() != 0)
          stext.erase(stext.rfind("<"));
    }

/*
    char *text =  (char *) xmlBufferContent(psBuf) ;
    cout << "e\n";
    // ГРЕБАННЫЙ КОСТЫЛЬ!!! Надо убрать тег, обрамляющий результат

       // вырезаем начало
          char *ptr_start;
          char *ptr_end;

          ptr_start = text;
          ptr_start = strstr(text, ">");
          if(ptr_start != NULL)
          {
            ptr_start++;
          }

       // вырезаем конец      
          int len = strlen(ptr_start);
          ptr_end = ptr_start + len - 1;

          //cout<<"["<<ptr_end<<"]"<<endl;

          while(ptr_end[0] != '<')
          {
            ptr_end[0] = '\0';
            ptr_end--;
          }
          ptr_end[0] = '\0';
  
        string stext;
        stext = ptr_start;

        //cout<<stext<<endl;
*/
        //TODO:!!!! КАКОГО ХЕРА !!!!!!
        /*stext = stext.replace(stext.begin(),stext.end(),"&#13;","");
        stext = stext.replace(stext.begin(),stext.end(),"'","\\'");
        stext = stext.replace(stext.begin(),stext.end(),"&amp;nbsp","&nbsp;");*/
      
  
        values.push_back( stext );

        //cout<<fields.size()<<endl;
      
        xmlBufferFree(psBuf);
        //text = NULL; 
  } 

  xmlXPathFreeObject(XPathObj);

  return values;
};














bool xml::convert(std::string charset)
{
  //cout<<this->data<<endl;

  if(charset != "UTF-8")
  {
    iconv_t cd;
    cd = iconv_open("UTF-8",charset.c_str());
  
    size_t size_in = this->data.length();
    size_t size_out = size_in*4;
    char buf[size_out];
    memset(&buf, 0, sizeof (char) * size_out );

    char *in = const_cast<char*>(this->data.c_str());
    char *out = buf;
  
    size_t k = iconv(cd,&in,&size_in,&out,&size_out);

    this->data = buf;

    iconv_close(cd);
  }


  //cout<<this->data<<endl;
};


}