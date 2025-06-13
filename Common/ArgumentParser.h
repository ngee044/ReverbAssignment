#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <vector>
#include <optional>

namespace Common
{
	class ArgumentParser
	{
	public:
		ArgumentParser(int32_t argc, char* argv[]);
		ArgumentParser(int32_t argc, wchar_t* argv[]);

		auto program_name(void) const -> std::string;
		auto program_folder(void) const -> std::string;

		auto to_string(const std::string& key) const -> std::optional<std::string>;

	protected:
		auto parse(int32_t argc, char* argv[]) -> std::map<std::string, std::string>;
		auto parse(int32_t argc, wchar_t* argv[]) -> std::map<std::string, std::string>;

	private:
		auto parse(const std::vector<std::string>& arguments) -> std::map<std::string, std::string>;

	private:
		std::string program_name_;
		std::string program_folder_;
		std::map<std::string, std::string> arguments_;
	};
}
