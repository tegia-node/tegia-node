#ifndef _H_CORE_JSON_
#define _H_CORE_JSON_
// --------------------------------------------------------------------

//	C++ STL
	#include <fstream>
	#include <filesystem> 

//	VENDORS 
	// #define JSON_DIAGNOSTICS 1  // https://json.nlohmann.me/home/exceptions/#extended-diagnostic-messages
	#include <nlohmann/json.hpp>
	#include <nlohmann/json-schema.hpp>


	#define JSON_FILE_NOT_FOUND		10
	#define JSON_FILE_PARSE_ERROR	11
	#define JSON_FILE_OUT_OF_RANGE	12

namespace core {
namespace json {

	class error
	{
		public:
			int type = 0;
			std::string filename = "";
			std::string message = "";

			error(){};
			error(int type, const std::string &filename, const std::string &message)
			{
				this->type = type;
				this->filename = filename;
				this->message = message;
			};
			~error(){};
	};

   nlohmann::json file(const std::string &filename, bool is_null = true);
   nlohmann::json file(const std::string &filename, core::json::error *error);
   
   bool save(const std::string &filename, const nlohmann::json &data,const int  indent = -1, const char  indent_char = ' ');
   nlohmann::json convert_xml(const std::string &xml_string);

} // END namespace json
} // END namespace core


#endif