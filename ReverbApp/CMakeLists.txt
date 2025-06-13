cmake_minimum_required(VERSION 3.18)

set(PROGRAM_NAME ReverbApp)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(SOURCE_FILES
	main.cpp
)

set(HEADER_FILES 
)

project(${PROGRAM_NAME} VERSION 1.0.0.0)

add_executable(${PROGRAM_NAME} ${HEADER_FILES} ${SOURCE_FILES})

target_link_libraries(${PROGRAM_NAME} PUBLIC JUCEModule Common)
target_include_directories(${PROGRAM_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")


