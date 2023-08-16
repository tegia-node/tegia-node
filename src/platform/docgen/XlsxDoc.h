#ifndef H_DOCGEN_XlsxDoc
#define H_DOCGEN_XlsxDoc
// -------------------------------------------------------------------------------------- //
//                                      INCLUDES                                          //
// -------------------------------------------------------------------------------------- //

#include <string>
#include <vector>
#include <unistd.h> //chdir

#include "BasicDoc.h"

namespace Docgen 
{

  /* ПРимер отправки сообщения
    
  std::shared_ptr<message_t2> genMessage(new message_t2("platform://generate", "/generate", core::cast<Json::Value>("{}")));

  genMessage->root["gendoc"]["template"] = "second.docx";
  genMessage->root["gendoc"]["gen_name"] = "second_try.docx";
  genMessage->root["gendoc"]["data"]["title"] = "КОВРИЖКА";
  genMessage->root["gendoc"]["data"]["date"] = "18 июня 2033 года";
  
  genMessage->type = "/generate";
  genMessage->recipient = "platform://generate";  
  Platform::Node::instance()->SendMessage(genMessage->recipient, genMessage);
  */


// -------------------------------------------------------------------------------------- //
//
//                                     XlsxDoc CLASS  
//
// -------------------------------------------------------------------------------------- //


class XlsxDoc: public BasicDoc
{
  protected:

    string _template_dir;

    // Функция создает строку из объекта data
    virtual string createText(Json::Value data)
    {
      return data.asString();
    };

    // Функция создает таблицу по данным из data
    virtual string createTable(Json::Value data)
    {
      return "";
    };

    // Функция создает нумерованный список по данным из data
    virtual string createList(Json::Value data)
    {
      return "";
    };

    // Функция создает маркированный список по данным из data
    virtual string createNewMark(string mark)
    {
      return "<!-- " + mark + " -->";
    };

  public:
  
    // ------------------------------- PUBLIC FUNCTIONS -------------------------------- //       
    
	XlsxDoc(string file_name, string template_dir, string content):BasicDoc(file_name, content)
  {
    this->_template_dir = template_dir;
   
  };


  ~XlsxDoc()
  {
  };

  Marker * findMarker()
  {
    string marker = "";
    string marker_action = "";
    string marker_component = "";
    vector<string> marker_args;

    size_t marker_content_start = 0;
    size_t marker_content_end = 0;


    size_t marker_start = this->_content.find("<!-- SET{{", _last_processed_mark_pos);
    if(marker_start != string::npos)
    {
      size_t marker_end = this->_content.find("}} -->", marker_start);
      marker = this->_content.substr(marker_start + 10, marker_end - marker_start - 10);
      auto marker_params = core::explode(marker, "|",false);
          
      marker_action = marker_params.at(0);
      marker_component = marker_params.at(1);

      for (int marker_param_index = 2; marker_param_index < marker_params.size(); marker_param_index++)
      {
        marker_args.push_back(marker_params[marker_param_index]);
      }

      this->_last_processed_mark_pos = this->_content.find("</t>", marker_end);
      
      marker_content_start = marker_end + 6;
      marker_content_end = this->_last_processed_mark_pos;

      return new Marker(this, marker_action, marker_component, marker_content_start, marker_content_end - marker_content_start, marker_args);
    }

    return nullptr;      
  };

  Marker * findMarker2()
  {
    string marker = "";
    string marker_action = "";
    string marker_component = "";
    vector<string> marker_args;

    size_t marker_content_start = 0;
    size_t marker_content_end = 0;

    marker_content_start = this->_content.find("SET{{", _last_processed_mark_pos);
    if(marker_content_start != string::npos)
    {
      marker_content_end = this->_content.find("}}", marker_content_start) + 2;
      marker = this->_content.substr(marker_content_start, marker_content_end - marker_content_start);
      
      marker_args.push_back(marker);

      this->_last_processed_mark_pos = marker_content_end;
      
      return new Marker(this, marker_content_start, marker_content_end - marker_content_start, marker_args);
    }

    return nullptr;      
  };


  string getTemplateDir()
  {
    return this->_template_dir;
  };
};

template <>
XlsxDoc * loadDocument<XlsxDoc>(const std::string & template_file_name)
{
  XlsxDoc * document = nullptr;
	string template_directory = tegia::random::uuid();

	int sys = system(("unzip -x -o -q templates/" + template_file_name + " -d tmp/" + template_directory).c_str());
	if (sys != 0)
	{
  	LDEBUG("UNZIP-error(code " + to_string(sys) + "); template file: " + template_file_name);
	}

	ifstream template_to_read(("tmp/" + template_directory + "/xl/sharedStrings.xml").c_str());
	if (!template_to_read.fail())
  {
		char c;
		string file_content = "";

		while(template_to_read.get(c))
		{
		  file_content += c;
		};

    document = new XlsxDoc(template_file_name, template_directory, file_content);
	}
  
  template_to_read.close();

  return document;
};



template <>
void saveDocument<XlsxDoc>(XlsxDoc * document, const std::string & document_name)
{
 	LDEBUG("saveDocument");
  ofstream file_to_write (("tmp/" + document->getTemplateDir() + "/xl/sharedStrings.xml").c_str());
  file_to_write << document->getContent();
  file_to_write.close();

	int sys;
	sys = chdir(("tmp/" +  document->getTemplateDir()).c_str());

	sys = system(("zip -r -q ../../generated/" + document_name + " ./").c_str());
	if (sys != 0)
	{
	 LDEBUG("Folder tmp/" + document->getTemplateDir() + " : ZIP-error(code " + to_string(sys) + ").");
	}
	else
	{
  	LDEBUG("Generate " + document_name + " success");
  	cout << "Generate " << document_name << " success"<<endl;
	}
  // system("cp xl/sharedStrings.xml ../../generated/sharedStrings.xml");


	sys = chdir("../../");
	sys = system(("rm -r tmp/" + document->getTemplateDir()).c_str());
};

};

#endif
