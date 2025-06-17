cmake_minimum_required(VERSION 3.24)

set(PROGRAM_NAME QtFrontendApp)

project(${PROGRAM_NAME} VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Core Multimedia)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Core Multimedia)

set(SOURCES
    main.cpp

    Core/ReverbSetting.hpp

    Widgets/DragDropWidget.cpp
    Widgets/DragDropWidget.h

	Widgets/MainWindow.cpp
	Widgets/MainWindow.h
	Widgets/MainWindow.ui
)

add_executable(${PROGRAM_NAME} WIN32 ${SOURCES})

if (WIN32)
    target_link_options(${PROGRAM_NAME} PRIVATE
        $<$<CONFIG:Release>:/SUBSYSTEM:WINDOWS>
    )

    if (QT_VERSION_MAJOR EQUAL 5 AND TARGET Qt5::WinMain)
        target_link_libraries(${PROGRAM_NAME} PRIVATE
            $<$<CONFIG:Release>:Qt5::WinMain>)
    elseif(QT_VERSION_MAJOR EQUAL 6 AND TARGET Qt6::EntryPoint)
        target_link_libraries(${PROGRAM_NAME} PRIVATE
            $<$<CONFIG:Release>:Qt6::EntryPoint>)
    endif()
endif()

target_include_directories(${PROGRAM_NAME} PRIVATE Core QWidgets Widgets Core)
target_link_libraries(${PROGRAM_NAME} PRIVATE 
    Qt${QT_VERSION_MAJOR}::Widgets 
    Qt${QT_VERSION_MAJOR}::Core 
    Qt${QT_VERSION_MAJOR}::Multimedia)