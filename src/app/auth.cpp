#include <iostream>

#include <tegia/core/time.h>
#include <tegia/app/auth.h>
#include <jwt/jwt.hpp>

#include "../threads/data.h"



namespace tegia {


::tegia::user * auth::test(
	const std::string _uuid,
	const std::string _surname,
	const std::string _name,
	const std::string _patronymic,
	int _gender,
	unsigned long long int roles)
{
	::tegia::user * _user = new ::tegia::user();

	_user->_uuid = _uuid;
	_user->_surname = _surname;
	_user->_name = _name;
	_user->_patronymic = _patronymic;
	_user->_gender = _gender;
	_user->_roles = std::bitset<64>(roles);
	
	return _user;
};



std::string auth::key(const std::string &filename)
{
	std::ifstream file; 
	file.open(filename);

	std::string data;
	std::string line;
	while ( file ) 
	{
		std::getline (file, line);
		data.append("\n");
		data.append(line);
	}

	file.close();
	return std::move(data);
};




::tegia::jwt * auth::generate_jwt(::tegia::user * user, int type, const std::string &priv_key)
{
	::tegia::jwt * _jwt = new ::tegia::jwt();


	using namespace ::jwt::params;
	::jwt::jwt_object token{algorithm("RS512"), secret(priv_key), payload({})};

	//
	// TODO: Настраивать время жизни в конфиге
	//

	_jwt->_type = type;
	if(type == 2)
	{
		// 180 суток
		token.payload().add_claim("exp", std::chrono::system_clock::now() + std::chrono::seconds{15552000});
	}
	else
	{
		// 24 часа
		token.payload().add_claim("exp", std::chrono::system_clock::now() + std::chrono::seconds{86400});
	}
	
	token.payload().add_claim("uuid", user->_uuid);
	// token.payload().add_claim("_t_create", this->_t_create);
	// token.payload().add_claim("status", this->status);
	token.payload().add_claim("name", user->_name);
	token.payload().add_claim("surname", user->_surname);
	token.payload().add_claim("patronymic", user->_patronymic);
	token.payload().add_claim("photo", user->_photo);
	token.payload().add_claim("gender", user->_gender);
	token.payload().add_claim("roles", user->_roles.to_ullong() );
	token.payload().add_claim("type", type );
	// token.payload().add_claim("ip", this->ip );

	std::error_code ec{};
	auto sign = token.signature(ec);
	if (ec) 
	{
		std::cerr << ec.message() << std::endl;
		_jwt->_status = 400;
		return _jwt;
	}
	else
	{
		_jwt->_status = 200;
		_jwt->_token = sign;
		return _jwt;
	}

	return _jwt;
};




::tegia::jwt * auth::unpack(const std::string &token, const std::string &pub_key)
{
	using namespace ::jwt::params;

	auto _user = std::make_shared<::tegia::user>();
	::tegia::jwt  * _jwt  = new ::tegia::jwt();

	try 
	{
		auto decode_token = ::jwt::decode(token, algorithms({"RS512"}), secret(pub_key));
		auto payload = decode_token.payload();
		
		//
		// USER
		//

		_user->_uuid = payload.get_claim_value<std::string>("uuid");	
		_user->_surname = payload.get_claim_value<std::string>("surname");
		_user->_name = payload.get_claim_value<std::string>("name");
		_user->_patronymic = payload.get_claim_value<std::string>("patronymic");
		_user->_gender = payload.get_claim_value<int>("gender");
		_user->_roles = std::bitset<64>(payload.get_claim_value<unsigned long long int>("roles"));
		_user->_status = 200;

		//
		// JWT
		//

		_jwt->_token = token;
		_jwt->_exp = payload.get_claim_value<long long int>("exp");
		auto tm = tegia::time(_jwt->_exp);
		_jwt->_t_create = tm.format();
		_jwt->_type = payload.get_claim_value<int>("type");
		_jwt->_status = 200;

		//

		tegia::threads::data->user = _user;
		return _jwt;
	}

	catch (const ::jwt::TokenExpiredError& e) 
	{
		//Handle Token expired exception here
		_jwt->_status = 1;
		tegia::threads::data->user = _user;
		return _jwt;
	} 

	catch (const ::jwt::SignatureFormatError& e) 
	{
		//Handle invalid signature format error
		_jwt->_status = 2;
		tegia::threads::data->user = _user;
		return _jwt;
	}

	catch (const ::jwt::DecodeError& e) 
	{
		//Handle all kinds of other decode errors
		_jwt->_status = 3;
		tegia::threads::data->user = _user;
		return _jwt;
	} 

	catch (const ::jwt::VerificationError& e) 
	{
		// Handle the base verification error.
		//NOTE: There are other derived types of verification errors
		// which will be discussed in next topic.
		_jwt->_status = 4;
		tegia::threads::data->user = _user;
		return _jwt;
	}

	catch (...) 
	{
		_jwt->_status = 5;
		tegia::threads::data->user = _user;
		return _jwt;
	}	
};



}	// END namespace tegia

