#include <tegia/ai/prompt.h>

namespace tegia {
namespace ai {

//
//
//

prompt_t::prompt_t()
{
	this->init();
};

//
//
//

prompt_t::~prompt_t()
{

};

//
//
//

void prompt_t::init()
{
	this->_prompt = {
		{ "prompt", "" },
		{ "appendixs", nlohmann::json::array() },
		{ "temperature", 0.2 }
	};
};

//
//
//

nlohmann::json prompt_t::get()
{
	return this->_prompt;
};

//
//
//

std::string prompt_t::load_file_content(const std::string &filename)
{
	std::string content = "";
	std::ifstream file(filename);
	if (!file) 
	{
		std::cerr << "Error opening file: " << filename << std::endl;
		return "";
	}

	std::string line;
	while (std::getline(file, line)) 
	{
		content.append(line);
		content.append(" ");
	}

	file.close();
	return std::move(content);
};

//
//
//

void prompt_t::prompt(const std::string &filename)
{
	this->_prompt["prompt"] = this->load_file_content(filename);
};

//
//
//

void prompt_t::appendix(const std::string &prefix, nlohmann::json data)
{
	this->_prompt["appendixs"].push_back(
		{
			{ "type", "text" }, 
			{ "value", prefix + "\n```json\n" + data.dump() + "\n```" }
		}
	);
};

void prompt_t::appendix(const std::string &prefix, const std::string &data)
{
	this->_prompt["appendixs"].push_back(
		{
			{ "type", "text" }, 
			{ "value", prefix + "\n" + data }
		}
	);
};

void prompt_t::appendix_file(const std::string &prefix, const std::string &filename)
{
	this->_prompt["appendixs"].push_back(
		{
			{ "type", "text" }, 
			{ "value", prefix + "\n" + this->load_file_content(filename) }
		}
	);
};


//
//
//

}  // END namespace ai
}  // END namespace tegia

