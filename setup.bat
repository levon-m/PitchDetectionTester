@echo off
echo Setting up Pitch Detection Tester build environment...

REM Check if JUCE directory exists
if not exist "JUCE" (
    echo Downloading JUCE framework...
    git clone https://github.com/juce-framework/JUCE.git
    if errorlevel 1 (
        echo Failed to download JUCE. Please check your internet connection.
        pause
        exit /b 1
    )
) else (
    echo JUCE directory already exists.
)

REM Check if CMake is available
cmake --version >nul 2>&1
if errorlevel 1 (
    echo CMake not found. Please install CMake and add it to your PATH.
    echo Download from: https://cmake.org/download/
    pause
    exit /b 1
)

echo Setup complete!
echo You can now run build.bat to build the plugin.
pause 