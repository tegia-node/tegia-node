#include <tegia/app/ws.h>


/////////////////////////////////////////////////////////////////////
#undef _LOG_LEVEL_
#define _LOG_LEVEL_ _LOG_WARNING_
#include <tegia/context/log.h>
/////////////////////////////////////////////////////////////////////

namespace tegia {
namespace ws {




}	// END namespace ws
}	// END namespace tegia


////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// ROUTERS                                                                                //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////


extern "C" tegia::actors::type_base * _load_type()
{
	auto actor_type = new tegia::actors::type<APP::LEAKS::APPLICATION>(ACTOR_TYPE);

	ADD_ACTION( "/router",							&APP::LEAKS::APPLICATION::router);

	ADD_ACTION( "/leaks/load",						&APP::LEAKS::APPLICATION::leaks_load);
	ADD_ACTION( "/check",							&APP::LEAKS::APPLICATION::check);
	ADD_ACTION( "/search",							&APP::LEAKS::APPLICATION::search);
	ADD_ACTION( "/stat",							&APP::LEAKS::APPLICATION::stat);
	ADD_ACTION( "/stat/generate",					&APP::LEAKS::APPLICATION::stat_generate);

	ADD_ACTION( "/get/source",						&APP::LEAKS::APPLICATION::get_source);
	ADD_ACTION( "/get",								&APP::LEAKS::APPLICATION::get);
	ADD_ACTION( "/graph",							&APP::LEAKS::APPLICATION::graph);	

	return actor_type;
};


////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// ACTION FUNCTIONS                                                                       //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////


namespace APP {
namespace LEAKS {


APPLICATION::APPLICATION(
	const std::string &_name, 
	nlohmann::json data)
	: tegia::actors::actor_base(ACTOR_TYPE, _name, data)
{
	this->_router = new tegia::app::router_t();


	////////////////////////////////////////////////////////////////////////////////////////
	//
	// CHECK API
	//
	////////////////////////////////////////////////////////////////////////////////////////

	/*
		API позволяет проверить, что приложение доступно
		curl -X GET http://leaks.tegia.local/api/v3/leaks/check -H 'Content-Type: application/json'
	*/

	this->_router->add("GET", "/api/v3/leaks/check",
		nlohmann::json::parse(R"({
			"actor": ")" + this->name + R"(",
			"action":"/check",
			"security": false,
			"mapping": {}
		})"));


	/*

		leaks/ws/admin      /{leak}/load
		leaks/ws/admin      /{leak}/replace

	*/

	////////////////////////////////////////////////////////////////////////////////////////
	//
	// LEAKS CHECK APP (leaks.tegia.ru)
	//
	////////////////////////////////////////////////////////////////////////////////////////


	/*
		curl -X POST http://leaks.tegia.ru/api/v3/contacts/search -H 'Content-Type: application/json' -d '{"contact":"79269271370"}'
	*/

	this->_router->add("POST", "/api/v3/contacts/search",
		nlohmann::json::parse(R"({
			"actor": ")" + this->name + R"(",
			"action":"/search",
			"security": false,
			"mapping":
			{
				"/contact": "/post/contact"
			}
		})"));


	this->_router->add("GET", "/api/v3/contacts/stat",
		nlohmann::json::parse(R"({
			"actor": ")" + this->name + R"(",
			"action":"/stat",
			"security": false
		})"));


	this->_router->add("GET", "/api/v3/contacts/stat/generate",
		nlohmann::json::parse(R"({
			"actor": ")" + this->name + R"(",
			"action":"/stat/generate",
			"security": false
		})"));


	////////////////////////////////////////////////////////////////////////////////////////
	//
	// LEAKS DATA
	//
	////////////////////////////////////////////////////////////////////////////////////////


	this->_router->add("GET", "/api/v3/leaks/sources",
		nlohmann::json::parse(R"({
			"actor": ")" + this->name + R"(",
			"action":"/sources",
			"security": false,
			"mapping": {}
		})"));


	/*

		/get/source  - "сырые" данные в формате csv (используется для разработки)
		/get/flat    - плоский список по источникам
		/get         - список по источникам
		/get/graph   - граф данных

	*/





	/*

		curl -X POST http://leaks.tegia.local/api/v3/leaks/get/source -H 'Content-Type: application/json' -d '{"type":"phone","contact":"79269271370"}'
		curl -X POST http://leaks.tegia.ru/api/v3/leaks/get/source -H 'Content-Type: application/json' -d '{"type":"phone","contact":"79085767153"}'
	
	*/

	this->_router->add("POST", "/api/v3/leaks/get/source",
		nlohmann::json::parse(R"({
			"actor": ")" + this->name + R"(",
			"action":"/get/source",
			"security": false,
			"mapping":
			{
				"/contact/type": "/post/type",
				"/contact/value": "/post/contact"
			}
		})"));

	/*

		curl -X POST http://app.my-check.local/api/v3/leaks/get -H 'Content-Type: application/json' -d '{"type":"phone","contact":"79269271370"}'
		curl -X POST http://leaks.tegia.ru/api/v3/leaks/get -H 'Content-Type: application/json' -d '{"type":"phone","contact":"79085767153"}'
	
		ERROR: curl -X POST http://leaks.tegia.local/api/v3/leaks/get -H 'Content-Type: application/json' -d '{"type":"phone","contact":"79217471449"}'
	*/

	this->_router->add("POST", "/api/v3/leaks/get",
		nlohmann::json::parse(R"({
			"actor": ")" + this->name + R"(",
			"action":"/get",
			"security": false,
			"mapping":
			{
				"/contact/type": "/post/type",
				"/contact/value": "/post/contact"
			}
		})"));

	/*
		curl -X POST http://leaks.tegia.ru/api/v3/leaks/graph -H 'Content-Type: application/json' -d '{"type":"phone","contact":"79269271370"}'
		curl -X POST http://leaks.tegia.local/api/v3/leaks/graph -H 'Content-Type: application/json' -d '{"type":"phone","contact":"79269271370"}'

		type = {phone,email,vk_id}
	*/

	this->_router->add("POST", "/api/v3/leaks/graph",
		nlohmann::json::parse(R"({
			"actor": ")" + this->name + R"(",
			"action":"/graph",
			"security": false,
			"mapping":
			{
				"/contact/type": "/post/type",
				"/contact/value": "/post/contact"
			}
		})"));


	////////////////////////////////////////////////////////////////////////////////////////
	//
	// LEAKS LOAD
	//
	////////////////////////////////////////////////////////////////////////////////////////










	//
	// ENRICHMENT SEARCH
	//

	/*

		curl -X POST http://enrich.tegia.local/api/v3/enrichment/search -H 'Content-Type: application/json' -d '{"type":"phone","contact":"79269271370"}'
		curl -X POST http://leaks.tegia.ru/api/v3/enrichment/search -H 'Content-Type: application/json' -d '{"type":"phone","contact":"79269271370"}'
	
	*/

	this->_router->add("POST", "/api/v3/enrichment/search",
		nlohmann::json::parse(R"({
			"actor": "app/enrichment",
			"action":"/search",
			"security": true,
			"mapping":
			{
				"/type": "/post/type",
				"/contact": "/post/contact"
			}
		})"));

	/*

		curl -X POST http://app.my-check.local/api/v3/enrichment/_get -H 'Content-Type: application/json' -d '{"type":"phone","contact":"79269271370"}'
		curl -X POST http://leaks.tegia.ru/api/v3/enrichment/_get -H 'Content-Type: application/json' -d '{"type":"phone","contact":"79085767153"}'
	
	*/

	this->_router->add("POST", "/api/v3/enrichment/_get",
		nlohmann::json::parse(R"({
			"actor": "app/enrichment",
			"action":"/_get",
			"security": false,
			"mapping":
			{
				"/type": "/post/type",
				"/contact": "/post/contact"
			}
		})"));

	// curl -X GET http://leaks.tegia.ru/api/v3/enrichment/_get/phone/1 -H 'Content-Type: application/json'

	this->_router->add("GET", "/api/v3/enrichment/_get/phone/{id}",
		nlohmann::json::parse(R"({
			"actor": "app/enrichment",
			"action":"/_phone",
			"security": false,
			"mapping":
			{
				"/id": "/params/id"
			}
		})"));


	// curl -X GET http://enrich.tegia.local/api/v3/enrichment/_all/1 -H 'Content-Type: application/json'

	this->_router->add("GET", "/api/v3/enrichment/_all/{offset}",
		nlohmann::json::parse(R"({
			"actor": "app/enrichment",
			"action":"/_all",
			"security": false,
			"mapping":
			{
				"/offset": "/params/offset"
			}
		})"));

	//
	// ENRICHMENT LOADER
	//

	this->_router->add("GET", "/api/v3/leaks/load/linkedin",
		nlohmann::json::parse(R"({
			"actor": "app/leaks2/loader",
			"action":"/linkedin",
			"security": false
		})"));





	this->_router->add("GET", "/api/v3/leaks/{source}/load",
		nlohmann::json::parse(R"({
			"actor": ")" + this->name + R"(",
			"action":"/leaks/load",
			"security": false,
			"mapping":
			{
				"/leaks/source": "/params/source"
			}
		})"));


	this->_router->add("GET", "/api/v3/leaks/{source}/load/csv",
		nlohmann::json::parse(R"({
			"actor": "leaks/loader/{/source}",
			"action":"/csv",
			"security": false,
			"mapping":
			{
				"/leaks/source": "/params/source"
			}
		})"));

	this->_router->add("GET", "/api/v3/leaks/{source}/replace2",
		nlohmann::json::parse(R"({
			"actor": "leaks/loader/{/source}",
			"action":"/replace2",
			"security": false,
			"mapping":
			{
				"/leaks/source": "/params/source"
			}
		})"));






	this->_router->add("GET", "/api/v3/leaks/{source}/test",
		nlohmann::json::parse(R"({
			"actor": "app/leaks2/loader",
			"action":"/test",
			"security": false,
			"mapping":
			{
				"/leaks/source": "/params/source"
			}
		})"));


	this->_router->add("GET", "/api/v3/leaks/test",
		nlohmann::json::parse(R"({
			"actor": "app/leaks2/loader",
			"action":"/test2",
			"security": false,
			"mapping": {}
		})"));


	this->_router->add("GET", "/api/v3/leaks/{source}/replace",
		nlohmann::json::parse(R"({
			"actor": "app/leaks2/loader",
			"action":"/replace",
			"security": false,
			"mapping":
			{
				"/leaks/source": "/params/source"
			}
		})"));


	this->_router->add("GET", "/api/v3/leaks/{source}/check",
		nlohmann::json::parse(R"({
			"actor": "app/leaks2/loader",
			"action":"/check",
			"security": false,
			"mapping":
			{
				"/leaks/source": "/params/source"
			}
		})"));

	this->_router->add("GET", "/api/v3/leaks/{source}/show/{row}",
		nlohmann::json::parse(R"({
			"actor": "app/leaks2/loader",
			"action":"/show",
			"security": false,
			"mapping":
			{
				"/leaks/source": "/params/source",
				"/leaks/row": "/params/row"
			}
		})"));





	/*

		Добавляет загруженную утечку в поисковые индексы
	
	*/
	
	this->_router->add("GET", "/api/v3/leaks/index/{source}/append",
		nlohmann::json::parse(R"({
			"actor": "app/leaks2/loader",
			"action":"/index/append",
			"security": false,
			"mapping":
			{
				"/leaks/source": "/params/source"
			}
		})"));

	this->_router->add("GET", "/api/v3/leaks/index/reload",
		nlohmann::json::parse(R"({
			"actor": "app/leaks2/loader",
			"action":"/index/reload",
			"security": false
		})"));


	//
	// TRASH
	//

	this->_router->add("GET", "/api/v3/enrichment/chat",
		nlohmann::json::parse(R"({
			"actor": "app/enrichment",
			"action":"/chat",
			"security": false,
			"mapping": {}
		})"));




	//
	// SOURCE CLEAN
	//

	this->_router->add("GET", "/api/v3/leaks/clear/010",
		nlohmann::json::parse(R"({
			"actor": "app/leaks2/loader",
			"action":"/010",
			"security": false,
			"mapping": {}
		})"));

	this->_router->add("GET", "/api/v3/leaks/clear/012",
		nlohmann::json::parse(R"({
			"actor": "app/leaks2/loader",
			"action":"/012",
			"security": false,
			"mapping": {}
		})"));

	this->_router->add("GET", "/api/v3/leaks/clear/013",
		nlohmann::json::parse(R"({
			"actor": "app/leaks2/loader",
			"action":"/013",
			"security": false,
			"mapping": {}
		})"));

	this->_router->add("GET", "/api/v3/leaks/clear/026",
		nlohmann::json::parse(R"({
			"actor": "app/leaks2/loader",
			"action":"/026",
			"security": false,
			"mapping": {}
		})"));



};


////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////////////////


APPLICATION::~APPLICATION()
{
	delete this->_router;
	delete this->sources;
};


////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////////////////


int APPLICATION::init(nlohmann::json _data)
{
	if(_data.contains("data") == false)
	{
		LERROR("ACTOR '" + this->name + "' INIT ERROR")
		return 500;
	}

	this->state.data_folder = _data["data"]["folder"].get<std::string>();
	this->state.data_source = _data["data"]["source"].get<std::string>();
	this->state.data_url = _data["data"]["url"].get<std::string>();
	this->state.data_token = _data["data"]["token"].get<std::string>();
	this->sources = new tegia::leaks::sources_t(this->state.data_folder);
	return 200;
};


}	// END namespace LEAKS
}	// END namespace APP


////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// ACTION FUNCTIONS                                                                       //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

#include "actions/check.cpp"
#include "actions/router.cpp"

#include "actions/leaks/load.cpp"
#include "actions/leaks/search.cpp"
#include "actions/leaks/stat.cpp"
#include "actions/leaks/stat_generate.cpp"

#include "actions/get/source.cpp"
#include "actions/get.cpp"
#include "actions/graph.cpp"

////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// COMMON FUNCTIONS                                                                       //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////


#include "common/_get.cpp"
