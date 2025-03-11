#ifndef H_TEGIA_DB_MANTICORE
#define H_TEGIA_DB_MANTICORE

#include <tegia/db/mysql/mysql.h>

namespace tegia {
namespace manticore {

	/**
	 * @brief Функция генерации уникального идентификатора записи среди всех таблиц в Manticore.
	 * @return Уникальный идентификатор записи. При возникновении внутренней ошибки возвращает 0.
	 */
	size_t generate_uuid();
	
	/**
	 * @brief Функция экранирования спецсимволов в строках, используемых в запросах к Manticore.
	 * @param str Строка для экранировния.
	 * @return Экранированная строка.
	 */
	std::string escape(const std::string &str);

}	// END namespace manticore
}	// END namespace tegia

#endif