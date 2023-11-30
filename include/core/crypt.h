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

const int PUBLICKEY = 1;
const int PRIVATEKEY = 0;
const int PADDING = RSA_PKCS1_PADDING;
				// RSA_PKCS1_OAEP_PADDING;
				// RSA_SSLV23_PADDING;
				// RSA_NO_PADDING;

class rsa
{
	private:
		RSA *pRSA = nullptr;

		unsigned char * decrypted = nullptr;
		int decrypted_len = 0;

		unsigned char * encrypted = nullptr;
		int encrypted_len = 0;


		int pubkey_len;

		long long int GetCipherTextSize()
		{
			return RSA_size(this->pRSA);
		};

	public:

		rsa()
		{
			this->encrypted = new unsigned char[40960];
			this->decrypted = new unsigned char[40960];
		};

		~rsa()
		{
			delete[] this->encrypted;
			delete[] this->decrypted;
			// RSA_free(this->pRSA);
		};

		RSA * createRSA(unsigned char * key,int _public)
		{
			RSA *rsa = nullptr;
			BIO *keybio = nullptr;
			keybio = BIO_new_mem_buf(key, -1);
			if (keybio == nullptr)
			{
				printf( "Failed to create key BIO");
				return 0;
			}

			if(_public == PUBLICKEY)
			{
				rsa = PEM_read_bio_RSA_PUBKEY(keybio, &rsa,NULL, NULL);				
			}
			else
			{
				rsa = PEM_read_bio_RSAPrivateKey(keybio, &rsa,NULL, NULL);
			}

			if(rsa == nullptr)
			{
				printf( "Failed to create RSA");
			}

			//BIO_set_close(keybio, BIO_NOCLOSE); /* So BIO_free() leaves BUF_MEM alone */
			//BIO_free(keybio);
		
			return rsa;
		}

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

		std::string get_encrypt()
		{
			return std::string( (char*)this->encrypted,this->encrypted_len);
		};

		std::string get_decrypt()
		{
			return std::string( (char*)this->decrypted,this->decrypted_len);
		};

};	// END class rsa

}	// END namespace crypt
}	// END namespace tegia


#endif