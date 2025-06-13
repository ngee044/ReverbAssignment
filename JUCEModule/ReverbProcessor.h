#pragma once
#include "JHeaders.h"
#include "AudioFile.h"

namespace JUCEModule
{
	struct ReverbSettings
	{
		float room_size_ = 0.5f;
		float damping_ = 0.5f;
		float wet_level_ = 0.33f;
		float dry_level_ = 0.4f;
		float width_ = 1.0f;
	};

	class ReverbProcessor
	{
	public:
		ReverbProcessor() = default;
		~ReverbProcessor() = default;

		auto process_file(const std::string &input_file, const std::string &output_file, const ReverbSettings &settings) -> std::tuple<bool, std::optional<std::string>>;
	};

}