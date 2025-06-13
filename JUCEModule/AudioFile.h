#pragma once

#include "JHeaders.h"

#include <optional>
#include <tuple>
#include <functional>


namespace JUCEModule
{
	class AudioFile final
	{
	public:
		AudioFile() = default;
		~AudioFile()  = default;

		static auto load_to_buffer(const juce::File &file,
								juce::AudioBuffer<float> &buffer_out,
								double &sample_rate_out)
			-> std::tuple<bool, std::optional<std::string>>;

		static auto write_from_buffer(const juce::File &file,
									const juce::AudioBuffer<float> &buffer,
									double sample_rate)
			-> std::tuple<bool, std::optional<std::string>>;

		
	};
}