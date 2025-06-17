cmake_minimum_required(VERSION 3.20)

set(PROJECT_NAME SupertoneAssignment)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/out")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

option(QT_FRONTEND_OPTION "Frontend QT Application" ON)

project(${PROJECT_NAME} VERSION 1.0.0.0)

add_subdirectory(JUCEModule)
add_subdirectory(Common)
add_subdirectory(ReverbApp)

if (QT_FRONTEND_OPTION)
message(STATUS "Building QT Frontend")
add_subdirectory(Frontend-qt)
endif()