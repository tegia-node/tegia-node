#ifndef H_TEGIA_AI_PROMPT
#define H_TEGIA_AI_PROMPT

#include <tegia/tegia.h>

namespace tegia {
namespace ai {

class prompt_t
{
	public:
		prompt_t();
		~prompt_t();

		void init();
		nlohmann::json get();

		void prompt(const std::string &filename);
		void appendix(const std::string &prefix, nlohmann::json data);
		void appendix(const std::string &prefix, const std::string &data);
		void appendix_file(const std::string &prefix, const std::string &filename);

	private:
		std::string load_file_content(const std::string &filename);
		nlohmann::json _prompt;
};

}  // END namespace ai
}  // END namespace tegia

#endif
