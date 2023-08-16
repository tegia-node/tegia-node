#ifndef H_DOCGEN_WORDDOC
#define H_DOCGEN_WORDDOC
// -------------------------------------------------------------------------------------- //
//                                      INCLUDES                                          //
// -------------------------------------------------------------------------------------- //

#include <string>
#include <vector>
#include <algorithm>
#include <unistd.h> //chdir

#include "BasicDoc.h"
#include "../k_xml.h"


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
//                                     WordDoc CLASS  
//
// -------------------------------------------------------------------------------------- //


class WordDoc: public BasicDoc
{
  protected:

    string _template_dir;
    string formatter;

    // //  Функция создает из объекта data
    virtual string createText(Json::Value data)
    {
      return "<w:r>" + this->formatter + "<w:t>" + data.asString() + "</w:t></w:r>";
    };

    // //  Функция создает html-таблицу по данным из data
    virtual string createTable(Json::Value data)
    {
      return "";
    };

    // //  Функция создает html-список по данным из data
    virtual string createList(Json::Value data)
    {
      return "";
    };

    virtual string createNewMark(string mark)
    {
      return "";
    };

  public:
  
    // ------------------------------- PUBLIC FUNCTIONS -------------------------------- //       
    
	WordDoc(string file_name, string template_dir, string content):BasicDoc(file_name, content)
  {
    this->_template_dir = template_dir;
   
  };


  ~WordDoc()
  {
  };

  Marker * findMarker()
  {
    LDEBUG("FINDMARKER");
    string marker = "";
    string marker_action = "";
    string marker_type = "";
    string marker_component = "";
    vector<string> marker_args;

    size_t marker_content_start = 0;
    size_t marker_content_end = 0;

    string begin = "<w:fldChar w:fldCharType=\"begin\"/>";
    string end = "<w:fldChar w:fldCharType=\"end\"/>";

    size_t marker_start_begin = this->_content.find(begin, _last_processed_mark_pos);
    if (marker_start_begin != string::npos)
    {
      size_t begin_tree = this->_content.rfind("<w:r ", marker_start_begin);
      size_t marker_start_end = this->_content.find(end, _last_processed_mark_pos);
      size_t end_tree = this->_content.find("</w:r>", marker_start_end); 
      size_t end_wr = this->_content.find(">", begin_tree) + 1; // 1 - >
      // Вырезаем форматирование
      this->formatter = this->_content.substr(end_wr,  marker_start_begin - end_wr);
      // LDEBUG("FORMATTER: " + this->formatter);
      // Вырезаем дерево, в котором находится маркер
      string tree_str = this->_content.substr(begin_tree, end_tree - begin_tree);
      // LDEBUG("TREE: \n" + tree_str);
      
      k_xml* tree = new k_xml;

      std::transform(tree_str.begin(), tree_str.end(), tree_str.begin(), ::tolower);

      // LDEBUG("TREEtran: \n" + tree_str);
      tree->loadFromString("<root>" + tree_str + "</root>");
      vector<string> insertText = tree->select("//instrtext", 1);

      // Восстанавливаем маркер
      for(int i = 0; i < insertText.size(); i++)
      {
        marker += insertText.at(i);
      }

      tree->free();
      delete tree;

      size_t pos = marker.find("{{");
      marker = marker.substr(pos + 2);
      pos = marker.rfind("}}");
      marker = marker.substr(0, pos);
     // LDEBUG("marker: " + marker);

      auto marker_params = core::explode(marker, "|", false);
        
      marker_action = marker_params.at(0);
      marker_type = marker_params.at(1);
      marker_component = marker_params.at(2);

      for (int marker_param_index = 3; marker_param_index < marker_params.size(); marker_param_index++)
      {
        marker_args.push_back(marker_params[marker_param_index]);
      }

      // Ищем окончание маркера
      size_t marker_finish_start = this->_content.find(begin, end_tree);
      if (marker_start_begin != string::npos)
      {
        size_t marker2_finish_begin = this->_content.find("{{", marker_finish_start);
        size_t marker2_finish_end = this->_content.find("}}", marker_finish_start);

        // Если нет маркера окончания, то ошибка
        if(marker2_finish_begin == string::npos || marker2_finish_end == string::npos)
          return nullptr;      

        // LDEBUG("END MARKER " + this->_content.substr(marker2_finish_begin + 2, marker2_finish_end - (marker2_finish_begin + 2)))
        auto marker2_params = core::explode(this->_content.substr(marker2_finish_begin + 2, marker2_finish_end - (marker2_finish_begin + 2)), "|",false);
        
        string marker2_action = marker2_params.at(0);
        string marker2_type = marker2_params.at(1);

        // LDEBUG("ALL marker_action " + marker_action + " marker2_action " + marker2_action + " marker_type " + marker_type + " marker2_type " + marker2_type);
        // Получаем информацию необходимую для вставки в маркер
        size_t marker_finish_end = this->_content.find(end, marker2_finish_end); 
        marker_content_start = end_tree + 6;// 6 - </w:r>
        marker_content_end = this->_content.rfind("<w:r ", marker_finish_start);
        this->_last_processed_mark_pos = _content.find("</w:r>", marker_finish_end);

        // Проверяем валидность маркера
        if(marker_action == marker2_action && marker_type == "start" && marker2_type == "end")
          return new Marker(this, marker_action, marker_component, marker_content_start, marker_content_end - marker_content_start, marker_args);
      }
      return nullptr;      
    }

    return nullptr;      
  };

  Marker * findMarker2()
  {
    return nullptr;      
  };

  string getTemplateDir()
  {
    return this->_template_dir;
  };

};

template <>
WordDoc * loadDocument<WordDoc>(const std::string & template_file_name)
{
  WordDoc * document = nullptr;
  
  string template_directory = tegia::random::uuid();

  int sys = system(("unzip -x -o -q templates/" + template_file_name + " -d tmp/" + template_directory).c_str());
  if (sys != 0)
  {
      LDEBUG("UNZIP-error(code " + to_string(sys) + "); template file: " + template_file_name);
  }

  ifstream template_to_read(("tmp/" + template_directory + "/word/document.xml").c_str());
  if (!template_to_read.fail())
  {
    char c;
    string file_content = "";

    while(template_to_read.get(c))
    {
      file_content += c;
    };

    document = new WordDoc(template_file_name, template_directory, file_content);
  }

  template_to_read.close();

  return document;
};



template <>
void saveDocument<WordDoc>(WordDoc * document, const std::string & document_name)
{
  // LDEBUG("saveDocument");
  ofstream file_to_write (("tmp/" + document->getTemplateDir() + "/word/document.xml").c_str());
  file_to_write << document->getContent();
  file_to_write.close();

  int sys;
  sys = chdir(("tmp/" +  document->getTemplateDir()).c_str());

  //system("cp word/document.xml ../../generated/doc.xml");

  sys = system(("zip -r -q ../../generated/" + document_name + " ./").c_str());
  if (sys != 0)
  {
    LDEBUG("Folder tmp/" + document->getTemplateDir() + " : ZIP-error(code " + to_string(sys) + ").");
  } else
  {
    LDEBUG("Generate " + document_name + " success");
    cout << "Generate " << document_name << " success"<<endl;
  }
  sys = chdir("../../");
  sys = system(("rm -r tmp/" + document->getTemplateDir()).c_str());
};

};

#endif
