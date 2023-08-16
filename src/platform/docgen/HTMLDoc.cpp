#include "HTMLDoc.h"

namespace Docgen {


HTMLDoc::HTMLDoc(string file_name, string content):BasicDoc(file_name, content)
{
	// this->_content = "";

	// ifstream fileToRead(file_name.c_str());
	// char c;
	// while(fileToRead.get(c))
	// {
	//  this->_content += c;
	// };

	// fileToRead.close();
};


HTMLDoc::~HTMLDoc()
{

};


Marker * HTMLDoc::findMarker()
{
	// std::cout << "find marker call()\n";
	string marker_action = "replace";
	string marker_component = "";

	vector<string> marker_args;

	//////////////////////////////////////////////////////////

	//	ищем маркер
	size_t marker_start = this->_content.find("[[", _last_processed_mark_pos);
	if (marker_start != string::npos)
	{
		size_t marker_end = this->_content.find("]]", marker_start + 1);

		auto marker_params = core::explode(this->_content.substr(marker_start + 2, marker_end - (marker_start + 2)), "|",false);
				
		marker_component = marker_params.at(0);

		for (int marker_param_index = 1; marker_param_index < marker_params.size(); marker_param_index++)
		{
			marker_args.push_back(marker_params[marker_param_index]);
		}


		this->_last_processed_mark_pos = marker_end + 2;

		//std::cout << this->_content.substr(marker_start, this->_last_processed_mark_pos - marker_start) << std::endl;

		
		//std::cout << "_last_processed_mark_pos = " << this->_last_processed_mark_pos << std::endl;


		return new Marker(this, marker_action, marker_component, marker_start, this->_last_processed_mark_pos - marker_start, marker_args);
	}
	else
	{
		return nullptr;
	}
};

Marker * HTMLDoc::findMarker2()
{
	return nullptr;
};


std::string HTMLDoc::createText(Json::Value data)
{
	return data.asString();//core::cast<string>(data);
};

std::string HTMLDoc::createTable(Json::Value data)
{
	return "";
};

std::string HTMLDoc::createList(Json::Value data)
{
	std::string result;

	if (data.type() == Json::arrayValue)
	{
		for ( int eventIndex = 0; eventIndex < data.size(); ++eventIndex )
		{
			result += "<li>" + data[eventIndex].asString() + "</li>";
		}
		result = "<ul>" + result + "</ul>";
	}

	return result;
};


std::string HTMLDoc::createNewMark(std::string mark)
{
  return "";
};


};