#include <regex>

#include <tegia/tegia.h>
#include <tegia/core/const.h>
#include <tegia/core/json.h>
#include <xml2json/include/xml2json.hpp>
#include <tegia/types/types.h>

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


//////////////////////////////////////////////////////////////////////////////////////////
/*

	VALIDATOR

*/
//////////////////////////////////////////////////////////////////////////////////////////


namespace tegia {
namespace json {


//
//
//

validator::validator()
	: _validator(), 
	_is_init(false)
{

};


//
//
//


validator::~validator()
{

};


//
//
//


void validator::format_check(const std::string &format, const std::string &value)
{
	/*
	if (format == "uuid") 
	{
		// tegia::types::
		// if (!check_value_for_something(value))
		{

		}
		throw std::invalid_argument("value is not a good something");
	} 
	else
	*/

	{
		throw std::logic_error("Don't know how to validate " + format);
	}
};


//
//
//


void validator::loader(const nlohmann::json_uri& uri, nlohmann::json& schema)
{
	std::cout << uri.url() << std::endl;
	std::cout << uri.path() << std::endl;
	std::cout << uri.to_string() << std::endl;
	
	std::string url = uri.url();
	auto start = url.find("file://[@");
	if(start != std::string::npos)
	{
		auto end = url.find("]/");
		std::string conf = url.substr(start + 9, end - start - 9);
		std::cout << conf << std::endl;
		std::cout << tegia::conf::path(conf) << std::endl;
	}
	// std::cout << tegia::conf::path();

	std::cout << std::filesystem::current_path().string() << std::endl;
	
	std::ifstream file(uri.url());

	if (!file)
		throw std::runtime_error("Не удалось открыть файл: " + uri.url());

	file >> schema;
}


//
//
//


validator::validator(const validator& other)
	: _filename(other._filename),
	  _schema(other._schema),
	  _validator(),
	  _err(),
	  _is_init(other._is_init)
{
	if (this->_is_init) 
	{
		this->_validator.set_root_schema(this->_schema);
	}
};


//
//
//


validator::validator(validator&& other) noexcept
	: _filename(std::move(other._filename)),
	  _schema(std::move(other._schema)),
	  _validator(),
	  _err(),
	  _is_init(other._is_init)
{
	if (this->_is_init) 
	{
		this->_validator.set_root_schema(this->_schema);
	}
}


//
//
//


bool validator::load(const std::string &filename)
{
	this->_filename = filename;

	int res = 0;
	std::string info = "";
	std::tie(res,info,this->_schema) = tegia::json::_file(filename);

	if(res != 0)
	{
		this->_is_init = false;
		return false;
	}

	this->_validator.set_root_schema(this->_schema); 
	this->_is_init = true;
	return true;
};


//
//
//


bool validator::load(nlohmann::json schema)
{
	this->_filename = "";

	this->_schema = schema;
	this->_validator.set_root_schema(this->_schema); 
	this->_is_init = true;
	return true;
};


//
//
//


bool validator::validate(nlohmann::json data)
{
	if(_is_init == false) return false;

	this->_validator.validate(data, this->_err);

	if (this->_err)
	{
		// std::cout << _ERR_TEXT_ << "Validation failed" << std::endl;
		return false;
	}
	else
	{
		// std::cout << "Validation succeeded\n";
		return true;
	}
};


//
//
//


bool validator::is_init()
{
	return this->_is_init;
};



/*
int validator::load(const std::string &filename)
{
	this->_filename = filename;

	auto [status,info,schema] = tegia::json::_file(filename);
	if(status != 0)
	{
		this->_is_load = false;
		return 502;
	}

	this->_validator.set_root_schema(schema);
	this->_is_load = true;
	return 200;
};


int validator::validate(nlohmann::json * data)
{
	try
	{
		this->_validator.validate( *data );
	}

	catch (const std::exception &e)
	{
		std::cout << _ERR_TEXT_ << "validator '" << this->_name << "'" << std::endl;
		std::cout << e.what() << std::endl; 
		
		std::cout << _YELLOW_ << "validate data" << _BASE_TEXT_ << std::endl;
		std::cout << *data << std::endl;

		return 400;
	}

	return 200;
};
*/

} // END namespace json
} // END namespace tegia
