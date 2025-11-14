@echo off
REM Build script for QEasyWidgets Examples (Windows)

echo ========================================
echo QEasyWidgets Examples Build Script
echo ========================================
echo.

REM Check if build directory exists
if exist build (
    echo Build directory exists. Cleaning...
    rmdir /s /q build
)

REM Create build directory
echo Creating build directory...
mkdir build
cd build

REM Configure with CMake
echo.
echo Configuring with CMake...
echo.

REM Try to find Qt automatically, or use user-specified path
if defined Qt6_DIR (
    echo Using Qt6_DIR: %Qt6_DIR%
    cmake .. -DCMAKE_PREFIX_PATH=%Qt6_DIR%
) else if defined Qt5_DIR (
    echo Using Qt5_DIR: %Qt5_DIR%
    cmake .. -DCMAKE_PREFIX_PATH=%Qt5_DIR%
) else (
    echo No Qt path specified. Attempting automatic detection...
    cmake ..
)

if %ERRORLEVEL% neq 0 (
    echo.
    echo ========================================
    echo ERROR: CMake configuration failed!
    echo ========================================
    echo.
    echo Please set Qt6_DIR or Qt5_DIR environment variable:
    echo   set Qt6_DIR=C:\Qt\6.x.x\msvc2019_64
    echo.
    echo Or specify it when running cmake:
    echo   cmake .. -DCMAKE_PREFIX_PATH=C:\Qt\6.x.x\msvc2019_64
    echo.
    pause
    exit /b 1
)

REM Build
echo.
echo Building...
echo.
cmake --build . --config Release

if %ERRORLEVEL% neq 0 (
    echo.
    echo ========================================
    echo ERROR: Build failed!
    echo ========================================
    echo.
    pause
    exit /b 1
)

REM Success
echo.
echo ========================================
echo Build completed successfully!
echo ========================================
echo.
echo Executable location: ..\bin\showcase.exe
echo.
echo To run the showcase:
echo   cd ..\bin
echo   showcase.exe
echo.
pause