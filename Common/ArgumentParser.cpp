#include "ArgumentParser.h"

#include "fmt/format.h"
#include "fmt/xchar.h"

#include "boost/dll.hpp"

#include "Converter.h"

#include <regex>
#include <algorithm>
#include <filesystem>

namespace Common
{
	ArgumentParser::ArgumentParser(int32_t argc, char* argv[]) { arguments_ = parse(argc, argv); }

	ArgumentParser::ArgumentParser(int32_t argc, wchar_t* argv[]) { arguments_ = parse(argc, argv); }

	auto ArgumentParser::program_folder(void) const -> std::string { return program_folder_; }

	auto ArgumentParser::program_name(void) const -> std::string { return program_name_; }

	auto ArgumentParser::to_string(const std::string& key) const -> std::optional<std::string>
	{
		auto target = arguments_.find(key);
		if (target == arguments_.end())
		{
			return std::nullopt;
		}

		return target->second;
	}

	auto ArgumentParser::parse(int32_t argc, char* argv[]) -> std::map<std::string, std::string>
	{
		std::filesystem::path program_path(argv[0]);
		program_name_ = boost::dll::program_location().string();
		program_folder_ = fmt::format("{}/", boost::dll::program_location().parent_path().string());

		std::vector<std::string> arguments;
		for (int32_t index = 1; index < argc; ++index)
		{
			arguments.push_back(argv[index]);
		}

		return parse(arguments);
	}

	auto ArgumentParser::parse(int32_t argc, wchar_t* argv[]) -> std::map<std::string, std::string>
	{
		std::filesystem::path program_path(argv[0]);
		program_name_ = boost::dll::program_location().string();
		program_folder_ = fmt::format("{}/", boost::dll::program_location().parent_path().string());

		std::vector<std::string> arguments;
		for (int32_t index = 1; index < argc; ++index)
		{
			arguments.push_back(Converter::to_string(argv[index]));
		}

		return parse(arguments);
	}

	auto ArgumentParser::parse(const std::vector<std::string>& arguments) -> std::map<std::string, std::string>
	{
		std::map<std::string, std::string> result;

		size_t argc = arguments.size();
		std::string argument_id;
		for (size_t index = 0; index < argc; ++index)
		{
			argument_id = arguments[index];
			size_t offset = argument_id.find("--", 0);
			if (offset != 0)
			{
				continue;
			}

			if (index + 1 >= argc)
			{
				break;
			}

			auto target = result.find(argument_id);
			if (target == result.end())
			{
				result.insert({ argument_id, arguments[index + 1] });
				++index;

				continue;
			}

			target->second = arguments[index + 1];
			++index;
		}

		return result;
	}
}
