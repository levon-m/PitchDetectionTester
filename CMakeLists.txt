cmake_minimum_required(VERSION 3.15)
project(PitchDetectionTester VERSION 1.0.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add JUCE as a subdirectory
add_subdirectory(JUCE)

# Create the VST3 plugin
juce_add_plugin(PitchDetectionTester
    COMPANY_NAME "PitchDetectionTester"
    COMPANY_WEBSITE "https://github.com/yourusername/PitchDetectionTester"
    COMPANY_EMAIL "your.email@example.com"
    PLUGIN_MANUFACTURER_CODE "Pdt"
    PLUGIN_CODE "Pdt1"
    FORMATS VST3
    PRODUCT_NAME "Pitch Detection Tester"
    COMPANY_WEBSITE "https://github.com/yourusername/PitchDetectionTester"
    COMPANY_EMAIL "your.email@example.com"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    VST3_CATEGORIES "Fx"
    AU_MAIN_TYPE "kAudioUnitType_Effect"
    COPY_PLUGIN_AFTER_BUILD TRUE
)

# Add source files
target_sources(PitchDetectionTester
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
        Source/PitchDetectionAlgorithms/PitchDetector.cpp
        Source/PitchDetectionAlgorithms/YinPitchDetector.cpp
        Source/PitchDetectionAlgorithms/FFTPitchDetector.cpp
        Source/Statistics/StatisticsManager.cpp
        Source/UI/StatisticsDisplay.cpp
)

# Link JUCE modules
target_link_libraries(PitchDetectionTester
    PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_dsp
        juce::juce_gui_extra
        juce::juce_gui_basics
        juce::juce_core
        juce::juce_data_structures
        juce::juce_events
        juce::juce_graphics
        juce::juce_opengl
)

# Set include directories
target_include_directories(PitchDetectionTester
    PRIVATE
        Source
        Source/PitchDetectionAlgorithms
        Source/Statistics
        Source/UI
)

# Set compiler flags
if(MSVC)
    target_compile_options(PitchDetectionTester PRIVATE /W4)
else()
    target_compile_options(PitchDetectionTester PRIVATE -Wall -Wextra -Wpedantic)
endif() 