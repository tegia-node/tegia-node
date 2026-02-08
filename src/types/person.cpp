#include <tegia/types/person.h>
#include <tegia/types/simple/gender.h>
#include <tegia/types/simple/date.h>

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



//
//
//

/*
	1 - мужской
	2 - женский
	0 - не определен
*/


int _g_name(const std::string &name)
{
	//
	//
	//

	if(name == "Айказ") return 1;
	if(name == "Бахтияр") return 1;
	if(name == "Борис") return 1;
	if(name == "Николай") return 1;
	if(name == "Павел") return 1;
	if(name == "Роман") return 1;
	if(name == "Сергей") return 1;
	if(name == "Александр") return 1;
	if(name == "Андрей") return 1;
	if(name == "Антон") return 1;
	if(name == "Вадим") return 1;
	if(name == "Валерий") return 1;
	if(name == "Василий") return 1;
	if(name == "Виктор") return 1;
	if(name == "Виталий") return 1;
	if(name == "Владимир") return 1;
	if(name == "Владислав") return 1;
	if(name == "Георгий") return 1;
	if(name == "Даниил") return 1;
	if(name == "Дмитрий") return 1;
	if(name == "Егор") return 1;
	if(name == "Иван") return 1;
	if(name == "Константин") return 1;
	if(name == "Леонид") return 1;
	if(name == "Максим") return 1;
	if(name == "Михаил") return 1;
	if(name == "Никита") return 1;
	if(name == "Олег") return 1;
	if(name == "Руслан") return 1;
	if(name == "Станислав") return 1;
	if(name == "Тимофей") return 1;
	if(name == "Юрий") return 1;
	if(name == "Ярослав") return 1;
	if(name == "Алан") return 1;
	if(name == "Али") return 1;
	if(name == "Альберт") return 1;
	if(name == "Артур") return 1;
	if(name == "Вячеслав") return 1;
	if(name == "Григорий") return 1;
	if(name == "Давид") return 1;
	if(name == "Евгений") return 1;
	if(name == "Захар") return 1;
	if(name == "Игорь") return 1;
	if(name == "Лев") return 1;
	if(name == "Марат") return 1;
	if(name == "Рустам") return 1;
	if(name == "Тимур") return 1;
	if(name == "Эдуард") return 1;
	if(name == "Эльдар") return 1;
	if(name == "Эмиль") return 1;
	if(name == "Максат") return 1;
	if(name == "Ян") return 1;
	if(name == "Азамат") return 1;
	if(name == "Аскар") return 1;
	if(name == "Богдан") return 1;
	if(name == "Валентин") return 1;
	if(name == "Валерий") return 1;
	if(name == "Вениамин") return 1;
	if(name == "Викентий") return 1;
	if(name == "Виссарион") return 1;
	if(name == "Владлен") return 1;
	if(name == "Геннадий") return 1;
	if(name == "Демид") return 1;
	if(name == "Елисей") return 1;
	if(name == "Зиновий") return 1;
	if(name == "Карен") return 1;
	if(name == "Кирилл") return 1;
	if(name == "Лука") return 1;
	if(name == "Мирослав") return 1;
	if(name == "Назар") return 1;
	if(name == "Платон") return 1;
	if(name == "Радмир") return 1;
	if(name == "Родион") return 1;
	if(name == "Савелий") return 1;
	if(name == "Святослав") return 1;
	if(name == "Семён") return 1;
	if(name == "Спартак") return 1;
	if(name == "Тарас") return 1;
	if(name == "Фёдор") return 1;
	if(name == "Ахмад") return 1;

	//
	//
	//

	if(name == "Анна") return 2;
	if(name == "Алина") return 2;
	if(name == "Екатерина") return 2;
	if(name == "Елена") return 2;
	if(name == "Ирина") return 2;
	if(name == "Марина") return 2;
	if(name == "Мария") return 2;
	if(name == "Ольга") return 2;
	if(name == "Татьяна") return 2;
	if(name == "Юлия") return 2;
	if(name == "Яна") return 2;
	if(name == "Аделина") return 2;
	if(name == "Анастасия") return 2;
	if(name == "Анжелика") return 2;
	if(name == "Валентина") return 2;
	if(name == "Валерия") return 2;
	if(name == "Вероника") return 2;
	if(name == "Виктория") return 2;
	if(name == "Галина") return 2;
	if(name == "Дарья") return 2;
	if(name == "Евгения") return 2;
	if(name == "Зоя") return 2;
	if(name == "Инна") return 2;
	if(name == "Карина") return 2;
	if(name == "Ксения") return 2;
	if(name == "Лариса") return 2;
	if(name == "Людмила") return 2;
	if(name == "Надежда") return 2;
	if(name == "Наталья") return 2;
	if(name == "Светлана") return 2;
	if(name == "София") return 2;
	if(name == "Юлиана") return 2;
	if(name == "Агата") return 2;
	if(name == "Агния") return 2;
	if(name == "Алёна") return 2;
	if(name == "Алла") return 2;
	if(name == "Амелия") return 2;
	if(name == "Варвара") return 2;
	if(name == "Василиса") return 2;
	if(name == "Вера") return 2;
	if(name == "Влада") return 2;
	if(name == "Клара") return 2;
	if(name == "Марьям") return 2;
	if(name == "Нина") return 2;
	if(name == "Римма") return 2;
	if(name == "Роза") return 2;
	if(name == "Самира") return 2;
	if(name == "Снежана") return 2;
	if(name == "Эльвира") return 2;
	if(name == "Элина") return 2;
	if(name == "Эльмира") return 2;
	if(name == "Эльнара") return 2;
	if(name == "Эмилия") return 2;
	if(name == "Иймонахон") return 2;

	//
	//
	//

	if(name == "Туан") return 0;
	if(name == "Карденас") return 0;
	if(name == "Казимерас") return 0;

	//
	//
	//

	std::cout << _ERR_TEXT_ << "name" << std::endl;
	std::cout << name << std::endl;
	// exit(0);

	return 0;
};



int person_t::gender(const std::string &name, const std::string &patronymic)
{
	auto _patronymic = tegia::string::str_to_u32str(patronymic);
	if(_patronymic.size() >= 3)
	{
		auto suffix = _patronymic.substr(_patronymic.size() - 3);

		if(suffix == U"вич") return 1;
		if(suffix == U"ьич") return 1;
		if(suffix == U"мич") return 1;
		if(suffix == U"тич") return 1;

		if(suffix == U"вна") return 2;
		if(suffix == U"чна") return 2;

		if(_patronymic.size() >= 4)
		{
			auto suffix2 = _patronymic.substr(_patronymic.size() - 4);

			if(suffix2 == U"оглы") return 1;	
			if(suffix2 == U"Оглы") return 1;	

			if(suffix2 == U"кызы") return 2;
			if(suffix2 == U"Кызы") return 2;
			if(suffix2 == U"кизи") return 2;
			if(suffix2 == U"Кизи") return 2;
		}


		//
		//
		//

		if(suffix == U"льз") return _g_name(name);
		if(suffix == U"лам") return _g_name(name);
		if(suffix == U"рто") return _g_name(name);
		if(suffix == U"ене") return _g_name(name);
		if(suffix == U"эла") return _g_name(name);
		if(suffix == U"кар") return _g_name(name);
		if(suffix == U"куб") return _g_name(name);
		if(suffix == U"рис") return _g_name(name);
		if(suffix == U"али") return _g_name(name);
		if(suffix == U"ова") return _g_name(name);
		if(suffix == U"рул") return _g_name(name);
		if(suffix == U"ике") return _g_name(name);
		if(suffix == U"лла") return _g_name(name);
		if(suffix == U"сул") return _g_name(name);
		if(suffix == U"суф") return _g_name(name);
		if(suffix == U"эль") return _g_name(name);
		if(suffix == U"овн") return _g_name(name);
		if(suffix == U"эвэ") return _g_name(name);
		if(suffix == U"Тум") return _g_name(name);
		if(suffix == U"аан") return _g_name(name);
		if(suffix == U"дит") return _g_name(name);
		if(suffix == U"оси") return _g_name(name);
		if(suffix == U"сам") return _g_name(name);
		if(suffix == U"мад") return _g_name(name);
		if(suffix == U"аил") return _g_name(name);
		if(suffix == U"мед") return _g_name(name);
		if(suffix == U"ина") return _g_name(name);
		if(suffix == U"сио") return _g_name(name);
		if(suffix == U"жер") return _g_name(name);
		if(suffix == U"хан") return _g_name(name);
		if(suffix == U"иси") return _g_name(name);
		if(suffix == U"Али") return _g_name(name);
		if(suffix == U"дин") return _g_name(name);
		if(suffix == U"ики") return _g_name(name);
		if(suffix == U"тор") return _g_name(name);
		if(suffix == U"рен") return _g_name(name);
		if(suffix == U"яна") return _g_name(name);
		if(suffix == U"улу") return _g_name(name);
		if(suffix == U"гиа") return _g_name(name);
		if(suffix == U"рия") return _g_name(name);
		if(suffix == U"улы") return _g_name(name);
		if(suffix == U"сус") return _g_name(name);
		if(suffix == U"тхи") return _g_name(name);
		if(suffix == U"еро") return _g_name(name);
		if(suffix == U"ель") return _g_name(name);
		if(suffix == U"чус") return _g_name(name);
		if(suffix == U"мзи") return _g_name(name);
		if(suffix == U"ков") return _g_name(name);
		if(suffix == U"изи") return _g_name(name);

		std::cout << name << std::endl;
		std::cout << patronymic << std::endl;
		std::cout << _patronymic.substr(_patronymic.size() - 3) << std::endl;

		// exit(0);
		return 0;
	}

	return 0;
};




}	// END namespace identifier
}	// END namespace tegia
