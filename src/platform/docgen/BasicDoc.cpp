#include "BasicDoc.h"

/** ---------------------------------------------------------------------------------------- 
    \brief   Пространство имен для классов генерации документов
    \details 
    \authors Букин Николай
 */
namespace Docgen 
{

// -------------------------------------------------------------------------------------- //
//
//                                     Marker CLASS  
//
// -------------------------------------------------------------------------------------- //

Marker::Marker(BasicDoc * document, string action, string component, size_t content_start, size_t content_length, vector<string> args):
	_document(document),
	_action(action),
	_component(component),
	_content_start(content_start),
	_content_length(content_length),
	_args(args),
	_data(Json::nullValue)
{

};

Marker::Marker(BasicDoc * document, size_t content_start, size_t content_length, vector<string> args):
	_document(document),
	_action(""),
	_component(""),
	_content_start(content_start),
	_content_length(content_length),
	_args(args),
	_data(Json::nullValue)
{

};

Marker::~Marker()
{

};

vector<string> Marker::getArgs()
{
	return this->_args;
};

void Marker::setData(Json::Value data)
{
	this->_data = data;

	this->_document->write(this);
};


void Marker::print()
{
	string 	out = "--------- MARKER -----------\n";
			out += "_action = " + _action + "\n";
			out += "_component = " + _component + "\n";
			out += "_content_start = " + to_string(_content_start) + "\n";
			out += "_content_length = " + to_string(_content_length) + "\n";
			out += "_ARGS : [\n";
			for (const string &argument : _args)
			{
				out += "\t" + argument + "\n";
			}
			out += "]";

	std::cout << out << std::endl;
};

std::string Marker::getAction()
{
	return this->_action;
};






// -------------------------------------------------------------------------------------- //
//
//                                     BasicDoc CLASS  
//
// -------------------------------------------------------------------------------------- //

BasicDoc::BasicDoc(string file_name, string content):
	_file_name(file_name),
	_content(content),
	_last_processed_mark_pos(0)
{
	
};

BasicDoc::~BasicDoc()
{

};

string BasicDoc::getContent() const
{
	return this->_content;
};


string BasicDoc::getTemplateDir() const
{
	return "";
};



void BasicDoc::write(Marker * marker)
{
	string formatted_data = "";


	//	TODO: заменить на делегат
	if (marker->_component == "text")
	{
		formatted_data = this->createText(marker->_data);
	}

	if (marker->_component == "table")
	{
		formatted_data = this->createTable(marker->_data);
	}

	if (marker->_component == "list")
	{
		formatted_data = this->createList(marker->_data);
	}
	
	size_t formatted_data_length = formatted_data.length();
	
	// std::cout << "replace call()\n";
	// LDEBUG("marker->_component: " + marker->_component + " formatted_data: " + formatted_data);
	this->_content.replace(marker->_content_start, marker->_content_length, formatted_data);
	
	//	Т. к. после подставновки новых данных изменилась длина документа, нужно вычислить новую позицию, с которой будет осуществляться поиск следующего маркера
	this->_last_processed_mark_pos += formatted_data_length - marker->_content_length;
};

void BasicDoc::replaceMark(Marker * marker)
{
	auto marker_args = marker->getArgs();
	string mark = this->createNewMark(marker_args.at(0));

	this->_content.replace(marker->_content_start, marker->_content_length, mark);
	
	//	Т. к. после подставновки новых данных изменилась длина документа, нужно вычислить новую позицию, с которой будет осуществляться поиск следующего маркера
	this->_last_processed_mark_pos += mark.length() - marker->_content_length;
};

};
