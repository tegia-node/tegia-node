#include <tegia/types/simple/rus_company_inn.h>
#include <charconv>

namespace tegia {
namespace types {


////////////////////////////////////////////////////////////////////////////////////////////
/**
		
*/   
////////////////////////////////////////////////////////////////////////////////////////////


rus_company_inn_t::rus_company_inn_t():simple_t("rus_copmany_inn")
{
	// TODO: Задавать поле _code значением идентификатора ИНН ЮЛ как в системе
};


////////////////////////////////////////////////////////////////////////////////////////////
/**
		
	0 - empty
	1 - ok
	2 - not valid

*/   
////////////////////////////////////////////////////////////////////////////////////////////


int rus_company_inn_t::parse(const std::string &value, const nlohmann::json &validate)
{
	std::string inn = value;

	if (inn == "")
	{
		this->_value = "";
		this->_is_valid = false;
		return 0;
	}

	// Длина ИНН ЮЛ должна составлять 10 цифр
	static const size_t company_inn_size = 10;

	if (inn.size() != company_inn_size)
	{
		if (inn.size() != company_inn_size - 1)
		{
			this->_value = "";
			this->_is_valid = false;
			return 2;
		}

		// ИНН может не содержать первую цифру, если это 0
		inn.insert(0, "0");
	}

	// Алгоритм валидации ИНН ЮЛ: http://www.kholenkov.ru/data-validation/inn/
	std::vector<int> digits(company_inn_size);

	for (size_t i = 0; i < inn.size(); i++)
	{
		if (std::from_chars(inn.data() + i, inn.data() + i + 1, digits[i]).ptr != inn.data() + i + 1)
		{
			this->_value = "";
			this->_is_valid = false;
			return 2;
		}
	}

	static const std::vector<int> digit_weights{ 2,4,10,3,5,9,4,6,8 };
	int check_sum = 0;

	for (size_t i = 0; i < company_inn_size - 1; i++)
	{
		check_sum += digits[i] * digit_weights[i];
	}

	int check_number = (check_sum % 11) % 10;

	if (check_number == digits[9])
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