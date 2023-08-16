#undef  _DEBUG_PRINT_MODE_
#define _DEBUG_PRINT_MODE_ true
#include <tegia/node/logger_print.h>

////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// CONFIG CLASS                                                                           //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

#include <tegia/node/config.h>


namespace tegia {

/////////////////////////////////////////////////////////////////////////////////////////

config::config(const std::string &name, const std::string &filename)
	: name(name), filename(filename)
{ 
	if(this->load() == false)
	{
		std::cout << _ERR_TEXT_ << "not load config file " << filename << std::endl;
		exit(0);
	};

	std::cout << _OK_TEXT_ << _BCYAN_ << "load config file " << _BASE_TEXT_ << filename << std::endl;
}; 

/////////////////////////////////////////////////////////////////////////////////////////

config::~config()
{ 
	
};

/////////////////////////////////////////////////////////////////////////////////////////

bool config::load()
{
	/*
	core::json::error * error = new core::json::error();
	this->data = core::json::file(this->filename, error);
	std::cout << "error->type = " << error->type << std::endl;
	return true;
	*/

	if( !std::filesystem::exists(filename) )
	{
		std::cout << _RED_TEXT_ << "configurations file " << filename << " not found" << _BASE_TEXT_ << std::endl;				
		return false;
	}

	this->data = core::json::file(this->filename);
	return true;

	// TODO: Проверки, что файл корректно открылся и загрузился
};

/////////////////////////////////////////////////////////////////////////////////////////

const nlohmann::json * config::operator[] (const std::string & name) const
{
	if(this->data.contains(name) == true)
	{
		return &this->data[name];
	}
	return nullptr;
};

const nlohmann::json * config::operator[] (nlohmann::json::json_pointer ptr) const
{
	if(this->data.contains(ptr) == true)
	{
		return &this->data[ptr];
	}
	return nullptr;
};


} // end namespace tegia





