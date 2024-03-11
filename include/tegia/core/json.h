#ifndef H_TEGIA_CORE_JSON
#define H_TEGIA_CORE_JSON
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

namespace tegia {
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

	[[deprecated("use tegia::json::_file()")]]
	nlohmann::json file(const std::string &filename, bool is_null = true);
	
	[[deprecated("use tegia::json::_file()")]]
	nlohmann::json file(const std::string &filename, tegia::json::error *error);

	std::tuple<int, std::string, nlohmann::json> _file(const std::string &filename);
   
	bool save(const std::string &filename, const nlohmann::json &data, const int  indent = -1, const char  indent_char = ' ');
	nlohmann::json convert_xml(const std::string &xml_string);

} // END namespace json
} // END namespace tegia


namespace tegia {
namespace json {

class validator
{
	public:
		validator() = default;
		~validator() = default;

		bool is_load();
		int  load(const std::string &name, const std::string &filename);
		int  validate(nlohmann::json * data);

	private:
		bool _is_load = false;
		std::string _name = "";
		std::string _filename = "";
		nlohmann::json_schema::json_validator _validator;
};

} // END namespace json
} // END namespace tegia


#endif