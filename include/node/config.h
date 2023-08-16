#ifndef H_TEGIA_PLATFORM_CONFIG
#define H_TEGIA_PLATFORM_CONFIG

////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// INCLUDES                                                                               //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

#include <tegia/const.h>
#include <tegia/core/string.h>
#include <tegia/core/json.h>

////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// DEFINES                                                                                //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// CONFIG CLASS                                                                           //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////


namespace tegia {

class config
{
	protected:  
		std::string filename;
		std::string name;
		nlohmann::json data; 

		bool load();


	public: 
		config(const std::string &name, const std::string &filename); 
		~config();

		const nlohmann::json * operator[] (const std::string & name) const;
		const nlohmann::json * operator[] (nlohmann::json::json_pointer ptr) const;

}; // end class config


}  // end namespace tegia


#endif