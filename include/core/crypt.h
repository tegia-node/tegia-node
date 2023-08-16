#ifndef _H_CORE_CRYPT_
#define _H_CORE_CRYPT_

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

//	TEGIA
	#include <tegia/core/string.h>


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

inline unsigned long int crc32(const std::string &data)
{
	unsigned long int crc = ::crc32(0L, Z_NULL, 0);
	crc = ::crc32(crc, (const unsigned char*)data.c_str(), data.length());
	return crc;
};

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



		/*int public_encrypt(unsigned char * data,int data_len,unsigned char * key, unsigned char *encrypted)
		{
			RSA * rsa = createRSA(key,1);
			int result = RSA_public_encrypt(data_len,data,encrypted,rsa,RSA_PKCS1_PADDING);
			return result;
		}
		int private_decrypt(unsigned char * enc_data,int data_len,unsigned char * key, unsigned char *decrypted)
		{
			RSA * rsa = createRSA(key,0);
			int  result = RSA_private_decrypt(data_len,enc_data,decrypted,rsa,RSA_PKCS1_PADDING);
			return result;
		}

		int private_encrypt(unsigned char * data,int data_len,unsigned char * key, unsigned char *encrypted)
		{
			RSA * rsa = createRSA(key,0);
			int result = RSA_private_encrypt(data_len,data,encrypted,rsa,RSA_PKCS1_PADDING);
			return result;
		}
		int public_decrypt(unsigned char * enc_data,int data_len,unsigned char * key, unsigned char *decrypted)
		{
			RSA * rsa = createRSA(key,1);
			int  result = RSA_public_decrypt(data_len,enc_data,decrypted,rsa,RSA_PKCS1_PADDING);
			return result;
		}*/

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






namespace core {



[[deprecated]]
class crypt
{
  protected:    
      
    
  public: 
     
    crypt() {   }; 
    ~crypt() {  };

	[[deprecated("use tegia::random::string()")]]
    static std::string genStr(int lenght)
    {
      int l2 = lenght * 2;
      unsigned char buf[l2];
      if(RAND_bytes(buf, l2-1))
      {
        char res[l2];
        for(int i=0; i<lenght; i++)
        {
          sprintf(res+i*2, "%02x", buf[i]);
        }
        return res;
      }

      return "";       
    };



	[[deprecated("use tegia::random::password()")]]
	static std::string password_gen(int lenght)
	{
		srand( std::time(nullptr)); //инициализируем генератор случайных чисел
		char *pass = new char[lenght + 1]; //выделяем память для строки пароля
		for (int i = 0; i < lenght; ++ i) 
		{
			switch(rand() % 3) //генерируем случайное число от 0 до 2
			{
				case 0: //если ноль
					pass[i] = rand() % 10 + '0'; //вставляем в пароль случайную цифру
					break;
				case 1: //если единица
					pass[i] = rand() % 26 + 'A'; //вставляем случайную большую букву
					break;
				case 2: //если двойка
					pass[i] = rand() % 26 + 'a'; //вставляем случайную маленькую букву
			}
		}  
		pass[lenght+1] = '\0';    
		std::string password(pass);
		delete[] pass;

		return password;
	};


	[[deprecated]]
	static std::string MD5(const std::string &str)
	{
		unsigned char digest[16];
		const char* string = str.c_str();
		MD5_CTX context;
		MD5_Init(&context);
		MD5_Update(&context, string, strlen(string));
		MD5_Final(digest, &context);

		char md5string[33];
		for(int i = 0; i < 16; ++i)
			sprintf(&md5string[i*2], "%02x", (unsigned int)digest[i]);

		return (const char*) md5string;
	};



};


} // end namespace core

#endif  // _H_CORE_CRYPT_