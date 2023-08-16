#include <tegia/core/person.h>



namespace tegia {
namespace person {

names * names::_self = nullptr;

names * names::instance()
{
	if(!_self)
	{
		_self = new names();
		_self->load();
	}
	return _self;
};

names::names(){};

names::~names(){};


void names::load()
{
	std::cout << "LOAD NAMES" << std::endl;

	auto res = tegia::mysql::query("core","SELECT * FROM `names` WHERE `valid` = 1;");
	if(res->code != 200)
	{
		std::cout << _ERR_TEXT_ << "query error = " << res->code << std::endl;
		std::cout << "SELECT * FROM `names` WHERE `valid` = 1;" << std::endl;
		exit(0);
	}

	for(long long int i = 0; i < res->count(); ++i)
	{
		this->_names.insert({res->get("names::rus_name",i),core::cast<int>(res->get("names::sex",i))});
	}

	delete res;
};


std::tuple<std::string,int> names::get(const std::string &name)
{
	auto pos = this->_names.find(name);
	if(pos != this->_names.end())
	{
		return std::make_tuple(pos->first,pos->second);
	}
	else
	{
		// TODO: Записать имя в БД
	}

	return std::make_tuple(name,0);
};



/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
/////////////////////////////////////////////////////////////////////////////////////////////////////

std::tuple<int,std::string,std::string,std::string> parse_fio(const std::string &fio)
{
	std::tuple<int,std::string,std::string,std::string> result;

	std::string surname = "";
	std::string name = "";
	std::string patronymic = "";

	// Разбиваем полную строку ФИО на компоненты
	auto arr = core::explode(fio," ",false);
	switch(arr.size())
	{
		case 3:
		{
			result = std::make_tuple(1,arr[0],arr[1],arr[2]);
		}
		break;

		case 4:
		{
			result = std::make_tuple(0,"","","");
			
			// Абилов Ильяс Сырадж оглы
			if( tegia::person::normal_name(arr[3]) == "Оглы")
			{
				result = std::make_tuple(1,arr[0],arr[1],arr[2] + " Оглы");
				break;
			}

			if( tegia::person::normal_name(arr[3]) == "Кызы")
			{
				result = std::make_tuple(1,arr[0],arr[1],arr[2] + " Кызы");
				break;
			}
		}
		break;

		default:
		{
			result = std::make_tuple(0,"","","");
		}
		break;
	}

	return result;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
/////////////////////////////////////////////////////////////////////////////////////////////////////

// utility wrapper to adapt locale-bound facets for wstring/wbuffer convert
template<class Facet>
struct deletable_facet : Facet
{
		template<class ...Args>
		deletable_facet(Args&& ...args) : Facet(std::forward<Args>(args)...) {}
		~deletable_facet() {}
};

std::string normal_name(const std::string &name)
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
				u32str[i] = _eng_to_rus(u32str[i]);
				u32str[i] = _up(u32str[i]);
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
					u32str[i] = _eng_to_rus(u32str[i]);
					u32str[i] = _low(u32str[i]);
					
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


/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
/////////////////////////////////////////////////////////////////////////////////////////////////////


int check_sex(const std::string &patronymic)
{
	std::u32string _patronymic = core::cast<std::u32string>(patronymic);
	size_t len = _patronymic.length();

	if(len < 2)
	{
		return 0;
	}

	//std::cout << core::cast<std::string>(_patronymic.substr(len-2)) << std::endl;
	//std::cout << core::cast<std::string>(_patronymic.substr(len-4)) << std::endl;

	if(len < 2)
	{
		return 0;
	}

	if(_patronymic.substr(len-2) == U"ич")
	{
		return 1;
	}

	if(_patronymic.substr(len-2) == U"на")
	{
		return 2;
	}

	if(len < 4)
	{
		return 0;
	}

	if(_patronymic.substr(len-4) == U"Оглы")
	{
		return 1;
	}

	if(_patronymic.substr(len-4) == U"Кызы")
	{
		return 2;
	}

	return 0;
};



/*
[ERR] unknown sex
    surname    = Есаян
    name       = Гагик
    patronymic = Самвели
*/


int check_sex(const std::string &name, const std::string &patronymic)
{
	std::u32string _patronymic = core::cast<std::u32string>(patronymic);
	size_t len = _patronymic.length();

	auto names = tegia::person::names::instance();
	
	std::string _name = "";
	int name_sex = 0;
	std::tie(_name,name_sex) = names->get(name);

	if(len < 2)
	{
		if(name_sex > 0)
		{
			return name_sex;
		}
		return 0;
	}

	if(_patronymic.substr(len-2) == U"ич")
	{
		/*
		if(name_sex == 1)
		{
			return 1;
		}
		*/
		return 1;
	}

	if(_patronymic.substr(len-2) == U"на")
	{
		/*
		if(name_sex == 2)
		{
			return 2;
		}
		*/
		return 2;
	}

	if(len < 4)
	{
		if(name_sex > 0)
		{
			return name_sex;
		}
		return 0;
	}

	if(_patronymic.substr(len-4) == U"Оглы")
	{
		/*
		if(name_sex == 1)
		{
			return 1;
		}
		*/
		return 1;
	}

	if(_patronymic.substr(len-4) == U"Кызы")
	{
		/*
		if(name_sex == 2)
		{
			return 2;
		}
		*/
		return 2;
	}

	if(name_sex > 0)
	{
		return name_sex;
	}
	return 0;

};



}	// END namespace person
}	// END namespace tegia