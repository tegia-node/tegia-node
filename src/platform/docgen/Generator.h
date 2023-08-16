#ifndef H_DOCGEN_GENERATOR
#define H_DOCGEN_GENERATOR

#include "BasicDoc.h"

/** ---------------------------------------------------------------------------------------- 
    \brief   Пространство имен для классов генерации документов
    \details 
    \authors Букин Николай
 */
namespace Docgen
{


class Generator
{
protected:
	Json::Value processAction(const string & action, const vector<string> & action_args, Json::Value data);
	
public:
	Generator();
	~Generator();

	void generate(BasicDoc * document, Json::Value data);
	void prepare(BasicDoc * document);
};

/* СООБЩЕНИЕ ДЛЯ ВЫЗОВА ПОДГОТОВКИ ШАБЛОНА

	std::shared_ptr<message_t2> genMessage(new message_t2("platform://generate", "/prepare", core::cast<Json::Value>("{}")));

	genMessage->root["gendoc"]["template"] = "tmp2.xlsx";
	genMessage->root["gendoc"]["gen_name"] = "../templates/tmp2.xlsx";

	genMessage->type = "/prepare";
	genMessage->recipient = "platform://generate";  
	Platform::Node::instance()->SendMessage(genMessage->recipient, genMessage);
*/


};
#endif