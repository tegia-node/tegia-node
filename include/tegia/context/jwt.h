#ifndef H_TEGIA_JWT
#define H_TEGIA_JWT

#include <bitset>

#include <tegia/core/const.h>
#include <tegia/core/json.h>

namespace tegia {

class auth;

class jwt
{
	friend class auth;

	private:
		// Дата генерации токена
		std::string _t_create;
		long long int _exp = 0;

		// тип токена 
		//   1 - клиенский токен
		//   2 - API токен
		int _type = 0;

		// IP адрес,для которого генерируется токен
		std::string _ip;
		
		// Строка, содержащая сгенерированный токен
		std::string _token;

		// Статус токена
		int _status = 0;

		jwt();
		int pack(const std::string &priv_key);
		int unpack(const std::string &tegia_token, const std::string &pub_key);

	public:
		
		~jwt();
		void print();
		int type();
		std::string token();
		int status();
};

}	// END namespace tegia



#endif