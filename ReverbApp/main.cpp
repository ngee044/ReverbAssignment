#include "AudioFile.h"
#include "ReverbProcessor.h"
#include "ArgumentParser.h"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <chrono>

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
auto init_logger(std::string log_file_name) -> void;

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
		auto t0 = std::chrono::steady_clock::now();
		ReverbProcessor processor;
		auto [result, error] = processor.process_file(read_file_.getFullPathName().toStdString(), write_file_.getFullPathName().toStdString(), settings_);
		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - t0).count();
		if (!result)
		{
			std::cout << "Error processing file: " << (error ? error.value() : "unknown error") << std::endl;
		}
		spdlog::info("{} → {} ({} ms)", read_file_.getFullPathName().toStdString(), write_file_.getFullPathName().toStdString(), ms);

		return jobHasFinished;
	}

	private:
		juce::File read_file_, write_file_;
		ReverbSettings settings_;
};


auto main(int argc, char* argv[]) -> int
{
	int file_count = 0;
	auto batch_start = std::chrono::steady_clock::now();

	std::string log_file_name = std::string("ReverbApp_") + std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) + ".log";
	init_logger(log_file_name);

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
		file_count = input_files.size();

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
		std::vector<ReverbJob*> jobs;

		for (auto& file : input_files)
		{
			juce::File output_file = output_folder.getChildFile(file.getFileNameWithoutExtension() + "_reverb.wav");
			auto* job = new ReverbJob(file, output_file, settings);
			jobs.push_back(job);
			thread_pool.addJob(job, true);
		}

		for (auto* job : jobs)
		{
			thread_pool.waitForJobToFinish(job, 10000);
		}

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

		file_count = 1;
		std::cout << "Processing file: " << input_file_ << " to " << output_file_ << std::endl;
		std::cout << "Settings: "
			<< "Room Size: " << settings.room_size_ << ", "
			<< "Damping: " << settings.damping_ << ", "
			<< "Wet Level: " << settings.wet_level_ << ", "
			<< "Dry Level: " << settings.dry_level_ << ", "
			<< "Width: " << settings.width_ << std::endl;

		ReverbProcessor processor;
		auto t0 = std::chrono::steady_clock::now();
		auto [result, error] = processor.process_file(input_file_, output_file_, settings);
		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - t0).count();
		if (!result)
		{
			std::cout << "Error processing file: " << (error ? error.value() : "unknown error") << std::endl;
			spdlog::error("{} → {})", input_file_, error ? error.value() : "unknown");
			return -1;
		}
		
		spdlog::info("{} → {} ({} ms)", input_file_, output_file_, ms);

	}

	auto total_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - batch_start).count();
	spdlog::info("Total processing time: {} ms, files: {}", total_ms, file_count);
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

auto init_logger(std::string log_file_name) -> void
{
	auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(log_file_name, true);

	auto logger = std::make_shared<spdlog::logger>("RVB", file_sink);
	logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [thread %t] %v");
	spdlog::set_default_logger(logger);
	spdlog::set_level(spdlog::level::info);
	spdlog::flush_on(spdlog::level::info);

	spdlog::info("ReverbApp started with log file: {}", log_file_name);

}