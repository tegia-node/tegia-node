
#include <tegia/db/manticore/manticore.h>

#include <format>

#undef _LOG_LEVEL_
#define _LOG_LEVEL_ _LOG_WARNING_
#include <tegia/context/log.h>

namespace tegia {
namespace manticore {

    size_t generate_uuid()
    {
        std::string query = "SELECT UUID_SHORT() `id`";

        auto res = tegia::mysql::query("manticore", query);

        if (res->code != 200)
        {
            LERROR(std::format("Error '{}' occurred while executing the following query: {}", res->info, query));

            delete res;
            return 0;
        }

        size_t id = std::stoull(res->get("::id", 0));

        delete res;
        return id;
    }

    std::string escape(const std::string &str)
	{
        // Спецсимволы, которые должны быть экранированы в Manticore, указаны в документации:
        // https://manual.manticoresearch.com/Searching/Full_text_matching/Escaping#Escaping-characters-in-query-string
        
        std::string result;

        for (char c : str)
        {
            switch (c)
            {
                // Одинарная кавычка должна быть экранирована только одним слешом.
                case '\'':
                {
                    result += "\\" + c;
                    break;
                }
                // Другие спецсимволы должны быть экранированы двумя слешами.
                case '!':
                case '"':
                case '$':
                case '(':
                case ')':
                case '-':
                case '/':
                case '<':
                case '@':
                case '\\':
                case '^':
                case '|':
                case '~':
                {
                    result += "\\\\" + c;
                    break;
                }
                default:
                {
                    result += c;
                    break;
                }
            }
        }
        
        return result;
	};

}	// END namespace manticore
}	// END namespace tegia