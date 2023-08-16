#include <tegia/context/jwt.h>
#include "jwt/jwt.hpp"

nlohmann::json JWT::json()
{
	nlohmann::json tmp = nlohmann::json();
	tmp["uuid"] = this->uuid;
	tmp["name"] = this->name;
	tmp["surname"] = this->surname;
	tmp["patronymic"] = this->patronymic;
	tmp["sex"] = this->sex;
	tmp["photo"] = this->photo;
	tmp["_t_create"] = this->_t_create;
	tmp["status"] = this->status;
	tmp["roles"] = this->_roles.to_ullong();
	tmp["type"] = this->type;
	tmp["ip"] = this->ip;
	return tmp;
};


int JWT::pack(const std::string &priv_key)
{
	using namespace jwt::params;


	jwt::jwt_object token{algorithm("RS256"), secret(priv_key), payload({})};

	//
	// TODO: Настраивать время жизни в конфиге
	//

	if(this->type == 2)
	{
		// 90 суток
		token.payload().add_claim("exp", std::chrono::system_clock::now() + std::chrono::seconds{7776000});
	}
	else
	{
		// 24 часа
		token.payload().add_claim("exp", std::chrono::system_clock::now() + std::chrono::seconds{86400});
	}
	
	token.payload().add_claim("uuid", this->uuid);
	token.payload().add_claim("_t_create", this->_t_create);
	token.payload().add_claim("status", this->status);
	token.payload().add_claim("name", this->name);
	token.payload().add_claim("surname", this->surname);
	token.payload().add_claim("patronymic", this->patronymic);
	token.payload().add_claim("photo", this->photo);
	token.payload().add_claim("sex", this->sex);
	token.payload().add_claim("roles", this->_roles.to_ullong() );
	token.payload().add_claim("type", this->type );
	token.payload().add_claim("ip", this->ip );

	std::error_code ec{};
	auto sign = token.signature(ec);
	if (ec) 
	{
		std::cerr << ec.message() << std::endl;
		return 1;
	}
	else
	{
		this->token = sign;
		return 0;
	}
};


int JWT::unpack(const std::string &tegia_token, const std::string &pub_key)
{
	using namespace jwt::params;
	this->token = tegia_token;

	try 
	{
		auto decode_token = jwt::decode(tegia_token, algorithms({"RS256"}), secret(pub_key));

		auto payload = decode_token.payload();
		this->uuid = payload.get_claim_value<std::string>("uuid");
		this->_t_create = payload.get_claim_value<std::string>("_t_create");
		this->status = payload.get_claim_value<int>("status");

		this->name = payload.get_claim_value<std::string>("name");
		this->surname = payload.get_claim_value<std::string>("surname");
		this->patronymic = payload.get_claim_value<std::string>("patronymic");
		this->sex = payload.get_claim_value<int>("sex");     
		this->photo = payload.get_claim_value<std::string>("photo");
		this->_roles = std::bitset<64>(payload.get_claim_value<unsigned long long int>("roles"));

		this->type = payload.get_claim_value<int>("type");     
		this->ip = payload.get_claim_value<std::string>("ip");		
		return 0;
	}

	catch (const jwt::TokenExpiredError& e) 
	{
		//Handle Token expired exception here
		return 1;
	} 

	catch (const jwt::SignatureFormatError& e) 
	{
		//Handle invalid signature format error
		return 2;
	}

	catch (const jwt::DecodeError& e) 
	{
		//Handle all kinds of other decode errors
		return 3;
	} 

	catch (const jwt::VerificationError& e) 
	{
		// Handle the base verification error.
		//NOTE: There are other derived types of verification errors
		// which will be discussed in next topic.
		return 4;
	}

	catch (...) 
	{
		return 5;
	}

	return 6;	
};
