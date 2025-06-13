#include "AudioFile.h"

namespace JUCEModule
{
	static auto create_format_manager() -> std::unique_ptr<juce::AudioFormatManager>
	{
		auto manager = std::make_unique<juce::AudioFormatManager>();

		manager->registerBasicFormats();
#if JUCE_USE_MP3AUDIOFORMAT
		manager->registerFormat(new juce::MP3AudioFormat(), false);
#endif

		return manager;
	}

	auto AudioFile::load_to_buffer(const juce::File& file,
							juce::AudioBuffer<float>& buffer_out,
							double& sample_rate_out) -> std::tuple<bool, std::optional<std::string>>
	{
		auto manager = create_format_manager();

		std::unique_ptr<juce::AudioFormatReader> reader(manager->createReaderFor(file));

		if (reader == nullptr)
		{
			return {false, "unsupported or unreadable audio file"};
		}

		sample_rate_out = reader->sampleRate;
		buffer_out.setSize((int)reader->numChannels, (int)reader->lengthInSamples);
		reader->read(&buffer_out,
					0,
					(int)reader->lengthInSamples,
					0,
					true,
					true);

		return {true, std::nullopt};
	}

	auto AudioFile::write_from_buffer(const juce::File& file,
								const juce::AudioBuffer<float>& buffer,
								double sample_rate) -> std::tuple<bool, std::optional<std::string>>
	{
		if (file.existsAsFile())
		{
			auto delete_result = file.deleteFile();
			if (!delete_result)
			{
				return {false, "failed to delete existing file"};
			}
		}

		juce::WavAudioFormat format;
		std::unique_ptr<juce::FileOutputStream> stream(file.createOutputStream());

		if (stream == nullptr || !stream->openedOk())
		{
			return {false, "cannot open output file for writing"};
		}

		std::unique_ptr<juce::AudioFormatWriter> writer(
			format.createWriterFor(stream.get(),
								sample_rate,
								(unsigned int)buffer.getNumChannels(),
								24,
								{},
								0));

		if (writer == nullptr)
		{
			return {false, "failed to create WAV writer"};
		}

		stream.release();
		writer->writeFromAudioSampleBuffer(buffer, 0, buffer.getNumSamples());
		return {true, std::nullopt};
	}
}