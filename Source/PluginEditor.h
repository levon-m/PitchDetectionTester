#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"
#include "UI/StatisticsDisplay.h"

class PitchDetectionTesterAudioProcessorEditor : public juce::AudioProcessorEditor, public juce::ComboBox::Listener
{
public:
    PitchDetectionTesterAudioProcessorEditor(PitchDetectionTesterAudioProcessor&);
    ~PitchDetectionTesterAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    
    // ComboBox::Listener implementation
    void comboBoxChanged(juce::ComboBox*) override;

private:
    PitchDetectionTesterAudioProcessor& audioProcessor;
    
    // UI Components
    juce::ComboBox algorithmSelector;
    juce::Label algorithmLabel;
    juce::TextButton resetButton;
    juce::TextButton helpButton;
    
    // Statistics display
    std::unique_ptr<StatisticsDisplay> statisticsDisplay;
    
    // Colors
    juce::Colour backgroundColor = juce::Colour(0xFF1E1E1E);
    juce::Colour panelColor = juce::Colour(0xFF2D2D30);
    juce::Colour textColor = juce::Colour(0xFFE1E1E1);
    juce::Colour accentColor = juce::Colour(0xFF007ACC);
    
    // Callbacks
    void algorithmChanged();
    void resetStatistics();
    void showHelp();
    
    // Helper methods
    void setupUI();
    void updateAlgorithmSelector();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PitchDetectionTesterAudioProcessorEditor)
}; 