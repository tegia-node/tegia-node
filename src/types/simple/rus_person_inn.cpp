#include <tegia/types/simple/rus_person_inn.h>
#include <charconv>

namespace tegia {
namespace types {


////////////////////////////////////////////////////////////////////////////////////////////
/**
		
*/   
////////////////////////////////////////////////////////////////////////////////////////////


rus_person_inn_t::rus_person_inn_t():simple_t("rus_person_inn")
{
	this->_code = 3643001;
};


////////////////////////////////////////////////////////////////////////////////////////////
/**
		
	0 - empty
	1 - ok
	2 - not valid

*/   
////////////////////////////////////////////////////////////////////////////////////////////


int rus_person_inn_t::parse(const std::string &value, const nlohmann::json &validate)
{
	std::string inn = value;

	if (inn == "")
	{
		this->_value = "";
		this->_is_valid = false;
		return 0;
	}

	// Длина ИНН ФЛ должна составлять 12 цифр
	static const size_t person_inn_size = 12;

	if (inn.size() != person_inn_size)
	{
		if (inn.size() != person_inn_size - 1)
		{
			this->_value = "";
			this->_is_valid = false;
			return 2;
		}

		// ИНН может не содержать первую цифру, если это 0
		inn.insert(0, "0");
	}

	// Алгоритм валидации ИНН ФЛ: http://www.kholenkov.ru/data-validation/inn/
	std::vector<int> digits(person_inn_size);

	for (size_t i = 0; i < value.size(); i++)
	{
		if (std::from_chars(inn.data() + i, inn.data() + i + 1, digits[i]).ptr != inn.data() + i + 1)
		{
			this->_value = "";
			this->_is_valid = false;
			return 2;
		}
	}

	static const std::vector<int> digit_weights1{ 7,2,4,10,3,5,9,4,6,8 };
	int check_sum = 0;

	for (size_t i = 0; i < person_inn_size - 2; i++)
	{
		check_sum += digits[i] * digit_weights1[i];
	}
	
	int check_number1 = (check_sum % 11) % 10;

	static const std::vector<int> digit_weights2{ 3,7,2,4,10,3,5,9,4,6,8 };
	check_sum = 0;

	for (size_t i = 0; i < person_inn_size - 1; i++)
	{
		check_sum += digits[i] * digit_weights2[i];
	}

	int check_number2 = (check_sum % 11) % 10;

	if (check_number1 == digits[10] && check_number2 == digits[11])
	{
		this->_value = inn;
		this->_is_valid = true;
		return 1;
	}
	else
	{
		this->_value = "";
		this->_is_valid = false;
		return 2;
	}
};

}	// END namespace types
}	// END namespace tegia