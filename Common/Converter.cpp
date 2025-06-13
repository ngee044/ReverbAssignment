#include "Converter.h"

#include <codecvt>

#include "fmt/xchar.h"
#include "fmt/format.h"

namespace Common
{
	auto Converter::to_string(const std::u32string& value, std::locale target_locale) -> std::string
	{
		if (value.empty())
		{
			return std::string();
		}

		typedef std::codecvt<char32_t, char, mbstate_t> codecvt_t;
		codecvt_t const& codecvt = std::use_facet<codecvt_t>(target_locale);

		mbstate_t state = mbstate_t();

		std::vector<char> result((value.size() + 1) * codecvt.max_length());
		char32_t const* in_text = value.data();
		char* out_text = &result[0];

		codecvt_t::result condition = codecvt.out(state, in_text, in_text + value.size(), in_text, out_text, out_text + result.size(), out_text);

		return std::string(result.data());
	}

	auto Converter::to_string(const std::u16string& value, std::locale target_locale) -> std::string
	{
		if (value.empty())
		{
			return std::string();
		}

		typedef std::codecvt<char16_t, char, mbstate_t> codecvt_t;
		codecvt_t const& codecvt = std::use_facet<codecvt_t>(target_locale);

		mbstate_t state = mbstate_t();

		std::vector<char> result((value.size() + 1) * codecvt.max_length());
		char16_t const* in_text = value.data();
		char* out_text = &result[0];

		codecvt_t::result condition = codecvt.out(state, in_text, in_text + value.size(), in_text, out_text, out_text + result.size(), out_text);

		return std::string(result.data());
	}

	auto Converter::to_string(const std::wstring& value, std::locale target_locale) -> std::string
	{
#ifdef WSTRING_IS_U16STRING
		return to_string(convert(value));
#else
		if (value.empty())
		{
			return std::string();
		}

		typedef std::codecvt<wchar_t, char, mbstate_t> codecvt_t;
		codecvt_t const& codecvt = std::use_facet<codecvt_t>(target_locale);

		mbstate_t state = mbstate_t();

		std::vector<char> result((value.size() + 1) * codecvt.max_length());
		wchar_t const* in_text = value.data();
		char* out_text = &result[0];

		codecvt_t::result condition = codecvt.out(state, value.data(), value.data() + value.size(), in_text, &result[0], &result[0] + result.size(), out_text);

		return std::string(&result[0], out_text);
#endif
	}

	auto Converter::to_string(const std::vector<uint8_t>& value) -> std::string
	{
		if (value.empty())
		{
			return std::string();
		}

		// UTF-8 BOM
		if (value.size() >= 3 && value[0] == 0xef && value[1] == 0xbb && value[2] == 0xbf)
		{
			return std::string((char*)value.data() + 2, value.size() - 2);
		}

		// UTF-8 no BOM
		return std::string((char*)value.data(), value.size());
	}

#ifdef WSTRING_IS_U16STRING
	auto Converter::convert(const std::u16string& value) -> std::wstring { return std::wstring(value.begin(), value.end()); }

	auto Converter::convert(const std::wstring& value) -> std::u16string { return std::u16string(value.begin(), value.end()); }
#endif
}
