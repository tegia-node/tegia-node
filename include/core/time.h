#ifndef _H_CORE_TIME_
#define _H_CORE_TIME_
// -------------------------------------------------------------------------------------- //
//                                      INCLUDES                                          //
// -------------------------------------------------------------------------------------- //

#include <cstring>
#include <string>
#include <chrono>
#include <iostream>

// strptime    http://www.opennet.ru/man.shtml?topic=strptime&category=3&russian=0

// -------------------------------------------------------------------------------------- //
//                                       DEFINES                                          //
// -------------------------------------------------------------------------------------- //

namespace tegia {
class time
{
	friend bool operator== (const tegia::time &lh, const tegia::time &rh);
	friend bool operator> (const tegia::time &lh, const tegia::time &rh);
	friend bool operator< (const tegia::time &lh, const tegia::time &rh);

	private:
		std::tm * tm;

	public:

		time()
		{
			std::time_t now_c = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
			this->tm = localtime ( &now_c );
		};

		time(const std::time_t &_time)
		{
			this->tm = localtime ( &_time );
		};
		
		time(const std::string &_time, const std::string &format = "%Y-%m-%d %H:%M:%S")
		{
			// https://en.cppreference.com/w/cpp/io/manip/get_time

			this->tm = new std::tm();
			memset(this->tm, 0, sizeof(this->tm));
			strptime(_time.c_str(), format.c_str(), this->tm);
		};

		~time()
		{};

		bool set()
		{
			std::time_t now_c = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
			this->tm = localtime ( &now_c );
			return true;
		};

		bool set(const std::time_t &_time)
		{
			this->tm = localtime ( &_time );
			return true;
		};
		
		bool set(const std::string &_time, const std::string &format = "%Y-%m-%d %H:%M:%S")
		{
			// https://en.cppreference.com/w/cpp/io/manip/get_time

			memset(this->tm, 0, sizeof(this->tm));
			strptime(_time.c_str(), format.c_str(), this->tm);
			return true;
		};


		std::time_t timestamp()
		{
			return mktime(this->tm);
		};

		bool next_day()
		{
			this->tm->tm_mday++;
			std::mktime(this->tm);
			return true;
		};

		bool hour(int _hour)
		{
			this->tm->tm_hour = _hour;
			std::mktime(this->tm);
			return true;
		};

		bool min(int _min)
		{
			this->tm->tm_min = _min;
			std::mktime(this->tm);
			return true;
		};

		bool sec(int _sec)
		{
			this->tm->tm_sec = _sec;
			std::mktime(this->tm);
			return true;
		};

		std::string format(const std::string &format = "%Y-%m-%d %H:%M:%S")
		{
			char buf[255];
			strftime(buf, sizeof(buf), format.c_str(), this->tm);
			//std::string out = (const char*)buf;
			return std::string((const char*)buf);
		};
};

inline bool operator==(const tegia::time &lh, const tegia::time &rh)
{
	//std::cout << lh.timestamp() << std::endl; 
	//if(lh.timestamp() == rh.timestamp()) return true;

	auto lh_tm = mktime(lh.tm);
	auto rh_tm = mktime(rh.tm);
	if(lh_tm == rh_tm) return true;
	return false;
};

inline bool operator>(const tegia::time &lh, const tegia::time &rh)
{
	//std::cout << lh.timestamp() << std::endl; 
	//if(lh.timestamp() > rh.timestamp()) return true;

	auto lh_tm = mktime(lh.tm);
	auto rh_tm = mktime(rh.tm);
	if(lh_tm > rh_tm) return true;
	return false;
};

inline bool operator<(const tegia::time &lh, const tegia::time &rh)
{
	//std::cout << lh.timestamp() << std::endl; 
	//if(lh.timestamp() > rh.timestamp()) return true;

	auto lh_tm = mktime(lh.tm);
	auto rh_tm = mktime(rh.tm);
	if(lh_tm < rh_tm) return true;
	return false;
};


} // END namespace tegia




namespace core {
namespace time {



	// REM:
	// http://en.cppreference.com/w/cpp/chrono/c/tm
	// http://cppstudio.com/post/621/ 

	// strftime: http://www.cplusplus.com/reference/ctime/strftime/
	// strptime: http://man7.org/linux/man-pages/man3/strptime.3.html

	inline std::tm to_tm(
		const std::string &time, 
		const std::string &format = "%Y-%m-%d %H:%M:%S",
		const std::string &locale = "ru_RU.UTF8")
	{
		/*if(locale != "ru_RU.UTF8")
		{
			std::locale::global(std::locale(locale));
			std::tm tm;
			strptime(time.c_str(), format.c_str(), &tm);
			std::locale::global(std::locale("ru_RU.UTF8"));
			return tm;
		}*/

		std::tm tm;
      std::memset(&tm, 0, sizeof(struct tm));
      char *end = nullptr;
		end = strptime(time.c_str(), format.c_str(), &tm);
      if(end == nullptr || *end != '\0')
      {
         // TODO: error
      }
		return tm;
	};


	inline std::string to_str(const std::tm &tm, const std::string &format = "%Y-%m-%d %H:%M:%S")
	{
		char buf[255];
		strftime(buf, sizeof(buf), format.c_str(), &tm);
		std::string out = (const char*)buf;
		return out;

		// https://en.cppreference.com/w/cpp/io/manip/put_time
	};


	inline std::string to_str(const std::time_t &time, const std::string &format = "%Y-%m-%d %H:%M:%S")
	{
		char buf[255];
		strftime(buf, sizeof(buf), format.c_str(), localtime(&time) );
		std::string out = (const char*)buf;
		return out;
	};


	////////////////////////////////////////////////////////////////////////////////////////////
	/** 
		\brief 
 
		format example "%Y-%m-%d %H:%M:%S"
	*/ 
	////////////////////////////////////////////////////////////////////////////////////////////

	inline std::tm str_to_tm(const std::string &time, const std::string &format = "%Y-%m-%d %H:%M:%S")
	{
		std::tm tm = {};
		strptime(time.c_str(), format.c_str(), &tm);
		return tm;

		// https://en.cppreference.com/w/cpp/io/manip/get_time
	};


	////////////////////////////////////////////////////////////////////////////////////////////
	/** 
		\brief 
 
		format example "%Y-%m-%d %H:%M:%S"
	*/
	////////////////////////////////////////////////////////////////////////////////////////////

	inline std::string tm_to_str(const std::tm &tm, const std::string &format = "%Y-%m-%d %H:%M:%S")
	{
		char buf[255];
		strftime(buf, sizeof(buf), format.c_str(), &tm);
		std::string out = (const char*)buf;
		return out;

		// https://en.cppreference.com/w/cpp/io/manip/put_time
	};



////////////////////////////////////////////////////////////////////////////////////////////
/** 
    \brief 
 
    format example "%Y-%m-%d %H:%M:%S"
*/   
////////////////////////////////////////////////////////////////////////////////////////////
    inline std::string unixtime_to_str(const std::time_t &time, const std::string &format = "%Y-%m-%d %H:%M:%S")
    {
       char buf[255];
       strftime(buf, sizeof(buf), format.c_str(), localtime(&time) );
       std::string out = (const char*)buf;
       return out;
    };


	////////////////////////////////////////////////////////////////////////////////////////////
	/** 
		\brief 
	*/
	////////////////////////////////////////////////////////////////////////////////////////////
	
	inline unsigned long long int timestamp(const std::string &time, const std::string &format = "%Y-%m-%d %H:%M:%S")
	{
		std::tm tm = {};
		strptime(time.c_str(), format.c_str(), &tm);
		time_t ttime_t = mktime(&tm);
		return static_cast<unsigned long long int>(ttime_t);
	};

	/** --------------------------------------------------------

	*/
    inline std::string current(const std::string &format = "%Y-%m-%d %H:%M:%S")
    {
       char buffer[80];
            
       std::time_t now_c = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
       std::strftime(buffer, 80, format.c_str(), localtime(&now_c));

       return buffer;
    };


    inline std::tm now()
    {
       std::time_t now_c = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
       std::tm *timeinfo = localtime ( &now_c );
       return *timeinfo;
    };

	inline std::time_t now_t()
	{
		return std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	};


   inline bool check(const std::string &date, const std::string &format = "%Y-%m-%d %H:%M:%S")
   {
      unsigned long long int _time = timestamp(date);
      std::string _tmp = unixtime_to_str(_time);

      if(_tmp == date)
      {
         return true;
      }
      
      return false;
   };


////////////////////////////////////////////////////////////////////////////////////////////
/** 
    \brief ФУНКЦИЯ находит в строке имя месяца на русском языке и возвращает его номер
             
*/   
////////////////////////////////////////////////////////////////////////////////////////////
    
    inline std::string extract_month(const std::string &str)
    { 
       std::string month;
       std::size_t found;

       month = "январ";  
       found = str.find(month);
       if (found!=std::string::npos) 
       {
          return "01";
       }    
       month = "Январ";  
       found = str.find(month);
       if (found!=std::string::npos) 
       {
          return "01";
       }  


       month = "феврал";  
       found = str.find(month);
       if (found!=std::string::npos) 
       {
          return "02";
       }  
       month = "Феврал";  
       found = str.find(month);
       if (found!=std::string::npos) 
       {
          return "02";
       }  


       month = "март";  
       found = str.find(month);
       if (found!=std::string::npos) 
       {
          return "03";
       }  
       month = "Март";  
       found = str.find(month);
       if (found!=std::string::npos) 
       {
          return "03";
       } 


       month = "апрел";  
       found = str.find(month);
       if (found!=std::string::npos) 
       {
          return "04";
       }  

       month = "май";  
       found = str.find(month);
       if (found!=std::string::npos) 
       {
          return "05";
       }  
       month = "Май";  
       found = str.find(month);
       if (found!=std::string::npos) 
       {
          return "05";
       } 


       month = "мая";  
       found = str.find(month);
       if (found!=std::string::npos) 
       {
          return "05";
       }  
       month = "Мая";  
       found = str.find(month);
       if (found!=std::string::npos) 
       {
          return "05";
       }  


       month = "мае";  
       found = str.find(month);
       if (found!=std::string::npos) 
       {
          return "06";
       }  
       month = "Мае";  
       found = str.find(month);
       if (found!=std::string::npos) 
       {
          return "06";
       }  


       month = "июн";  
       found = str.find(month);
       if (found!=std::string::npos) 
       {
          return "06";
       }  
       month = "Июн";  
       found = str.find(month);
       if (found!=std::string::npos) 
       {
          return "06";
       } 


       month = "июл";  
       found = str.find(month);
       if (found!=std::string::npos) 
       {
          return "07";
       }  
       month = "Июл";  
       found = str.find(month);
       if (found!=std::string::npos) 
       {
          return "07";
       }  


       month = "август";  
       found = str.find(month);
       if (found!=std::string::npos) 
       {
          return "08";
       }  
       month = "Август";  
       found = str.find(month);
       if (found!=std::string::npos) 
       {
          return "08";
       } 

       month = "сентябр";  
       found = str.find(month);
       if (found!=std::string::npos) 
       {
          return "09";
       }  
       month = "Сентябр";  
       found = str.find(month);
       if (found!=std::string::npos) 
       {
          return "09";
       } 

       month = "октябр";  
       found = str.find(month);
       if (found!=std::string::npos) 
       {
          return "10";
       }  


       month = "ноябр";  
       found = str.find(month);
       if (found!=std::string::npos) 
       {
          return "11";
       }  
       month = "Ноябр";  
       found = str.find(month);
       if (found!=std::string::npos) 
       {
          return "11";
       } 


       month = "декабр";  
       found = str.find(month);
       if (found!=std::string::npos) 
       {
          return "12";
       }  
       month = "Декабр";  
       found = str.find(month);
       if (found!=std::string::npos) 
       {
          return "12";
       } 


       return "0";

    };



	////////////////////////////////////////////////////////////////////////////////////////////
	/** 
		\brief 
	
		Example:
		cout << core::time::format("2001-11-12 18:31:01", "%Y-%m-%d %H:%M:%S", "%d.%m.%Y %H:%M:%S") << endl;
		out:    12.11.2001 18:31:01

	*/   
	////////////////////////////////////////////////////////////////////////////////////////////

	inline std::string format(
		const std::string &time, 
		const std::string &from, 
		const std::string &to)
	{
		struct tm tm;
		char buf[255];

		setlocale(LC_ALL, "");

		if(strptime(time.c_str(), from.c_str(), &tm) != NULL)
		{
			strftime(buf, sizeof(buf), to.c_str(), &tm);
			std::string out = (const char*)buf;
			return out;
		}
		else
		{
			return "#undef";
		}
	};




    inline std::string getToday()
    {
      time_t rawTime; 
      std::time(&rawTime);
      tm *timeinfo = localtime(&rawTime);

      std::string today = std::to_string( timeinfo->tm_year + 1900 ) + "-" + 
      ( timeinfo->tm_mon > 8 ? std::to_string( timeinfo->tm_mon + 1 ) : "0" + std::to_string( timeinfo->tm_mon + 1 )) + "-" +
      ( timeinfo->tm_mday > 9 ? std::to_string( timeinfo->tm_mday ) : "0" + std::to_string( timeinfo->tm_mday ));
      return today;
    };


  /** --------------------------------------------------------

  */
    inline int getDateDistance(const std::string &date1, const std::string &date2)
    {
        auto _date1 = timestamp( date1, "%4d-%2d-%2d" );
        auto _date2 = timestamp( date2, "%4d-%2d-%2d" );
        auto time1 = std::chrono::system_clock::from_time_t( _date1 );
        auto time2 = std::chrono::system_clock::from_time_t( _date2 );

        int elapsed_days = std::chrono::duration_cast< std::chrono::duration<int, std::ratio<24*3600> > >(time2 - time1).count();
        if( elapsed_days < 0)
          return 0;

        return elapsed_days;
    };



    inline std::string getDateForSQL(unsigned int dt)
    {
      time_t rawTime = std::time(NULL); 
      tm * timeinfo;
      time_t offset = dt;
      timeinfo = localtime(&offset);

      std::string date = std::to_string( timeinfo->tm_year + 1900 ) + "-" + 
      ( timeinfo->tm_mon > 8 ? std::to_string( timeinfo->tm_mon + 1 ) : "0" + std::to_string( timeinfo->tm_mon + 1 )) + "-" +
      ( timeinfo->tm_mday > 9 ? std::to_string( timeinfo->tm_mday ) : "0" + std::to_string( timeinfo->tm_mday ));
      return date;
    };


  }  // end namespase time

}  // end namespace tegia



#endif /* _H_TEGIA_TIME_ */