#pragma once
#include "JHeaders.h"
#include "AudioFile.h"

namespace JUCEModule
{
	struct ReverbSettings
	{
		float room_size_;
		float damping_;
		float wet_level_;
		float dry_level_;
		float width_;
	};

	class ReverbProcessor
	{
	public:
		ReverbProcessor() = default;
		~ReverbProcessor() = default;

		auto process_file(const std::string &input_file, const std::string &output_file, const ReverbSettings &settings) -> std::tuple<bool, std::optional<std::string>>;
	};

}