#include "ReverbProcessor.h"

namespace JUCEModule
{
	static auto convert_settings(const ReverbSettings &s) -> juce::dsp::Reverb::Parameters
	{
		juce::dsp::Reverb::Parameters p;
		p.roomSize = s.room_size_ / 100;
		p.damping = s.damping_ / 100;
		p.wetLevel = s.wet_level_ / 100;
		p.dryLevel = s.dry_level_ / 100;
		p.width = s.width_ / 100;
		return p;
	}

	auto ReverbProcessor::process_file(const std::string &input_file, const std::string &output_file, const ReverbSettings &settings) -> std::tuple<bool, std::optional<std::string>>
	{
		juce::File input_file_obj(input_file);
		juce::File output_file_obj(output_file);

		juce::AudioBuffer<float> buffer;
		double sample_rate = 0.0;

		auto [ok, err] = AudioFile::load_to_buffer(input_file_obj, buffer, sample_rate);

		if (!ok)
		{
			return {false, err};
		}

		juce::dsp::Reverb reverb;
		reverb.setParameters(convert_settings(settings));

		const int total_samples = buffer.getNumSamples();
		const int step_size = 4096;
		int processed = 0;

		while (processed < total_samples)
		{
			int block_len = juce::jmin(step_size, total_samples - processed);
			juce::dsp::AudioBlock<float> block(buffer.getArrayOfWritePointers(),
											   (size_t)buffer.getNumChannels(),
											   (size_t)processed,
											   (size_t)block_len);

			juce::dsp::ProcessContextReplacing<float> ctx(block);
			reverb.process(ctx);

			processed += block_len;
		}

		return AudioFile::write_from_buffer(output_file_obj, buffer, sample_rate);
	}
}