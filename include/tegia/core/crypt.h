#ifndef H_TEGIA_CORE_CRYPT
#define H_TEGIA_CORE_CRYPT

/////////////////////////////////////////////////////////////////////////////////
//
// INCLUDES
//
/////////////////////////////////////////////////////////////////////////////////

//	STL
	#include <cstring>
	#include <stdio.h>
	#include <stdlib.h>
	#include <sstream>
	#include <string>
	#include <iostream>

	#include <zlib.h>

//	VENDORS
	#include <uuid/uuid.h>

	#include <openssl/md5.h>
	#include <openssl/rsa.h>
	#include <openssl/pem.h>
	#include <openssl/rand.h>
	#include <openssl/bio.h>
	#include <openssl/buffer.h>
	#include <openssl/err.h>


/////////////////////////////////////////////////////////////////////////////////
//
// Random
//
/////////////////////////////////////////////////////////////////////////////////


namespace tegia {
namespace random {

/**
	\brief   Генерирует случайную строку
	\details

	\param[in] lenght   Длина генерируемой случайной строки
*/

std::string string(int lenght);

/**
	\brief   Генерирует случайный пароль длинной lenght символов
	\details

	\param[in] lenght   Длина генерируемого пароля
*/

std::string password(int lenght);

/**
	\brief   Генерирует глобальный уникальный идентификатор 
	\details
*/

std::string uuid();


}	// END namespace random
}	// END namespace tegia


/////////////////////////////////////////////////////////////////////////////////
//
// MD5
//
/////////////////////////////////////////////////////////////////////////////////


namespace tegia {
namespace crypt {

/**
	\brief   Генерирует хеш MD5 для строки
	\details

	\param[in] str   Хешируемая строка
*/

std::string MD5(const std::string &str);
std::string MD5u(const std::string &str);

}	// END namespace crypt
}	// END namespace tegia


/////////////////////////////////////////////////////////////////////////////////
//
// CRC32
//
/////////////////////////////////////////////////////////////////////////////////


namespace tegia {
namespace crypt {

unsigned long int crc32(const std::string &data);

}	// END namespace crypt
}	// END namespace tegia


/////////////////////////////////////////////////////////////////////////////////
//
// RSA
//
/////////////////////////////////////////////////////////////////////////////////

namespace tegia {
namespace crypt {

std::string base64_encode(const unsigned char* buffer, size_t length);
std::string base64_decode(const std::string& encoded_data);

const int PUBLICKEY = 1;
const int PRIVATEKEY = 0;
const int PADDING = RSA_PKCS1_PADDING;
				// RSA_PKCS1_OAEP_PADDING;
				// RSA_SSLV23_PADDING;
				// RSA_NO_PADDING;

class rsa
{
	private:
		// RSA *pRSA = nullptr;
		EVP_PKEY *pkey = nullptr;

		unsigned char * decrypted = nullptr;
		int decrypted_len = 0;

		unsigned char * encrypted = nullptr;
		int encrypted_len = 0;


		int pubkey_len;

		/*
		long long int GetCipherTextSize()
		{
			return RSA_size(this->pRSA);
		};
		*/

	public:

		rsa();
		~rsa();
		// RSA * createRSA(unsigned char * key,int _public);
		// std::string unpack_key(const std::string& base64Key, int keyType);

		//
		//
		//

		bool generate_key_pair(int key_size);
		std::string get_public_key();
		std::string get_private_key();

		// Шифрование публичным ключом
		int public_encrypt(const std::string &data);
		int private_decrypt(const std::string &encrypted_data);
		int private_encrypt(const std::string &data);
		int public_decrypt(const std::string &encrypted_data);

		bool initialize_with_key(const std::string &key, int keyType);

		std::string base64_encode(const unsigned char* buffer, size_t length);
		std::string base64_decode(const std::string& encoded_data);

		//
		//
		//


		/*
		int public_encrypt2(const std::string &data, const std::string &key)
		{
			RSA * rsa = createRSA( (unsigned char *)key.c_str(), PUBLICKEY);
			this->encrypted_len = RSA_public_encrypt(data.length(),(unsigned char *)data.c_str(),this->encrypted,rsa,PADDING);
			RSA_free(rsa);

			std::cout << "public_encrypt2::_2" << std::endl;

			return this->encrypted_len;
		}

		int private_decrypt2(const std::string &data, const std::string &key)
		{
			RSA * rsa = createRSA( (unsigned char *)key.c_str(), PRIVATEKEY);
			this->decrypted_len = RSA_private_decrypt(data.length(),(unsigned char *)data.c_str(),this->decrypted,rsa,PADDING);
			RSA_free(rsa);
			return this->decrypted_len;
		}

		int private_encrypt2(const std::string &data, const std::string &key)
		{
			RSA * rsa = createRSA( (unsigned char *)key.c_str(), PRIVATEKEY);
			this->encrypted_len = RSA_private_encrypt(data.length(),(unsigned char *)data.c_str(),this->encrypted,rsa,PADDING);
			RSA_free(rsa);
			return this->encrypted_len;
		}

		int public_decrypt2(const std::string &data, const std::string &key)
		{
			RSA * rsa = createRSA( (unsigned char *)key.c_str(), PUBLICKEY);
			this->decrypted_len = RSA_public_decrypt(data.length(),(unsigned char *)data.c_str(),this->decrypted,rsa,PADDING);
			RSA_free(rsa);
			return this->decrypted_len;
		}
		*/

		std::string get_encrypt()
		{
			return std::string( (char*)this->encrypted,this->encrypted_len);
			// return this->base64_encode(this->encrypted, this->encrypted_len);
		};

		std::string get_decrypt()
		{
			return std::string( (char*)this->decrypted,this->decrypted_len);
		};

		std::string get_encrypted_base64() 
		{
			return base64_encode(this->encrypted, this->encrypted_len);
		};

};	// END class 

}	// END namespace crypt
}	// END namespace tegia


#endif