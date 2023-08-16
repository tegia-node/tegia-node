#include "../../include/core/html_parser2.h"

namespace core {  
namespace html { 


////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////////////////


node::node(xmlNodePtr libxml_node)
{
   this->libxml_node = libxml_node;
};


node::node()
{ };



std::string node::get_attr(const std::string &name)
{
	std::string ret_val = ""; // std::string();

	xmlAttr* attribute = this->libxml_node->properties;
	while(attribute)
	{
		xmlChar* value = xmlNodeListGetString(this->libxml_node->doc, attribute->children, 1);

		// std::cout << (const char*) attribute->name << std::endl;

		if(strcmp( (const char*) attribute->name, name.c_str() ) == 0 )
		{
		   ret_val = std::string( (char*)value );  
		   xmlFree(value); 
		   return ret_val;	
		}

		xmlFree(value); 
		attribute = attribute->next;
	}

	return ret_val;
};



std::string node::get_text()
{
	xmlChar *text = xmlNodeGetContent(this->libxml_node);
	return std::string( ( char*) text );
};



std::string node::get_name()
{
	return std::string( ( char*) this->libxml_node->name );
};


int node::child_count()
{
	return xmlChildElementCount(this->libxml_node);
};


//node node::child(const std::string &nodename, int index)
//{
	/*xmlNodePtr _node = this->libxml_node->children;
	const xmlChar * _nodename = (const xmlChar *) nodename.c_str();

	while(_node != nullptr)
	{

		_node = this->libxml_node->next;		

		if(_node == this->libxml_node->last)
		{
			_node = nullptr;
		}
	}*/

   //return std::string( ( char*) this->libxml_node->name );
//};


/*
std::string node::dump()
{
	xmlBufferPtr buf;
	buf = xmlBufferCreate();

	xmlBufNodeDump(buf,this->html,this->libxml_node,1,1);
   std::string _ret = std::string( (char*) buf->content );
	xmlBufferFree(buf);

	return _ret;

};
*/


////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////////////////


nodes::~nodes()
{
	/* Cleanup */
	xmlXPathFreeObject(this->xpathObj);
	xmlXPathFreeContext(this->xpathCtx);
};


node nodes::operator[] (const int index)
{
	node _ret_node;
	_ret_node.libxml_node = this->xpathObj->nodesetval->nodeTab[index];
	return _ret_node;
};


int nodes::count()
{
	int size = (this->xpathObj->nodesetval) ? this->xpathObj->nodesetval->nodeNr : 0;
	return size;
};


////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////////////////

parser::parser()
{

};


parser::parser(const std::string &content)
{
	this->html = htmlReadMemory(content.c_str(), content.length(), nullptr, nullptr, 1);

	if(this->html == nullptr)
	{
		std::cout << "html parse error" << std::endl;
	}
};


void parser::parseXML(const std::string &content)
{
	this->html = xmlReadMemory(content.c_str(), content.length(), nullptr, nullptr, 1);

	if(this->html == nullptr)
	{
		std::cout << "html parse error" << std::endl;
	}
};

////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////////////////


parser::~parser()
{
	xmlFreeDoc(this->html); 
};


////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////////////////

// http://www.xmlsoft.org/examples/xpath1.c

nodes parser::xpath(const std::string &xpath, nlohmann::json ns)
{
	nodes _ret;

	const xmlChar* xpathExpr = (const xmlChar *) xpath.c_str();

	/* Create xpath evaluation context */
	_ret.xpathCtx = xmlXPathNewContext(this->html);
	if(_ret.xpathCtx == NULL)
	{
		std::cout << _ERR_TEXT_ << "Error: unable to create new XPath context\n";
		//fprintf(stderr,"Error: unable to create new XPath context\n");
		//xmlFreeDoc(doc); 
		//return(-1);
		exit(0);
	}

	//
	if(ns.is_array())
	{
		for(auto it = ns.begin(); it != ns.end(); it++)
		{
			std::string ns_name = (*it)["name"].get<std::string>();
			std::string ns_path = (*it)["path"].get<std::string>();
			xmlXPathRegisterNs(_ret.xpathCtx, BAD_CAST ns_name.c_str(), BAD_CAST ns_path.c_str());
		}
	}
	
	//xmlXPathRegisterNs(_ret.xpathCtx,  BAD_CAST "xdr", BAD_CAST "http://schemas.openxmlformats.org/drawingml/2006/spreadsheetDrawing");
	//xmlXPathRegisterNs(_ret.xpathCtx,  BAD_CAST "a", BAD_CAST "http://schemas.openxmlformats.org/drawingml/2006/main");

	/* Evaluate xpath expression */
	_ret.xpathObj = xmlXPathEvalExpression(xpathExpr, _ret.xpathCtx);
	if(_ret.xpathObj == NULL)
	{
		std::cout << _ERR_TEXT_ << "Error: unable to evaluate xpath expression " << xpathExpr << "\n";
		// fprintf(stderr,"Error: unable to evaluate xpath expression \"%s\"\n", xpathExpr);
		xmlXPathFreeContext(_ret.xpathCtx); 
		//xmlFreeDoc(doc); 
		//return(-1);
		exit(0);
	}

	//_ret.libxml_nodes = xpathObj->nodesetval;

	/* Cleanup */
	//xmlXPathFreeObject(xpathObj);
	//xmlXPathFreeContext(xpathCtx);

	return _ret;
};


////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////////////////





} // END namespace html
} // END namespace core