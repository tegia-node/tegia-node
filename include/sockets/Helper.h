#pragma once

#ifndef B5B021AA_BA96_4A2A_8A1D_D5DC255CD792
#define B5B021AA_BA96_4A2A_8A1D_D5DC255CD792

#include <locale>
#include <optional>
#include <concepts>
#include <charconv>
#include <string>
#include <string_view>

namespace tegia::common
{

    template <typename T >
    concept arithmetic = std::is_arithmetic_v<T>;


    template <arithmetic T>
    std::from_chars_result from_chars(const std::string_view str, T& value) noexcept
    {
        const auto beg = str.data();
        const auto end = beg + str.size();
        return std::from_chars(beg, end, value);
    }

    template <arithmetic T>
    std::optional<T> try_parse(const std::string_view str) noexcept
    {
        T value{};
        const auto beg = str.data();
        const auto end = beg + str.size();
        if (const auto [ptr, ec] = std::from_chars(beg, end, value); ec == std::errc())
        {
            return std::make_optional<T>(value);
        }
        return std::nullopt;
    }


    template <arithmetic T>
    std::from_chars_result from_chars_remove_leading(const std::string_view str, T& value) noexcept
    {
        const auto beg = str.data();
        const auto end = beg + str.size();
        auto ptr = beg;
        while ((*ptr <= '0' || *ptr > '9') && *ptr != '-' && ptr != end) ++ptr;

        return (ptr != end) ? std::from_chars(ptr, end, value) : std::from_chars_result {beg, std::errc::invalid_argument};
    }


    template <arithmetic T>
    std::from_chars_result from_chars_remove_leading_blanks(const std::string_view str, T& value) noexcept
    {
        const auto beg = str.data();
        const auto end = beg + str.size();
        auto ptr = beg;
        while ((*ptr == ' ' || *ptr == '\t') && ptr != end) ++ptr;

        return (ptr != end) ? std::from_chars(ptr, end, value) : std::from_chars_result {beg, std::errc::invalid_argument};
    }

    template <arithmetic T>
    std::optional<T> try_parse_remove_leading_blanks(const std::string_view str) noexcept
    {
        T value{};
        if (const auto [ptr, ec] = from_chars_remove_leading_blanks(str, value); ec == std::errc())
        {
            return std::make_optional<T>(value);
        }
        return std::nullopt;
    }


    template<typename T>
    concept resizable = requires(T v, std::size_t n) { v.size(); v.resize(n); };


    template<typename std::size_t N, resizable ... Args>
    void remove_tail_characters(Args&... args) noexcept
    {
        (args.resize(args.size() > N ? args.size() - N : 0U), ...);
    }


    inline static auto const current_loc = std::locale();

    template<class CharT, class Traits = std::char_traits<CharT>>
    bool basic_compare_ic(CharT const * const ptr1, CharT const * const ptr2, typename std::basic_string_view<CharT, Traits>::size_type const len, std::locale loc) noexcept
    {
        auto const& facet = std::use_facet<std::ctype<CharT>>(loc);

        for (typename std::basic_string_view<CharT, Traits>::size_type i = 0U; i < len; ++i)
        {
            if (ptr1[i] != ptr2[i] && facet.tolower(ptr1[i]) != facet.tolower(ptr2[i])) return false;
        }

        return true;
    }
 
    template<class CharT, class Traits = std::char_traits<CharT>>
    bool basic_ends_with_ic(const std::basic_string_view<CharT, Traits> str, const std::basic_string_view<CharT, Traits> suffix, std::locale loc = current_loc) noexcept
    {
        typename std::basic_string_view<CharT, Traits>::size_type const len2 = suffix.size();
        typename std::basic_string_view<CharT, Traits>::size_type const len1 = str.size();
        if (len2 > len1) return false;

        return basic_compare_ic<CharT, Traits>(str.data() + len1 - len2, suffix.data(), len2, loc);
    }

    template<class CharT, class Traits = std::char_traits<CharT>>
    bool basic_starts_with_ic(const std::basic_string_view<CharT, Traits> str, const std::basic_string_view<CharT, Traits> prefix, std::locale loc = current_loc) noexcept
    {
        typename std::basic_string_view<CharT, Traits>::size_type const len = prefix.size();
        if (len > str.size()) return false;

        return basic_compare_ic<CharT, Traits>(str.data(), prefix.data(), len, loc);
    }

    template<class CharT, class Traits = std::char_traits<CharT>>
    bool basic_equal_ic(const std::basic_string_view<CharT, Traits> str1, const std::basic_string_view<CharT, Traits> str2, std::locale loc = current_loc) noexcept
    {
        typename std::basic_string_view<CharT, Traits>::size_type const len = str1.size();
        if (len != str2.size()) return false;

        return basic_compare_ic<CharT, Traits>(str1.data(), str2.data(), len, loc);
    }

    template<class CharT, class Traits = std::char_traits<CharT>>
    std::basic_string<CharT, Traits> to_lower(std::basic_string_view<CharT, Traits> const in_str, std::locale const loc = current_loc) noexcept
    {
        std::basic_string<CharT, Traits> out_str { in_str };
        typename std::basic_string<CharT, Traits>::size_type const len = out_str.size();

        auto const& facet = std::use_facet<std::ctype<typename std::basic_string<CharT, Traits>::value_type>>(current_loc);
        auto* const ptr = out_str.data();

        for (typename std::basic_string<CharT, Traits>::size_type i = 0U; i < len; ++i)
        {
            ptr[i] = facet.tolower(ptr[i]);
        }
        return out_str;
    }

    inline bool compare_ic(char const * const ptr1, char const * const ptr2, typename std::string_view::size_type const len) noexcept
    {
        return basic_compare_ic<char>(ptr1, ptr2, len, current_loc);
    }

    inline bool ends_with_ic(const std::string_view str, const std::string_view suffix) noexcept
    {
        typename std::string_view::size_type const len2 = suffix.size();
        typename std::string_view::size_type const len1 = str.size();
        if (len2 > len1) return false;

        return compare_ic(str.data() + len1 - len2, suffix.data(), len2);
    }

    inline bool starts_with_ic(const std::string_view str, const std::string_view prefix) noexcept
    {
        typename std::string_view::size_type const len = prefix.size();
        if (len > str.size()) return false;

        return compare_ic(str.data(), prefix.data(), len);
    }

    inline bool equal_ic(const std::string_view str1, const std::string_view str2) noexcept
    {
        typename std::string_view::size_type const len = str1.size();
        if (len != str2.size()) return false;

        return compare_ic(str1.data(), str2.data(), len);
    }

    inline std::string to_lower(const std::string_view in_str) noexcept
    {
        return to_lower<std::string::value_type>(in_str);
    }


    inline std::string mysql_escape_string(const std::string_view in_str) noexcept
    {
        std::string out_str{};
        out_str.reserve(in_str.size() + 16);
        for (auto const ch : in_str)
        {
            if (ch == '\\' || ch == '\'' || ch == '\r' || ch == '\n' || ch == '"' || ch == '\x00' || ch == '\x1a') out_str.push_back('\\');
            out_str.push_back(ch);
        }
        return out_str;
    }

    template<class CharT, class Traits = std::char_traits<CharT>>
    std::basic_string<CharT, Traits> basic_mysql_escape_string(const std::basic_string_view<CharT, Traits> in_str) noexcept
    {
        std::basic_string<CharT, Traits> out_str{};
        out_str.reserve(in_str.size() + 16);
        for (auto const ch : in_str)
        {
            if (ch == CharT{'\\'} || ch == CharT{'\''} || ch == CharT{'"'} || 
                ch == CharT{'\r'} || ch == CharT{'\n'} || ch == CharT{'\x00'} || ch == CharT{'\x1a'}) 
                out_str.push_back('\\');
            out_str.push_back(ch);
        }
        return out_str;
    }

	//
	// Makes unordered map keys case insensitive
	//
	struct case_insensitive_unordered_map 
	{
		struct equal_to
		{
			bool operator() (const std::string& lhs, const std::string& rhs) const 
			{
				return equal_ic(lhs, rhs);
			}
		};

		struct hash : public std::hash<std::string>
		{
			std::size_t operator() (const std::string& str) const 
			{
				return std::hash<std::string>()(to_lower(str));
			}
		};
	};

    // Makes map keys case insensitive
    struct case_insensitive_map 
    {
        struct less 
        {
            bool operator() (const std::string& lhs, const std::string& rhs) const 
            {
                return strcasecmp(lhs.c_str(), rhs.c_str()) < 0;
            }
        };
    };
}


#endif /* B5B021AA_BA96_4A2A_8A1D_D5DC255CD792 */
