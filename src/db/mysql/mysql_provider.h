#ifndef H_TEGIA_DB_MYSQL_PROVIDER
#define H_TEGIA_DB_MYSQL_PROVIDER

////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// INCLUDES                                                                               //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

#include <tegia/const.h>
#include <tegia/core/string.h>

#include "mysql_pool.h"

////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                        //
// PROVIDER CLASS                                                                         //
//                                                                                        //
////////////////////////////////////////////////////////////////////////////////////////////

namespace tegia { 
namespace mysql {

class provider
{
	private:
		static provider* _self;
		std::map < std::string , tegia::mysql::pool * > _pools;
		std::map < std::string , std::string > _aliases;
		tegia::mysql::pool * get_pool(const std::string &name);
		tegia::mysql::pool * add_pool(int threads_count, nlohmann::json config);
		provider();

	public:
		static tegia::mysql::pool * get(const std::string &name);
		static tegia::mysql::pool * add(int threads_count, nlohmann::json config);

		~provider();

};

}   // end namespace mysql
}   // end namespace tegia

#endif 