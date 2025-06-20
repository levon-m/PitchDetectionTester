cmake_minimum_required(VERSION 3.15)
project(PitchDetectionTester VERSION 1.0.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add JUCE as a subdirectory
add_subdirectory(JUCE)

# Create the VST3 plugin (VST3 only, no other formats)
juce_add_plugin(PitchDetectionTester
    COMPANY_NAME "PitchDetectionTester"
    COMPANY_WEBSITE "https://github.com/yourusername/PitchDetectionTester"
    COMPANY_EMAIL "your.email@example.com"
    PLUGIN_MANUFACTURER_CODE "PdtT"
    PLUGIN_CODE "Pdt1"
    FORMATS VST3
    PRODUCT_NAME "Pitch Detection Tester"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    VST3_CATEGORIES "Fx"
    AU_MAIN_TYPE "kAudioUnitType_Effect"
    COPY_PLUGIN_AFTER_BUILD FALSE
)

# Explicitly disable other plugin formats and ignore the VST2->VST3 warning
target_compile_definitions(PitchDetectionTester
    PRIVATE
        JucePlugin_Build_VST=0
        JucePlugin_Build_VST3=1
        JucePlugin_Build_AU=0
        JucePlugin_Build_AUv3=0
        JucePlugin_Build_RTAS=0
        JucePlugin_Build_AAX=0
        JucePlugin_Build_Standalone=0
        JucePlugin_Build_Unity=0
        JUCE_IGNORE_VST3_MISMATCHED_PARAMETER_ID_WARNING=1
)

# Add source files
target_sources(PitchDetectionTester
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
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

# Windows-specific settings
if(WIN32)
    # Add Windows threading support
    target_link_libraries(PitchDetectionTester PRIVATE ws2_32 winmm)
    
    # Set compiler flags
    target_compile_options(PitchDetectionTester PRIVATE /W4)
else()
    target_compile_options(PitchDetectionTester PRIVATE -Wall -Wextra -Wpedantic)
endif() 