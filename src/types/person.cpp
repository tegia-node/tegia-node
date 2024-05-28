#include <tegia/types/person.h>
#include <tegia/types/gender.h>
#include <tegia/types/date.h>

#include <tegia/core/cast.h>
#include <tegia/core/string.h>

#include <tegia/db/mysql/mysql.h>

namespace tegia {
namespace types {


////////////////////////////////////////////////////////////////////////////////////////////
/**
		
*/   
////////////////////////////////////////////////////////////////////////////////////////////


person_t::person_t():base_t("person")
{
	
};


////////////////////////////////////////////////////////////////////////////////////////////
/**
		
*/   
////////////////////////////////////////////////////////////////////////////////////////////


std::string person_t::value() const
{
	return this->_uuid + " " + this->_inn + " " + this->_surname + " " + this->_name + " " + this->_patronymic + ", " + this->_t_birth;
};


////////////////////////////////////////////////////////////////////////////////////////////
/**
		
*/   
////////////////////////////////////////////////////////////////////////////////////////////


std::string person_t::hash() const
{
	return tegia::crypt::MD5u(this->_inn + this->_surname + this->_name + this->_patronymic + this->_t_birth);
};


////////////////////////////////////////////////////////////////////////////////////////////
/**
		
*/   
////////////////////////////////////////////////////////////////////////////////////////////


nlohmann::json person_t::json() const
{
	nlohmann::json tmp;
	tmp["uuid"] = this->_uuid;
	tmp["inn"] = this->_inn;
	tmp["surname"] = this->_surname;
	tmp["name"] = this->_name;
	tmp["patronymic"] = this->_patronymic;
	tmp["gender"] = this->_gender;
	tmp["t_birth"] = this->_t_birth;

	return std::move(tmp);
};

nlohmann::json person_t::graph() const
{
	nlohmann::json tmp;
	tmp["type"] = "person";
	tmp["value"] = this->json();

	return std::move(tmp);
};


////////////////////////////////////////////////////////////////////////////////////////////
/**
		
*/   
////////////////////////////////////////////////////////////////////////////////////////////

std::string person_t::normal(const std::string &name, int part)
{
	if(name == "")
	{
		return "";
	}

	if(part == 0)
	{
		return person_t::_normal(name);
	}

	std::string query = "SELECT * FROM `fio_aliases` WHERE `alias` = '" + tegia::mysql::strip(name) + "' AND part = " + core::cast<std::string>(part) + ";";
	auto res = tegia::mysql::query("enrich",query);
	if(res->count() == 1)
	{
		std::string _name = res->get("fio_aliases::value",0);
		delete res;
		return _name;
	}

	/*
	std::cout << _ERR_TEXT_ << std::endl;
	std::cout << "      part  = " << part << std::endl;
	std::cout << "      alias = '" << name << "'" << std::endl;
	exit(0);
	*/

	delete res;
	return person_t::_normal(name);
};



std::string person_t::_normal(const std::string &name)
{
	//std::cout << "   old [" << name << "]" << std::endl;
	
	std::wstring_convert<deletable_facet<std::codecvt<char32_t, char, std::mbstate_t>>, char32_t> conv32;
	std::u32string u32str = conv32.from_bytes(name);

	u32str.erase(std::remove(u32str.begin(), u32str.end(), '\n'), u32str.end());
	u32str.erase(std::remove(u32str.begin(), u32str.end(), '\r'), u32str.end());

	int state = 0;

	//  0 - начальное состояние - убираем пробелы в начале строки
	//  1 - состояние начала имени - нужно написать имя с прописной буквы
	//  2 - состояние середины имени - нужно писать строчные буквы
	//  3 - условная "середина" сложного имени тип Анна-Мария или Байрам Оглы
	//  4 - состояние окончания имени - нужно убрать концевые пробелы, если они есть

	int length = u32str.size();
	int i = 0;

	while(i < length)
	{
		switch(state)
		{
			case 0:
			{
				if(u32str[i] == U' ')
				{
					u32str.erase(0,1);
					i++;
				}
				else
				{
					state = 1;
				}
			}
			break;

			case 1:
			{
				// В написании имен встречаются варианты, когда русская буква заменена (ошибочно или специально) на похожую английскую
				u32str[i] = tegia::string::_eng_to_rus(u32str[i]);
				u32str[i] = tegia::string::_up(u32str[i]);
				i++;
				state = 2;
			}
			break;

			case 2:
			{
				if(u32str[i] == U' ' || u32str[i] == U'-')
				{
					// Условная "середина" сложного имени тип Анна-Мария или Байрам Оглы
					state = 3;
				}
				else
				{
					u32str[i] = tegia::string::_eng_to_rus(u32str[i]);
					u32str[i] = tegia::string::_low(u32str[i]);
					
					if(u32str[i] == U'ё')
					{
						u32str[i] = U'е';
					}

					i++;
				}
			}
			break;

			case 3:
			{
				// Находимся "в середине" имени, пока не получим новый символ
				if(u32str[i] == U' ' || u32str[i] == U'-')
				{
					i++;
				}
				else
				{
					state = 1;
				}
			}
			break;
		}
	}

	// Не забыть подчистить пробелы в конце имени
	while(true)
	{
		if(u32str.back() == U' ')
		{
			u32str.pop_back();
		}
		else
		{
			break;
		}
	}

	std::string name_ = conv32.to_bytes(u32str);

	//std::cout << "   new [" << name_ << "]" << std::endl;

	return name_;
};


////////////////////////////////////////////////////////////////////////////////////////////
/**
		
*/   
////////////////////////////////////////////////////////////////////////////////////////////

std::string person_t::uuid()
{
	return this->_uuid;
};

std::string person_t::inn()
{
	return this->_inn;
};

std::string person_t::surname()
{
	return this->_surname;
};

std::string person_t::name()
{
	return this->_name;
};

std::string person_t::patronymic()
{
	return this->_patronymic;
};

std::string person_t::t_birth()
{
	return this->_t_birth;
};


////////////////////////////////////////////////////////////////////////////////////////////
/**
		
*/   
////////////////////////////////////////////////////////////////////////////////////////////


int person_t::parse(const std::string &value, const nlohmann::json &validate)
{
	return this->parse(nlohmann::json::parse(value),validate);
};


////////////////////////////////////////////////////////////////////////////////////////////
/**

	0 - empty
	1 - ok
	2 - not valid

*/   
////////////////////////////////////////////////////////////////////////////////////////////


int person_t::parse(const nlohmann::json &data, const nlohmann::json &validate)
{
	// nlohmann::json data = nlohmann::json::parse(value);

	int flag = 0;

	//
	// INN
	//

	if(data.contains("inn") == true && data["inn"].get<std::string>() != "")
	{
		this->_inn = data["inn"].get<std::string>();
		flag = flag + 10000;
	}
	else
	{
		this->_inn = "";
	}

	//
	// SURNAME
	//

	if(data.contains("surname") == true && data["surname"].get<std::string>() != "")
	{
		this->_surname = tegia::types::person_t::normal(data["surname"].get<std::string>(),1);
		flag = flag + 1000;
	}
	else
	{
		this->_surname = "";
	}

	//
	// NAME
	//

	if(data.contains("name") == true && data["name"].get<std::string>() != "")
	{
		this->_name = tegia::types::person_t::normal(data["name"].get<std::string>(),2);
		flag = flag + 100;
	}
	else
	{
		this->_name = "";
	}

	//
	// PATRONYMIC
	//

	if(data.contains("patronymic") == true && data["patronymic"].get<std::string>() != "")
	{
		this->_patronymic = tegia::types::person_t::normal(data["patronymic"].get<std::string>(),3);
		flag = flag + 10;
	}
	else
	{
		this->_patronymic = "";
	}

	//
	// T_BIRTH
	//

	if(data.contains("t_birth") == true)
	{
		tegia::types::date_t t_birth;
		auto res = t_birth.parse(data["t_birth"].get<std::string>());

		if(res == 1)
		{
			this->_t_birth = t_birth.value();
			flag = flag + 1;
		}
		else
		{
			this->_t_birth = "";
		}
	}
	else
	{
		this->_t_birth = "";
	}


	//
	// GENDER
	//

	if(data.contains("gender") == true)
	{
		tegia::types::gender_t gender;
		auto res = gender.parse(data["gender"].get<std::string>());

		if(res == 1)
		{
			this->_gender = core::cast<int>(gender.value());
			// flag = flag + 1;
		}
		else
		{
			this->_gender = 0;
		}
	}
	else
	{
		this->_gender = 0;
	}


	switch(flag)
	{
		case 0:     return 0;
		case 1:     return 0;
		case 100:   return 0;
		case 1110:  return 0;
		case 1101:  return 1;
		case 1111:  return 1;
		case 11111: return 1;

		default:
		{
			std::cout << _ERR_TEXT_ << "flag = " << flag << std::endl;
			std::cout << "raw data   = " << data << std::endl;
			std::cout << "surname    = " << this->_surname << std::endl;
			std::cout << "name       = " << this->_name << std::endl;
			std::cout << "patronymic = " << this->_patronymic << std::endl;
			std::cout << "t_birth    = " << this->_t_birth << std::endl;

			exit(0);
		}
		break;
	}

	return 2;
};



}	// END namespace identifier
}	// END namespace tegia
