#include <tegia/core/json.h>
#include <xml2json/include/xml2json.hpp>


namespace tegia {
namespace json {


	nlohmann::json file(const std::string &filename, bool is_null)
	{
		// -----------------------------------------------
		//
		// TODO: https://tegia.atlassian.net/browse/MC-48
		//
		// -----------------------------------------------

		nlohmann::json data = nlohmann::json();

		if(!std::filesystem::exists(filename))
		{
			if(is_null == true)
			{
				return data;
			}
			else
			{
				throw new json::error(JSON_FILE_NOT_FOUND, filename, "file " + filename + " not found");   
			}
		} 

		try
		{
			std::ifstream ifs(filename);
			data = nlohmann::json::parse(ifs);  
			ifs.close();
		}

		catch (nlohmann::json::parse_error& e)
		{
			std::cout << e.what() << " from [" << filename << "]" << std::endl;
			throw new json::error(JSON_FILE_PARSE_ERROR, filename, e.what());   
		}

		catch (nlohmann::json::out_of_range& e)
		{
			std::cout << e.what() << " from [" << filename << "]" << std::endl;
			throw new json::error(JSON_FILE_OUT_OF_RANGE, filename, e.what());   
		}

		return data;
	};


	nlohmann::json file(const std::string &filename, tegia::json::error *error)
	{
		// -----------------------------------------------
		//
		// TODO: https://tegia.atlassian.net/browse/MC-48
		//
		// -----------------------------------------------

		nlohmann::json data = nlohmann::json();

		if(!std::filesystem::exists(filename))
		{
			error->type = JSON_FILE_NOT_FOUND;
			error->filename = filename;
			error->message = "file " + filename + " not found";
			return data;
		} 

		try
		{
			std::ifstream ifs(filename);
			data = nlohmann::json::parse(ifs);  
			ifs.close();
		}

		catch (nlohmann::json::parse_error& e)
		{
			std::cout << e.what() << " from [" << filename << "]" << std::endl;
			error->type = JSON_FILE_PARSE_ERROR;
			error->filename = filename;
			error->message = e.what();
			return data;
		}

		catch (nlohmann::json::out_of_range& e)
		{
			std::cout << e.what() << " from [" << filename << "]" << std::endl;
			error->type = JSON_FILE_OUT_OF_RANGE;
			error->filename = filename;
			error->message = e.what();
			return data;
		}

		return data;
	};



	std::tuple<int, std::string, nlohmann::json> _file(const std::string &filename)
	{
		if(!std::filesystem::exists(filename))
		{
			return std::make_tuple(JSON_FILE_NOT_FOUND, "file '" + filename + "' not found", nlohmann::json::object());
		}

		try
		{
			std::ifstream ifs(filename);
			nlohmann::json data = nlohmann::json::parse(ifs);  
			ifs.close();
			return std::make_tuple(0, "ok", data);
		}

		catch (nlohmann::json::parse_error& e)
		{
			return std::make_tuple(JSON_FILE_PARSE_ERROR, e.what(), nlohmann::json::object());
		}

		catch (nlohmann::json::out_of_range& e)
		{
			return std::make_tuple(JSON_FILE_OUT_OF_RANGE, e.what(), nlohmann::json::object());
		}
	};





	bool save(const std::string &filename, const nlohmann::json &data, const int indent, const char indent_char)
	{
		// -----------------------------------------------
		//
		// TODO: https://tegia.atlassian.net/browse/MC-49
		//
		// -----------------------------------------------

		if(!std::filesystem::exists(filename))
		{
			// TODO: Определить поведение системы

			// throw new json::error(JSON_LOAD_FILE_NOT_FOUND, "file " + filename + " not found");   
		} 

		std::ofstream ofs;
		ofs.open ( filename, std::ios::out );
		if(ofs.good())
		{
			ofs << data.dump(indent,indent_char);
		}
		else
		{
			// TODO: 
		}
		ofs.close();

		return true;
	};


	nlohmann::json convert_xml(const std::string &xml_string)
	{
		return nlohmann::json::parse( xml2json(xml_string.c_str()) );
	};


} // END namespace json
} // END namespace tegia



