#ifndef H_DOCGEN_HTMLDOC
#define H_DOCGEN_HTMLDOC
// -------------------------------------------------------------------------------------- //
//                                      INCLUDES                                          //
// -------------------------------------------------------------------------------------- //

#include <string>

#include "BasicDoc.h"


namespace Docgen 
{

// -------------------------------------------------------------------------------------- //
//
//                                     HTMLDoc CLASS  
//
// -------------------------------------------------------------------------------------- //


class HTMLDoc: public BasicDoc
{
  protected:

    //  Функция создает из объекта data
    virtual string createText(Json::Value data);
    //  Функция создает html-таблицу по данным из data
    virtual string createTable(Json::Value data);
    //  Функция создает html-список по данным из data
    virtual string createList(Json::Value data);
    //  Функция изменяет маркер
    virtual string createNewMark(string mark);


  public:
  
    // ------------------------------- PUBLIC FUNCTIONS -------------------------------- //       
    
  	HTMLDoc(string, string);
  	~HTMLDoc();

    virtual Marker * findMarker() override;

    virtual Marker * findMarker2() override;

    // void dump();

};


}

#endif
