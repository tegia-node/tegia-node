#include <iostream>
#include <tegia/core/const.h>

#include <tegia/dictionaries/index.h>
#include "catalog.h"


/////////////////////////////////////////////////////////////////////////////////////////////////
/*

	function tegia::dictionaries::index()

*/
/////////////////////////////////////////////////////////////////////////////////////////////////


namespace tegia {
namespace dictionaries {


index_t * index(const std::string &dictionary, const std::string &index)
{
	auto catalog = tegia::dictionaries::catalog_t::instance();
	
	if(catalog->status() != 200)
	{
		std::cout << _ERR_TEXT_ << "catalog->status = " << catalog->status() << std::endl;
		return nullptr;
	}

	return catalog->index(dictionary,index);
};


} // END namespace dictionaries
} // END namespace tegia


/////////////////////////////////////////////////////////////////////////////////////////////////
/*

	tegia::dictionaries::index_t

*/
/////////////////////////////////////////////////////////////////////////////////////////////////


namespace tegia {
namespace dictionaries {


std::tuple<bool,nlohmann::json> index_t::find(const std::string &index)
{
	auto pos = this->_index.find(index);
	if(pos == this->_index.end())
	{
		return std::make_tuple(false,nullptr);
	}

	return std::make_tuple(true,*pos->second);
};


} // END namespace dictionaries
} // END namespace tegia




