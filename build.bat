@echo off
setlocal enabledelayedexpansion

:: Store original directory
set "ORIGINAL_DIR=%CD%"

:: Clean and create build directory
if exist build (
    rmdir /s /q build
)
mkdir build
pushd build

:: Run CMake configuration
cmake .. ^
    -DCMAKE_TOOLCHAIN_FILE="..\..\vcpkg\scripts\buildsystems\vcpkg.cmake" ^
    -DCMAKE_BUILD_TYPE=Release

:: Build the project
cmake --build . --config Release

:: Set locale to C
set "LC_ALL=C"
set "LANGUAGE="

:: Return to original directory
popd
cd "%ORIGINAL_DIR%"

endlocal