#ifndef H_DOCGEN_BASICDOC
#define H_DOCGEN_BASICDOC
// -------------------------------------------------------------------------------------- //
//                                      INCLUDES                                          //
// -------------------------------------------------------------------------------------- //

  #include <string>
  #include <fstream>
  #include <vector>
  #include <iostream>


  #include "../jsonextension.h"
  #include "../core.h"

  using namespace std;


/** ---------------------------------------------------------------------------------------- 
    \brief   Пространство имен для классов генерации документов
    \details 
    \authors Букин Николай
 */
namespace Docgen {



/** ----------------------------------------------------------------------------------------
       \fn      template<class DocObj> DocObj * load(std::string file_name)
       \brief   Функция загрузки в документа в память
       \authors Букин Николай
       \details Функция пытается прочитать файл как текстовый. 
                Если нужно осуществлять более хитое чтение(документа Word, например), то требуется конкретизировать данную функцию для класса, представляющего этот новый документ
  */
template <class DocObj>
DocObj * loadDocument(const std::string & template_file_name)
{
  DocObj * document = nullptr;

  ifstream template_to_read(template_file_name.c_str());
  if (!template_to_read.fail())
  {
    char c;
    string file_content = "";

    while(template_to_read.get(c))
    {
      file_content += c;
    };

    document = new DocObj(template_file_name, file_content);
  }

  template_to_read.close();
  return document;
};


/** ----------------------------------------------------------------------------------------
       \fn      void saveDocument(DocObj * document, const std::string & document_name)
       \brief   Функция сохранения документа на диск
       \authors Букин Николай
       \details Записывает документ как текстовый файл. Если нужно осуществлять более хитрую запись, то нужно конкретизировать эту функцию классом записываемого документа
  */
template <class DocObj>
void saveDocument(DocObj * document, const std::string & document_name)
{
  ofstream file_to_write(document_name.c_str());
  file_to_write << document->getContent();
  file_to_write.close();
}








class BasicDoc;

/** ----------------------------------------------------------------------------------------
    \brief   Класс, предоставляющий абстракцию для маркера
    \authors Букин Николай
    \details Маркер служит для вставки данных в документ
*/
class Marker
{
  friend class BasicDoc;

  protected:
  
    BasicDoc * _document;

    string _action;
    string _component; // "table", "list", "text"

    size_t _content_start;
    size_t _content_length;

    vector<string> _args;

    Json::Value _data;
  

  public:
  
  /** ----------------------------------------------------------------------------------------
       \brief   Конструктор
       \authors Букин Николай
       \details    

       \param document          ссылка на документ, в котором находится маркер
       \param action            тип маркера
       \param component         форма представления данного маркера в документе - текст, таблица, список
       \param content_start     позиция, начиная с которой в документе храниться содержимое маркера
       \param content_length    длина содержмого маркера в символах
       \param args              вектор аргументов маркера

  */ 
  Marker(BasicDoc * document, string action, string component, size_t content_start, size_t content_length, vector<string> args);
  
  /** ----------------------------------------------------------------------------------------
         \brief   Конструктор
         \authors Корастелёв Вадим
         \details    

         \param document          ссылка на документ, в котором находится маркер
         \param content_start     позиция, начиная с которой в документе храниться содержимое маркера
         \param content_length    длина содержмого маркера в символах
         \param args              вектор аргументов маркера

  */ 
  Marker(BasicDoc * document, size_t content_start, size_t content_length, vector<string> args);
  ~Marker();

  /** ----------------------------------------------------------------------------------------
       \fn      void setData(Json::Value data);
       \brief   Помещает данные в маркер
       \authors Букин Николай
       \details    
  */ 
  void setData(Json::Value data);


  /** ----------------------------------------------------------------------------------------
       \fn      void print();
       \brief   Печатает в консоль содержимое ключевых полей маркера
       \authors Букин Николай
       \details Используется для отладки
  */ 
  void print();


  /** ----------------------------------------------------------------------------------------
       \fn      vector<string> getArgs();
       \brief   Возвращает вектор аргументов маркера
       \authors Букин Николай
       \details Используется Генератором
  */
  vector<string> getArgs();

  /** ----------------------------------------------------------------------------------------
       \fn      string getAction();
       \brief   Возвращает тип маркера
       \authors Букин Николай
       \details Используется Генератором
  */
  string getAction();

};




/** ----------------------------------------------------------------------------------------
    \brief   Базовый класс для представления генерируемых документов
    \authors Букин Николай
    \details 
*/
class BasicDoc
{
  //friend class Marker;

  protected:
  
    string _file_name;
    string _content;

    size_t _last_processed_mark_pos;

    // ----------------------------- PROTECTED FUNCTIONS ------------------------------- //       

    //  Функция создает строку, содержащую текст из data, соотвествующую данному формату документа
    virtual string createText(Json::Value data) = 0;
    //  Функция создает таблицу по данным из data, соотвествующую данному формату документа
    virtual string createTable(Json::Value data) = 0;
    //  Функция создает список по данным из data, соответствующий данному формату документа
    virtual string createList(Json::Value data) = 0;
    //  Функция создает новую метку
    virtual string createNewMark(string mark) = 0;


  public:

    BasicDoc(string file_name, string content);
    virtual ~BasicDoc();

    string getContent() const;

    string getTemplateDir() const;

    //  Возвращает первый необработанный маркер документа, используется при ГЕНЕРАЦИИ файла по шаблону
    virtual Marker * findMarker() = 0;

    //  Возвращает первый необработанный маркер документа, используется при ПОДГОТОВКЕ шаблона
    virtual Marker * findMarker2() = 0;

    //  Записывает данные, помещенные в маркер, в документ
    virtual void write(Marker * marker);

    //  Изменяет маркер в документе
    virtual void replaceMark(Marker * marker);

};




};

#endif
