#include "AudioFile.h"
#include "ReverbProcessor.h"
#include "ArgumentParser.h"

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <signal.h>


using namespace JUCEModule;
using namespace Common;

auto register_signal(void) -> void;
auto signal_callback(int32_t signum) -> void;

auto parse_arguments(ArgumentParser& arguments) -> void;
auto setup_reverb_settings(ReverbSettings& settings) -> void;

float room_size_ = 0.7f;
float damping_ = 0.5f;
float wet_level_ = 0.5f;
float dry_level_ = 0.25f;
float width_ = 1.0f;

unsigned int thread_min_count_ = 2;
unsigned int thread_max_count_ = 5;

std::string input_file_ = "";
std::string output_file_ = "";

std::string input_folder_ = "";
std::string output_folder_ = "";

bool async_ = true;

class ReverbJob : public juce::ThreadPoolJob
{
	public:
	ReverbJob(juce::File input_file, juce::File output_file, const ReverbSettings& settings)
		: juce::ThreadPoolJob("ReverbJob"), read_file_(std::move(input_file)), write_file_(std::move(output_file)), settings_(settings) {}

	auto runJob() -> JobStatus override
	{
		ReverbProcessor processor;
		auto [result, error] = processor.process_file(read_file_.getFullPathName().toStdString(), write_file_.getFullPathName().toStdString(), settings_);
		if (!result)
		{
			std::cout << "Error processing file: " << (error ? error.value() : "unknown error") << std::endl;
		}

		return jobHasFinished;
	}

	private:
		juce::File read_file_, write_file_;
		ReverbSettings settings_;
};


auto main(int argc, char* argv[]) -> int
{
	ArgumentParser arguments(argc, argv);
	parse_arguments(arguments);

	if (input_folder_.empty() && output_folder_.empty())
	{
		async_ = false;
	}

	if (async_)
	{
		if (input_folder_.empty() || output_folder_.empty())
		{
			std::cout << "usage: ReverbApp --input_folder <input_folder> --output_folder <output_folder> "
						<< "[--room <v>] [--damp <v>] [--wet <v>] [--dry <v>] [--width <v>]\n";
			return -1;
		}

		juce::File input_folder(input_folder_);
		juce::File output_folder(output_folder_);

		if (!input_folder.isDirectory() || (!output_folder.isDirectory()))
		{
			std::cout << "Error: Input or output folder is not a valid directory.\n";
			return -1;
		}

		juce::Array<juce::File> input_files;
		input_folder.findChildFiles(input_files, juce::File::TypesOfFileToFind::findFiles, false, "*.wav;*.mp3");

		if (input_files.isEmpty())
		{
			std::cout << "No audio files found in the input folder.\n";
			return -1;
		}

		ReverbSettings settings;
		setup_reverb_settings(settings);

		std::cout << "Processing files in: " << input_folder_ << " to " << output_folder_ << std::endl;
		std::cout << "Settings: "
			<< "Room Size: " << settings.room_size_ << ", "
			<< "Damping: " << settings.damping_ << ", "
			<< "Wet Level: " << settings.wet_level_ << ", "
			<< "Dry Level: " << settings.dry_level_ << ", "
			<< "Width: " << settings.width_ << std::endl;

		const unsigned int hardware_threads = std::thread::hardware_concurrency();
		auto thread_count = std::clamp(hardware_threads, thread_min_count_, thread_max_count_);
		juce::ThreadPool thread_pool(thread_count);

		for (auto& file : input_files)
		{
			//std::cout << "Adding job for file: " << file.getFullPathName() << std::endl;
			juce::File output_file = output_folder.getChildFile(file.getFileNameWithoutExtension() + "_reverb.wav");
			//std::cout << "Output file: " << output_file.getFullPathName() << std::endl;
			std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Simulate some delay for job submission
			thread_pool.addJob(new ReverbJob(file, output_file, settings), true);
		}

		thread_pool.removeAllJobs(true, 30000); // Wait for jobs to finish with a timeout of 30 seconds
		std::cout << "All files processed.\n";
		if (thread_pool.getNumJobs() > 0)
		{
			std::cout << "Some jobs did not finish in time.\n";
		}
		else
		{
			std::cout << "All jobs completed successfully.\n";
		}
	}
	else
	{
		if (input_file_.empty() || output_file_.empty())
		{
			std::cout << "usage: ReverbApp --input_file <input.wav> --output_file <output.wav> "
						"[--room <v>] [--damp <v>] [--wet <v>] [--dry <v>] [--width <v>]\n";
			return -1;
		}

		ReverbSettings settings;
		setup_reverb_settings(settings);


		std::cout << "Processing file: " << input_file_ << " to " << output_file_ << std::endl;
		std::cout << "Settings: "
			<< "Room Size: " << settings.room_size_ << ", "
			<< "Damping: " << settings.damping_ << ", "
			<< "Wet Level: " << settings.wet_level_ << ", "
			<< "Dry Level: " << settings.dry_level_ << ", "
			<< "Width: " << settings.width_ << std::endl;

		ReverbProcessor processor;
		auto [result, error] = processor.process_file(input_file_, output_file_, settings);
		if (!result)
		{
			std::cout << "Error processing file: " << (error ? error.value() : "unknown error") << std::endl;
			return -1;
		}
	}

	register_signal();

	return 0;
}

auto register_signal(void) -> void
{
	signal(SIGINT, signal_callback);
	signal(SIGILL, signal_callback);
	signal(SIGABRT, signal_callback);
	signal(SIGFPE, signal_callback);
	signal(SIGSEGV, signal_callback);
	signal(SIGTERM, signal_callback);
}

auto deregister_signal(void) -> void
{
	signal(SIGINT, nullptr);
	signal(SIGILL, nullptr);
	signal(SIGABRT, nullptr);
	signal(SIGFPE, nullptr);
	signal(SIGSEGV, nullptr);
	signal(SIGTERM, nullptr);
}

auto signal_callback(int32_t signum) -> void
{
	deregister_signal();
}

auto setup_reverb_settings(ReverbSettings& settings) -> void
{
	settings.room_size_ = room_size_;
	settings.damping_ = damping_;
	settings.wet_level_ = wet_level_;
	settings.dry_level_ = dry_level_;
	settings.width_ = width_;
}

auto parse_arguments(ArgumentParser& arguments) -> void
{
	auto target = arguments.to_string("--input_folder");
	if (target != std::nullopt)
	{
		input_folder_ = target.value();
	}

	target = arguments.to_string("--output_folder");
	if (target != std::nullopt)
	{
		output_folder_= target.value();
	}
	
	target = arguments.to_string("--input_file");
	if (target != std::nullopt)
	{
		input_file_ = target.value();
	}

	target = arguments.to_string("--output_file");
	if (target != std::nullopt)
	{
		output_file_ = target.value();
	}
	
	target = arguments.to_string("--room");
	if (target != std::nullopt)
	{
		room_size_ = (float)atof(target.value().c_str());
	}

	target = arguments.to_string("--damp");
	if (target != std::nullopt)
	{
		damping_ = (float)atof(target.value().c_str());
	}

	target = arguments.to_string("--wet");
	if (target != std::nullopt)
	{
		wet_level_ = (float)atof(target.value().c_str());
	}

	target = arguments.to_string("--dry");
	if (target != std::nullopt)
	{
		dry_level_ = (float)atof(target.value().c_str());
	}

	target = arguments.to_string("--width");
	if (target != std::nullopt)
	{
		width_ = (float)atof(target.value().c_str());
	}
}