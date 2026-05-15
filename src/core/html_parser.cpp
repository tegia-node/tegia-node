#include <tegia/core/html_parser.h>

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
    xmlChar* raw = xmlNodeGetContent(this->libxml_node);
    if (!raw) return "";

    std::string result((char*)raw);
    xmlFree(raw);
    return result;
};


std::string node::get_html()
{
	xmlBufferPtr buffer = xmlBufferCreate();
    if (!buffer) return "";

    // Сериализация узла (включая сам тег)
    htmlNodeDump(buffer, this->libxml_node->doc, this->libxml_node);

    std::string result((const char*)buffer->content);

    xmlBufferFree(buffer);
    return result;
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

nodes::nodes(nodes&& other) noexcept
{
    this->xpathCtx = other.xpathCtx;
    this->xpathObj = other.xpathObj;

    other.xpathCtx = nullptr;
    other.xpathObj = nullptr;
};


nodes& nodes::operator=(nodes&& other) noexcept
{
    if (this != &other)
    {
        // сначала освобождаем свои ресурсы
        if (this->xpathObj)
            xmlXPathFreeObject(this->xpathObj);

        if (this->xpathCtx)
            xmlXPathFreeContext(this->xpathCtx);

        // забираем ресурсы
        this->xpathCtx = other.xpathCtx;
        this->xpathObj = other.xpathObj;

        // обнуляем источник
        other.xpathCtx = nullptr;
        other.xpathObj = nullptr;
    }
    return *this;
};


nodes::~nodes()
{
	/* Cleanup */
	xmlXPathFreeObject(this->xpathObj);
	xmlXPathFreeContext(this->xpathCtx);
};


node nodes::operator[] (const int index)
{
	// std::cout << "index = " << index << std::endl;

	if (this->xpathObj == nullptr)
	{
		std::cout << _ERR_TEXT_ << "this->xpathObj == nullptr" << std::endl;
		exit(0);
	}

	if (this->xpathObj->nodesetval == nullptr)
	{
		std::cout << _ERR_TEXT_ << "this->xpathObj->nodesetval == nullptr" << std::endl;
		exit(0);
	}

	auto* set = xpathObj->nodesetval;
	if (index < 0 || index >= set->nodeNr)
	{
		std::cout << _ERR_TEXT_ << "index < 0 || index >= set->nodeNr" << std::endl;
		exit(0);
	}

	node _ret_node;
	_ret_node.libxml_node = set->nodeTab[index];
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


void silentErrorHandler(void* ctx, const char* msg, ...)
{
    // ничего не делаем
}



parser::parser()
{

};


parser::parser(const std::string &content)
{
	xmlSetGenericErrorFunc(nullptr, silentErrorHandler);
	
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

	/* Create xpath evaluation context */
	_ret.xpathCtx = xmlXPathNewContext(this->html);
	if(_ret.xpathCtx == NULL)
	{
		std::cout << _ERR_TEXT_ << "Error: unable to create new XPath context\n";
		exit(0);
	}

	//
	if(ns.is_array())
	{
		for (const auto& item : ns) 
		{
			std::string ns_name = item["name"].get<std::string>();
			std::string ns_path = item["path"].get<std::string>();
			xmlXPathRegisterNs(_ret.xpathCtx, BAD_CAST ns_name.c_str(), BAD_CAST ns_path.c_str());
		}
	}
	
	/* Evaluate xpath expression */
	_ret.xpathObj = xmlXPathEvalExpression(BAD_CAST xpath.c_str(), _ret.xpathCtx);
	if(_ret.xpathObj == NULL)
	{
		std::cout << _ERR_TEXT_ << "Error: unable to evaluate xpath expression " << (const xmlChar *) xpath.c_str() << "\n";
		xmlXPathFreeContext(_ret.xpathCtx); 
		exit(0);
	}

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