#include "../../include/core/html_parser.h"


   #include <regex>


namespace core { 

////////////////////////////////////////////////////////////////////////////////////////////

html_parser::html_parser(const std::string &contents)
{
	this->output = gumbo_parse(contents.c_str());
	this->root = this->output->root;
};

////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////////////////

html_parser::~html_parser()
{
	gumbo_destroy_output(&kGumboDefaultOptions, this->output);
};

////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////////////////

std::vector<GumboNode *> html_parser::find_child(GumboNode *node, const std::string &find_tag)
{
	std::vector<GumboNode *> v;
	GumboNode *child_node = nullptr;

	GumboVector* children = &(node->v.element.children);
	//std::cout << "children->length = " << children->length << std::endl;

	for (unsigned int i = 0; i < children->length; i++) 
	{
		child_node = static_cast<GumboNode*>(children->data[i]);
		//std::cout << "tag type = [" <<  child_node->type << "]" << std::endl; 
		if(child_node->type == GUMBO_NODE_ELEMENT)
		{
			std::string tagname(gumbo_normalized_tagname(child_node->v.element.tag));
			//std::cout << "tag = [" <<  tagname << "]" << std::endl;
			if(tagname == find_tag)
			{
				v.push_back(child_node);
				//return child_node;
			}
		}
		child_node = nullptr;
	}
	return v;
};

////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////////////////

std::string html_parser::getAttr(GumboNode *curr_node, const std::string &attr_name)
{
	GumboAttribute *className = gumbo_get_attribute( &(curr_node->v.element.attributes), attr_name.c_str());

	if(className == nullptr)
	{
		//std::cout << "FACK !!!" << std::endl;   
		return std::string();
	}
	else
	{
		return std::string(className->value);
		//std::cout << "[" << className->value << "]" << std::endl;
	}
}


std::vector<GumboNode *> html_parser::find_child_x(GumboNode *curr_node, const std::string &xpath)
{
	std::vector<GumboNode *> return_childrens;

	GumboNode *child_node = nullptr;
	GumboVector* childrens = &(curr_node->v.element.children);

	std::string find_tag;

	bool is_attr = false;
	std::string attr_name = "";
	std::string attr_value = "";

	// Проверяем, есть ли для узла условия в []
	auto pos = xpath.find("[");
	if(pos != std::string::npos)
	{
		auto len = xpath.length();
		find_tag = xpath.substr(0,pos);
		std::string cond = xpath.substr(pos+1,len-pos-2);

		std::cout << "<" << find_tag << "> <" << cond << ">" << std::endl;

		if(cond[0] == '@')
		{
			auto arr = core::explode(cond,"=");

			attr_name  = arr[0].substr(1);
			attr_value = arr[1].substr(1, arr[1].length()-2);

			std::cout << "attr_name = [" << attr_name << "] attr_value = [" << attr_value << "]" << std::endl;

			is_attr = true;
		}
	}
	else
	{
		find_tag = xpath;
	}

	// Выполняем поиск
	for (unsigned int i = 0; i < childrens->length; i++) 
	{
		child_node = static_cast<GumboNode*>(childrens->data[i]);
		if(child_node->type == GUMBO_NODE_ELEMENT)
		{
			std::string tagname(gumbo_normalized_tagname(child_node->v.element.tag));

			if(tagname == find_tag)
			{
				if(is_attr == true)
				{
				// Излекаем аттрибуты
				if(attr_value == this->getAttr(child_node,attr_name))
				{
					return_childrens.push_back(child_node);
				}
				}
				else
				{
				return_childrens.push_back(child_node);
				}
			}
		}
		else
		{
			std::cout << "tag type = [" <<  child_node->type << "]" << std::endl; 
		}

		child_node = nullptr;
	}

	return return_childrens;
}


/*std::vector<GumboNode *> html_parser::xpath_x(GumboNode *node, const std::string &xpath)
{
	std::vector<GumboNode *> find_nodes;
	GumboNode *curr_node = node;

	auto xpath_chink = core::explode(xpath, "/");

	int count = xpath_chink.size();

	for(auto i = 0; i < count-1 ; i++) 
	{
		auto v = this->find_child_x(curr_node, xpath_chink[i]);
		if()
	}

	return find_nodes;
}*/

// https://www.cyberforum.ru/blsogs/17222/blog1640.ht   for (int i = 0; i < children->length; i++) 

std::vector<GumboNode *> html_parser::xpath(GumboNode *node, const std::string &xpath)
{
	std::vector<GumboNode *> v;

	// Разбираем XPath
	auto xpath_chink = core::explode(xpath, "/");

	GumboNode *curr_node = node;
	GumboNode *child_node = nullptr;
	GumboVector* children = nullptr;

	int state = 0;
	std::string find_tag;
	bool is_cond = false;

	for(auto it = xpath_chink.begin(); it != xpath_chink.end(); it++) 
	{
		// Проверяем, есть ли для узла условия в []
		auto pos = (*it).find("[");
		if(pos != std::string::npos)
		{
			auto len = (*it).length();
			find_tag = (*it).substr(0,pos);
			std::string cond = (*it).substr(pos+1,len-pos-2);

			std::cout << "<" << find_tag << "> <" << cond << ">" << std::endl;

			if(cond[0] == '@')
			{
				auto arr = core::explode(cond,"=");

				std::string attr_name  = arr[0].substr(1);
				std::string attr_value = arr[1].substr(1, arr[1].length()-2);

				std::cout << "attr_name = [" << attr_name << "] attr_value = [" << attr_value << "]" << std::endl;

				is_cond = true;
			}
		}
		else
		{
			find_tag = (*it);
		}

		//  


		children = &(curr_node->v.element.children);

		unsigned int i = 0;
		state = 0;
		bool flag = true;
		//

		while(flag)
		{
			switch(state)
			{
				case 0:
				{
					if(i == children->length)
					{
						state = 100;
						break;
					}

					child_node = static_cast<GumboNode*>(children->data[i]);
					if(child_node->type != GUMBO_NODE_ELEMENT)
					{
						i++;  
					}
					else
					{
						std::string tagname(gumbo_normalized_tagname(child_node->v.element.tag));
						if(tagname == find_tag)
						{
							state = 200;
							break;
						}
						else
						{
							i++;
						}
					}
				}
				break;

				case 100:
				{
					// Не найден ни один подходящий дочерний элемент
					flag = false;
					return v;
				}
				break;

				case 200:
				{
					curr_node = child_node;
					flag = false;
				}
				break;
			}

		} // while(flag) 

		//std::cout << "[" << (*it) << "] state = " << state << std::endl;
		
	} // for(auto it = xpath_chink.begin(); it != xpath_chink.end(); it++) 

	//std::cout << "[end]" << std::endl;

	if(state == 200)
	{
		for (unsigned int i = 0; i < children->length; i++) 
		{
			child_node = static_cast<GumboNode*>(children->data[i]);
			//std::cout << "tag type = [" <<  child_node->type << "]" << std::endl; 
			if(child_node->type == GUMBO_NODE_ELEMENT)
			{
				std::string tagname(gumbo_normalized_tagname(child_node->v.element.tag));
				//std::cout << "tag = [" <<  tagname << "]" << std::endl;
				if(tagname == find_tag)
				{
				v.push_back(child_node);
				//return child_node;
				}
			}
			child_node = nullptr;
		}
	}

	return v;
};

////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////////////////

std::string html_parser::inner_text(GumboNode *node)
{
	GumboVector* children = &(node->v.element.children);
	if(children->length == 0)
	{
		return "";
	}
	GumboNode *child_node = static_cast<GumboNode*>(children->data[0]);
	if(child_node->type == GUMBO_NODE_TEXT)
	{
		return std::string(child_node->v.text.text);
	}
	else
	{
		return "";
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////////////////

nlohmann::json html_parser::get_links(const std::string &prefix)
{
	nlohmann::json v;
	this->search_for_links(output->root, &v, prefix);
	return v;
};

////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////////////////

void html_parser::search_for_links(GumboNode* node, nlohmann::json * links, const std::string &prefix) 
{
	if (node->type != GUMBO_NODE_ELEMENT) {
		return;
	}
	GumboAttribute* href;
	if (node->v.element.tag == GUMBO_TAG_A &&
		(href = gumbo_get_attribute(&node->v.element.attributes, "href"))) 
	{
		std::string tmp(href->value);
		if(prefix == tmp.substr(0,prefix.size()))
		{ 
			//std::cout << tmp << std::endl;
			links->push_back(tmp);
		}
	}

	GumboVector* children = &node->v.element.children;
	for (unsigned int i = 0; i < children->length; ++i) 
	{
		search_for_links(static_cast<GumboNode*>(children->data[i]),links,prefix);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////////////////

}