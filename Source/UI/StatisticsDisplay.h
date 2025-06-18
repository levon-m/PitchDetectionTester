#pragma once

#include <JuceHeader.h>
#include "../Statistics/StatisticsManager.h"

class StatisticsDisplay : public juce::Component, public juce::Timer
{
public:
    StatisticsDisplay(StatisticsManager& statsManager);
    ~StatisticsDisplay() override = default;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    void timerCallback() override;
    
    // Set display mode
    enum DisplayMode
    {
        RealTime,
        History,
        Comparison
    };
    
    void setDisplayMode(DisplayMode mode) { displayMode = mode; repaint(); }
    
private:
    StatisticsManager& statisticsManager;
    DisplayMode displayMode = RealTime;
    
    // Display components
    juce::Label currentPitchLabel;
    juce::Label currentNoteLabel;
    juce::Label averagePitchLabel;
    juce::Label stabilityLabel;
    juce::Label confidenceLabel;
    juce::Label responseTimeLabel;
    juce::Label detectionCountLabel;
    
    // Colors
    juce::Colour backgroundColor = juce::Colour(0xFF2D2D30);
    juce::Colour textColor = juce::Colour(0xFFE1E1E1);
    juce::Colour accentColor = juce::Colour(0xFF007ACC);
    juce::Colour successColor = juce::Colour(0xFF4CAF50);
    juce::Colour warningColor = juce::Colour(0xFFFF9800);
    juce::Colour errorColor = juce::Colour(0xFFF44336);
    
    // Helper methods
    void setupLabels();
    void updateLabels();
    juce::String formatFrequency(float frequency) const;
    juce::String formatPercentage(float value) const;
    juce::String formatTime(float seconds) const;
    juce::Colour getStabilityColor(float stability) const;
    juce::Colour getConfidenceColor(float confidence) const;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StatisticsDisplay)
}; 