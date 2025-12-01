#include <tegia/core/crypt.h>
#include <random>
#include <chrono>

#include <openssl/evp.h>
#include <vector>
#include <iomanip>
#include <sstream>

/////////////////////////////////////////////////////////////////////////////////
//
//
// RANDOM
//
//
/////////////////////////////////////////////////////////////////////////////////


namespace tegia {
namespace random {


std::string string(int lenght)
{
	char symbols[63] = "abcdefghijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	std::mt19937 rnd(std::chrono::steady_clock::now().time_since_epoch().count());
	std::uniform_int_distribution<int> distrib(0, 61);

	char * _random = new char[lenght];
	std::memset(_random, 0, lenght);

	for(int i = 0; i < lenght; i++)
	{
		/*
		int _rnd = distrib(rnd);
		char s = symbols[_rnd];
		std::cout << i << " [" << _rnd << "] [" << s << "]" << std::endl;
		_random[i] = s;
		*/
		_random[i] = symbols[distrib(rnd)];
	}
	std::string random('0',lenght);
	random.assign(&_random[0],&_random[0] + lenght);

	delete [] _random;
	return random;
};


std::string password(int lenght)
{
	srand( std::time(nullptr)); //инициализируем генератор случайных чисел
	char *pass = new char[lenght + 1]; //выделяем память для строки пароля
	for (int i = 0; i < lenght; ++ i) 
	{
		switch(rand() % 3) //генерируем случайное число от 0 до 2
		{
			case 0: //если ноль
			{
				pass[i] = rand() % 10 + '0'; //вставляем в пароль случайную цифру
			}
			break;
			
			case 1: //если единица
			{
				pass[i] = rand() % 26 + 'A'; //вставляем случайную большую букву
			}	
			break;

			case 2: //если двойка
			{
				pass[i] = rand() % 26 + 'a'; //вставляем случайную маленькую букву
			}
			break;
		}
	}
	pass[lenght+1] = '\0';
	std::string password(pass);
	delete[] pass;

	return password;
};



std::string uuid()
{
	uuid_t out;
	uuid_generate(out);

	if(uuid_is_null(out) == 1)
	{
		return "";
	}
	else
	{
		char *str = new char[37];
		uuid_unparse_lower(out, str);
		std::string uuid_str = (const char*) str;
		delete[] str;

		return uuid_str;
	}
};


}	// END namespace random
}	// END namespace tegia


/////////////////////////////////////////////////////////////////////////////////
//
//
// CRC32
//
//
/////////////////////////////////////////////////////////////////////////////////


namespace tegia {
namespace crypt {

unsigned long int crc32(const std::string &data)
{
	unsigned long int crc = ::crc32(0L, Z_NULL, 0);
	crc = ::crc32(crc, (const unsigned char*)data.c_str(), data.length());
	return crc;
};

}	// END namespace crypt
}	// END namespace tegia


/////////////////////////////////////////////////////////////////////////////////
//
//
// MD5
//
//
/////////////////////////////////////////////////////////////////////////////////


namespace tegia {
namespace crypt {

/*
std::string MD5(const std::string &str)
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
*/

std::string MD5(const std::string &str)
{
	EVP_MD_CTX *ctx = EVP_MD_CTX_new();
	if (!ctx) return {};

	if (EVP_DigestInit_ex(ctx, EVP_md5(), nullptr) != 1) {
		EVP_MD_CTX_free(ctx);
		return {};
	}

	if (EVP_DigestUpdate(ctx, str.data(), str.size()) != 1) {
		EVP_MD_CTX_free(ctx);
		return {};
	}

	unsigned char digest[EVP_MAX_MD_SIZE];
	unsigned int digest_len = 0;
	if (EVP_DigestFinal_ex(ctx, digest, &digest_len) != 1) {
		EVP_MD_CTX_free(ctx);
		return {};
	}

	EVP_MD_CTX_free(ctx);

	std::ostringstream oss;
	for (unsigned int i = 0; i < digest_len; ++i)
		oss << std::hex << std::setw(2) << std::setfill('0') << (int)digest[i];

	return oss.str();
}


std::string MD5u(const std::string &str)
{
	std::string hash = MD5(str);
	return hash.substr(0,8) + "-" + hash.substr(8,4) + "-" + hash.substr(12,4) + "-" + hash.substr(16,4) + "-" + hash.substr(20);
};

}	// END namespace crypt
}	// END namespace tegia


/////////////////////////////////////////////////////////////////////////////////
//
//
// RSA
//
//
/////////////////////////////////////////////////////////////////////////////////


namespace tegia {
namespace crypt {



std::string base64_encode(const unsigned char* buffer, size_t length)
{
	BIO *bio, *b64;
	BUF_MEM *buffer_ptr;

	b64 = BIO_new(BIO_f_base64());
	bio = BIO_new(BIO_s_mem());
	bio = BIO_push(b64, bio);

	BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); // Без новой строки
	BIO_write(bio, buffer, length);
	BIO_flush(bio);
	BIO_get_mem_ptr(bio, &buffer_ptr);

	std::string base64_str(buffer_ptr->data, buffer_ptr->length);
	BIO_free_all(bio);

	return base64_str;
}


std::string base64_decode(const std::string& encoded_data)
{
	BIO *bio, *b64;
	int decode_len = encoded_data.length();
	std::vector<unsigned char> decoded_data(decode_len);

	bio = BIO_new_mem_buf(encoded_data.data(), -1);
	b64 = BIO_new(BIO_f_base64());
	bio = BIO_push(b64, bio);

	BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); // Без новой строки
	decode_len = BIO_read(bio, decoded_data.data(), decode_len);
	decoded_data.resize(decode_len); // Обрезаем до фактического размера
	BIO_free_all(bio);

	return std::string(decoded_data.begin(), decoded_data.end());
}


/////////////////////////////////////////////////////////////////////////////////
/*

*/
/////////////////////////////////////////////////////////////////////////////////



rsa::rsa()
{
	this->encrypted = new unsigned char[40960];
	this->decrypted = new unsigned char[40960];
};


rsa::~rsa()
{
	delete[] this->encrypted;
	delete[] this->decrypted;
	/*
	if (this->pRSA) 
	{
		RSA_free(this->pRSA);
	}	
	*/
};


/////////////////////////////////////////////////////////////////////////////////
/*

*/
/////////////////////////////////////////////////////////////////////////////////


bool rsa::generate_key_pair(int key_size) 
{
	EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, nullptr);
	if (!ctx) {
		return false;
	}

	if (EVP_PKEY_keygen_init(ctx) <= 0) {
		EVP_PKEY_CTX_free(ctx);
		return false;
	}

	if (EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, key_size) <= 0) {
		EVP_PKEY_CTX_free(ctx);
		return false;
	}

	EVP_PKEY *pkey = nullptr;
	if (EVP_PKEY_keygen(ctx, &pkey) <= 0) {
		EVP_PKEY_CTX_free(ctx);
		return false;
	}

	EVP_PKEY_CTX_free(ctx);

	// Если в классе раньше был pRSA, теперь заменяем на pkey
	if (this->pkey) {
		EVP_PKEY_free(this->pkey);
	}
	this->pkey = pkey;

	return true;
}


/*
bool rsa::generate_key_pair(int key_size) 
{
	BIGNUM *bn = BN_new();
	BN_set_word(bn, RSA_F4);

	this->pRSA = RSA_new();
	if (RSA_generate_key_ex(this->pRSA, key_size, bn, nullptr) != 1) 
	{
		BN_free(bn);
		return false;
	}

	BN_free(bn);
	return true;
};
*/


/////////////////////////////////////////////////////////////////////////////////
/*

*/
/////////////////////////////////////////////////////////////////////////////////

std::string rsa::get_public_key() 
{
	BIO *bio = BIO_new(BIO_s_mem());
	if (!bio) {
		return {};
	}

	// Современный способ через EVP_PKEY
	if (PEM_write_bio_PUBKEY(bio, this->pkey) != 1) {
		BIO_free(bio);
		return {};
	}

	char *key_data = nullptr;
	long key_len = BIO_get_mem_data(bio, &key_data);
	std::string public_key(key_data, key_len);

	BIO_free(bio);
	return public_key;
}

/*
std::string rsa::get_public_key() 
{
	BIO *bio = BIO_new(BIO_s_mem());
	PEM_write_bio_RSA_PUBKEY(bio, this->pRSA);

	char *key_data = nullptr;
	long key_len = BIO_get_mem_data(bio, &key_data);
	std::string public_key(key_data, key_len);

	BIO_free(bio);
	return public_key;
}
*/

/////////////////////////////////////////////////////////////////////////////////
/*

*/
/////////////////////////////////////////////////////////////////////////////////

std::string rsa::get_private_key() 
{
	BIO *bio = BIO_new(BIO_s_mem());
	if (!bio) {
		return {};
	}

	// пишем через EVP_PKEY (современный API)
	if (PEM_write_bio_PrivateKey(bio, this->pkey, nullptr, nullptr, 0, nullptr, nullptr) != 1) {
		BIO_free(bio);
		return {};
	}

	char *key_data = nullptr;
	long key_len = BIO_get_mem_data(bio, &key_data);
	std::string private_key(key_data, key_len);

	BIO_free(bio);
	return private_key;
}

/*
std::string rsa::get_private_key() 
{
	BIO *bio = BIO_new(BIO_s_mem());
	PEM_write_bio_RSAPrivateKey(bio, this->pRSA, nullptr, nullptr, 0, nullptr, nullptr);

	char *key_data = nullptr;
	long key_len = BIO_get_mem_data(bio, &key_data);
	std::string private_key(key_data, key_len);

	BIO_free(bio);
	return private_key;
}
*/

/////////////////////////////////////////////////////////////////////////////////
/*

*/
/////////////////////////////////////////////////////////////////////////////////


int rsa::public_encrypt(const std::string &data) 
{
	// this->encrypted_len = RSA_public_encrypt(data.length(), (unsigned char *)data.c_str(), this->encrypted, pRSA, PADDING);
	// return this->encrypted_len;

	EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(this->pkey, nullptr);
	if (!ctx) return -1;

	if (EVP_PKEY_encrypt_init(ctx) <= 0) {
		EVP_PKEY_CTX_free(ctx);
		return -1;
	}

	size_t outlen = 0;
	if (EVP_PKEY_encrypt(ctx, nullptr, &outlen,
						reinterpret_cast<const unsigned char*>(data.data()),
						data.size()) <= 0) {
		EVP_PKEY_CTX_free(ctx);
		return -1;
	}

	this->encrypted_len = outlen;
	if (EVP_PKEY_encrypt(ctx, this->encrypted, &outlen,
						reinterpret_cast<const unsigned char*>(data.data()),
						data.size()) <= 0) {
		EVP_PKEY_CTX_free(ctx);
		return -1;
	}

	EVP_PKEY_CTX_free(ctx);
	return this->encrypted_len;	
};

int rsa::private_decrypt(const std::string &encrypted_data) 
{
	// this->decrypted_len = RSA_private_decrypt(encrypted_data.length(), (unsigned char *)encrypted_data.c_str(), this->decrypted, pRSA, PADDING);
	// return this->decrypted_len;

	EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(this->pkey, nullptr);
	if (!ctx) return -1;

	if (EVP_PKEY_decrypt_init(ctx) <= 0) {
		EVP_PKEY_CTX_free(ctx);
		return -1;
	}

	size_t outlen = 0;
	if (EVP_PKEY_decrypt(ctx, nullptr, &outlen,
						reinterpret_cast<const unsigned char*>(encrypted_data.data()),
						encrypted_data.size()) <= 0) {
		EVP_PKEY_CTX_free(ctx);
		return -1;
	}

	this->decrypted_len = outlen;
	if (EVP_PKEY_decrypt(ctx, this->decrypted, &outlen,
						reinterpret_cast<const unsigned char*>(encrypted_data.data()),
						encrypted_data.size()) <= 0) {
		EVP_PKEY_CTX_free(ctx);
		return -1;
	}

	EVP_PKEY_CTX_free(ctx);
	return this->decrypted_len;	
};


int rsa::private_encrypt(const std::string &data) 
{
	// this->encrypted_len = RSA_private_encrypt(data.length(), (unsigned char *)data.c_str(), this->encrypted, pRSA, PADDING);
	// return this->encrypted_len;

	EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(this->pkey, nullptr);
	if (!ctx) return -1;

	if (EVP_PKEY_sign_init(ctx) <= 0) {
		EVP_PKEY_CTX_free(ctx);
		return -1;
	}

	size_t outlen = 0;
	if (EVP_PKEY_sign(ctx, nullptr, &outlen,
					reinterpret_cast<const unsigned char*>(data.data()),
					data.size()) <= 0) {
		EVP_PKEY_CTX_free(ctx);
		return -1;
	}

	this->encrypted_len = outlen;
	if (EVP_PKEY_sign(ctx, this->encrypted, &outlen,
					reinterpret_cast<const unsigned char*>(data.data()),
					data.size()) <= 0) {
		EVP_PKEY_CTX_free(ctx);
		return -1;
	}

	EVP_PKEY_CTX_free(ctx);
	return this->encrypted_len;	
}


int rsa::public_decrypt(const std::string &encrypted_data) 
{
	// this->decrypted_len = RSA_public_decrypt(encrypted_data.length(), (unsigned char *)encrypted_data.c_str(), this->decrypted, pRSA, PADDING);
	// return this->decrypted_len;

	// Public decrypt = проверка подписи (verify)
	EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(this->pkey, nullptr);
	if (!ctx) return -1;

	if (EVP_PKEY_verify_recover_init(ctx) <= 0) {
		EVP_PKEY_CTX_free(ctx);
		return -1;
	}

	size_t outlen = 0;
	if (EVP_PKEY_verify_recover(ctx, nullptr, &outlen,
								reinterpret_cast<const unsigned char*>(encrypted_data.data()),
								encrypted_data.size()) <= 0) {
		EVP_PKEY_CTX_free(ctx);
		return -1;
	}

	this->decrypted_len = outlen;
	if (EVP_PKEY_verify_recover(ctx, this->decrypted, &outlen,
								reinterpret_cast<const unsigned char*>(encrypted_data.data()),
								encrypted_data.size()) <= 0) {
		EVP_PKEY_CTX_free(ctx);
		return -1;
	}

	EVP_PKEY_CTX_free(ctx);
	return this->decrypted_len;	
}


/////////////////////////////////////////////////////////////////////////////////
/*

*/
/////////////////////////////////////////////////////////////////////////////////


bool rsa::initialize_with_key(const std::string &key, int keyType)
{
	BIO *keybio = BIO_new_mem_buf(key.data(), -1);
	if (keyType == PUBLICKEY) 
	{
		// this->pRSA = PEM_read_bio_RSA_PUBKEY(keybio, &this->pRSA, nullptr, nullptr);
		this->pkey = PEM_read_bio_PUBKEY(keybio, nullptr, nullptr, nullptr);
	} 
	else if (keyType == PRIVATEKEY) 
	{
		// this->pRSA = PEM_read_bio_RSAPrivateKey(keybio, &this->pRSA, nullptr, nullptr);
		this->pkey = PEM_read_bio_PrivateKey(keybio, nullptr, nullptr, nullptr);
	}

	BIO_free(keybio);
	if (!pkey)
	{
		return false;
    }

	// Извлекаем RSA*
	/*
	RSA *rsa_key = EVP_PKEY_get1_RSA(pkey);
	EVP_PKEY_free(pkey);

	if (!rsa_key) 
	{
		return false;
	}

	this->pRSA = rsa_key;
	*/

	return true;
}


/////////////////////////////////////////////////////////////////////////////////
/*

*/
/////////////////////////////////////////////////////////////////////////////////


std::string rsa::base64_encode(const unsigned char* buffer, size_t length)
{
	BIO *bio, *b64;
	BUF_MEM *buffer_ptr;

	b64 = BIO_new(BIO_f_base64());
	bio = BIO_new(BIO_s_mem());
	bio = BIO_push(b64, bio);

	BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); // Без новой строки
	BIO_write(bio, buffer, length);
	BIO_flush(bio);
	BIO_get_mem_ptr(bio, &buffer_ptr);

	std::string base64_str(buffer_ptr->data, buffer_ptr->length);
	BIO_free_all(bio);

	return base64_str;
}


std::string rsa::base64_decode(const std::string& encoded_data)
{
	BIO *bio, *b64;
	int decode_len = encoded_data.length();
	std::vector<unsigned char> decoded_data(decode_len);

	bio = BIO_new_mem_buf(encoded_data.data(), -1);
	b64 = BIO_new(BIO_f_base64());
	bio = BIO_push(b64, bio);

	BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); // Без новой строки
	decode_len = BIO_read(bio, decoded_data.data(), decode_len);
	decoded_data.resize(decode_len); // Обрезаем до фактического размера
	BIO_free_all(bio);

	return std::string(decoded_data.begin(), decoded_data.end());
}







/*
RSA * rsa::createRSA(unsigned char * key,int _public)
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

	//BIO_set_close(keybio, BIO_NOCLOSE); // So BIO_free() leaves BUF_MEM alone
	//BIO_free(keybio);

	return rsa;
};
*/


/*
std::string rsa::unpack_key(const std::string& base64Key, int keyType) 
{
	// Декодируем base64
	BIO *bio, *b64;
	char *decodedData;
	long decodedLength;

	bio = BIO_new_mem_buf(base64Key.data(), base64Key.size());
	b64 = BIO_new(BIO_f_base64());
	bio = BIO_push(b64, bio);

	// Отключаем автоматическое добавление переноса строки
	BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);

	// Выделяем буфер для хранения декодированных данных
	decodedData = (char *)malloc(base64Key.size());
	decodedLength = BIO_read(bio, decodedData, base64Key.size());

	BIO_free_all(bio);

	if (decodedLength <= 0) {
		std::cerr << "Error: Failed to decode base64 key" << std::endl;
		free(decodedData);
		return "";
	}

	std::string decodedKey(decodedData, decodedLength);
	free(decodedData);

	BIO* keybio = BIO_new_mem_buf(decodedKey.data(), decodedKey.size());
	RSA* rsa = nullptr;

	if (keyType == PUBLICKEY) {
		rsa = PEM_read_bio_RSA_PUBKEY(keybio, &rsa, nullptr, nullptr);
		if (!rsa) {
			std::cerr << "Error: Failed to create RSA public key" << std::endl;
		}
	} else if (keyType == PRIVATEKEY) {
		rsa = PEM_read_bio_RSAPrivateKey(keybio, &rsa, nullptr, nullptr);
		if (!rsa) {
			std::cerr << "Error: Failed to create RSA private key" << std::endl;
		}
	}

	BIO_free(keybio);

	if (!rsa) {
		return "";
	}

	BIO* outputBio = BIO_new(BIO_s_mem());
	if (keyType == PUBLICKEY) {
		if (!PEM_write_bio_RSA_PUBKEY(outputBio, rsa)) {
			std::cerr << "Error: Failed to write RSA public key" << std::endl;
		}
	} else if (keyType == PRIVATEKEY) {
		if (!PEM_write_bio_RSAPrivateKey(outputBio, rsa, nullptr, nullptr, 0, nullptr, nullptr)) {
			std::cerr << "Error: Failed to write RSA private key" << std::endl;
		}
	}

	RSA_free(rsa);

	char* pemKey = nullptr;
	long pemLength = BIO_get_mem_data(outputBio, &pemKey);
	std::string unpackedKey(pemKey, pemLength);

	BIO_free(outputBio);
	return unpackedKey;
}
*/

}	// END namespace crypt
}	// END namespace tegia




