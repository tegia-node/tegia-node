#include "Generator.h"

namespace Docgen 
{
	Generator::Generator()
	{

	};

	Generator::~Generator()
	{

	};

	void Generator::generate(BasicDoc * document, Json::Value data)
	{
		Marker * marker = document->findMarker();
		while(marker != nullptr)
		{
			// marker->print();

			auto marker_action = marker->getAction();
			auto marker_args = marker->getArgs();

			auto data_to_set = this->processAction(marker_action, marker_args, data);
			marker->setData(data_to_set);

			//	TODO: спрятать delete маркера в какую-нибудь функцию типа Docgen::freeMarker(marker);
			delete marker;
			marker = document->findMarker();
		};
	};

	Json::Value Generator::processAction(const string & action, const vector<string> &action_args, Json::Value data)
	{
		if (action == "replace")
		{
			Json::Path xpath(action_args.at(0));
			return xpath.resolve(data, "");
		}

		return "";
	};
	

	void Generator::prepare(BasicDoc * document)
	{
		Marker * marker = document->findMarker2();
		while(marker != nullptr)
		{
			// marker->print();
			document->replaceMark(marker);

			//	TODO: спрятать delete маркера в какую-нибудь функцию типа Docgen::freeMarker(marker);
			delete marker;
			marker = document->findMarker2();
		};
	};

};