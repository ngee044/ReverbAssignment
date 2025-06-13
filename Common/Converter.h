#pragma once

#include <cstdint>
#include <locale>
#include <string>
#include <vector>

#define WSTRING_IS_U16STRING

namespace Common
{
	class Converter
	{
	public:
		static auto to_string(const std::u32string& value, std::locale target_locale = std::locale("")) -> std::string;
		static auto to_string(const std::u16string& value, std::locale target_locale = std::locale("")) -> std::string;
		static auto to_string(const std::wstring& value, std::locale target_locale = std::locale("")) -> std::string;
		static auto to_string(const std::vector<uint8_t>& value) -> std::string;

#ifdef WSTRING_IS_U16STRING
	private:
		static auto convert(const std::u16string& value) -> std::wstring;
		static auto convert(const std::wstring& value) -> std::u16string;
#endif
	};
}
