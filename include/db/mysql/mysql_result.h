#ifndef H_TEGIA_DB_MYSQL_RESULT
#define H_TEGIA_DB_MYSQL_RESULT


namespace tegia {
namespace mysql {

	template<class _Obj_>
	class result
	{
		public:
			int code;
			std::string info;
			_Obj_ *obj;

			int affected_rows = 0;
			std::string dup_key = "";
			std::string dup_value = "";
			
			result():
				code(0),
				info(""),
				obj(nullptr)
			{

			};
	};

}	// END namespace mysql
}	// END namespace tegia

#endif 