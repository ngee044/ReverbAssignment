cmake_minimum_required(VERSION 3.18)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(LIBRARY_NAME JUCEModule)

project(${LIBRARY_NAME} VERSION 1.0.0.0)

include(GenerateExportHeader)

if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
	set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

set(SOURCE_FILES
	AudioFile.cpp
    ReverbProcessor.cpp
)

set(HEADER_FILES
	AudioFile.h
	JHeaders.h
    ReverbProcessor.h
)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES} ${HEADER_FILES})

if (LINUX)
    find_package(mpg123 CONFIG REQUIRED)
    target_link_libraries(JUCEModule PUBLIC mpg123::libmpg123)
endif()

find_package(juce CONFIG REQUIRED)

include(GenerateExportHeader)
generate_export_header(${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)
target_compile_definitions(${PROJECT_NAME} PUBLIC
    JUCE_USE_MP3AUDIOFORMAT=1
    JUCE_GLOBAL_MODULE_SETTINGS_INCLUDED=1
    JUCE_STANDALONE_APPLICATION=1
    JUCE_PLUGINHOST_VST3=1
    JUCE_VST3_HOSTING=1
)

target_link_libraries(${PROJECT_NAME} PUBLIC
    juce::juce_core
    juce::juce_audio_basics
    juce::juce_audio_formats
    juce::juce_graphics
    juce::juce_events
    juce::juce_audio_utils
    juce::juce_audio_processors
    
    # For Linux, we need to link against the mpg123 library for MP3 support
    #mpg123::libmpg123
)