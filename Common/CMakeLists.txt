cmake_minimum_required(VERSION 3.18)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(LIBRARY_NAME Common)
project(${LIBRARY_NAME} VERSION 1.0.0.0)

if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
	set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

set(HEADER_FILES
	ArgumentParser.h
	Converter.h
)

set(SOURCE_FILES
	ArgumentParser.cpp
	Converter.cpp
)

add_library(${LIBRARY_NAME} ${HEADER_FILES} ${SOURCE_FILES})
target_include_directories(${LIBRARY_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

find_package(fmt CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS system filesystem json )

target_link_libraries(${LIBRARY_NAME} PUBLIC fmt::fmt-header-only ${CMAKE_DL_LIBS} Boost::filesystem Boost::system Boost::json)
