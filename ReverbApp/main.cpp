#include "AudioFile.h"
#include "ReverbProcessor.h"
#include "ArgumentParser.h"

#include <iostream>
#include <string>
#include <signal.h>


using namespace JUCEModule;
using namespace Common;

auto register_signal(void) -> void;
auto signal_callback(int32_t signum) -> void;

auto parse_arguments(ArgumentParser& arguments) -> void;

float room_size_ = 0.7f;
float damping_ = 0.5f;
float wet_level_ = 0.5f;
float dry_level_ = 0.25f;
float width_ = 1.0f;

std::string input_file_ = "";
std::string output_file_ = "";

auto main(int argc, char* argv[]) -> int
{
	ArgumentParser arguments(argc, argv);
	parse_arguments(arguments);

	if (input_file_.empty() || output_file_.empty())
	{
        std::cout << "usage: ReverbApp --input_file <input.wav> --output_file <output.wav> "
					 "[--room <v>] [--damp <v>] [--wet <v>] [--dry <v>] [--width <v>]\n";
		return -1;
	}

	ReverbSettings settings;
	settings.room_size_ = room_size_;
	settings.damping_ = damping_;
	settings.wet_level_ = wet_level_;
	settings.dry_level_ = dry_level_;
	settings.width_ = width_;

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


auto parse_arguments(ArgumentParser& arguments) -> void
{
	auto target = arguments.to_string("--input_file");
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