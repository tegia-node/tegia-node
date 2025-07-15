#ifndef H_TEGIA_CORE_HTML
#define H_TEGIA_CORE_HTML

//
//
//

#include <tegia/tegia.h>

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#include <libxml/HTMLparser.h>
#include <libxml/HTMLtree.h>

//
//
//

namespace tegia { 
namespace html { 

	
class document_t
{
	public: 
		document_t();
		~document_t();

		bool load(const std::string& html);
		std::string text();

	private:
		bool _is_load = false;
		htmlDocPtr _doc;
};


} // END namespace html
} // END namespace tegia


#endif



