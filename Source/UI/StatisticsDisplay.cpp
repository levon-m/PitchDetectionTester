#include "StatisticsDisplay.h"

StatisticsDisplay::StatisticsDisplay(StatisticsManager& statsManager)
    : statisticsManager(statsManager)
{
    setupLabels();
    startTimerHz(30); // Update 30 times per second
}

void StatisticsDisplay::paint(juce::Graphics& g)
{
    // Fill background
    g.fillAll(backgroundColor);
    
    // Draw title
    g.setColour(textColor);
    g.setFont(juce::Font(20.0f, juce::Font::bold));
    g.drawText("Pitch Detection Statistics", getLocalBounds().removeFromTop(40),
               juce::Justification::centred, true);
    
    // Draw separator line
    g.setColour(accentColor);
    g.drawHorizontalLine(45, 0.0f, static_cast<float>(getWidth()));
}

void StatisticsDisplay::resized()
{
    auto bounds = getLocalBounds().reduced(20);
    bounds.removeFromTop(50); // Space for title
    
    int labelHeight = 25;
    int spacing = 10;
    
    // Current pitch section
    auto currentSection = bounds.removeFromTop(labelHeight * 2 + spacing);
    currentPitchLabel.setBounds(currentSection.removeFromLeft(getWidth() / 2 - 10));
    currentNoteLabel.setBounds(currentSection.removeFromRight(getWidth() / 2 - 10));
    
    bounds.removeFromTop(spacing);
    
    // Statistics section
    auto statsSection = bounds.removeFromTop(labelHeight * 4 + spacing * 3);
    
    averagePitchLabel.setBounds(statsSection.removeFromTop(labelHeight));
    statsSection.removeFromTop(spacing);
    
    stabilityLabel.setBounds(statsSection.removeFromTop(labelHeight));
    statsSection.removeFromTop(spacing);
    
    confidenceLabel.setBounds(statsSection.removeFromTop(labelHeight));
    statsSection.removeFromTop(spacing);
    
    responseTimeLabel.setBounds(statsSection.removeFromTop(labelHeight));
    
    bounds.removeFromTop(spacing);
    
    // Detection count
    detectionCountLabel.setBounds(bounds.removeFromTop(labelHeight));
}

void StatisticsDisplay::timerCallback()
{
    updateLabels();
}

void StatisticsDisplay::setupLabels()
{
    juce::Font labelFont(16.0f, juce::Font::bold);
    juce::Font valueFont(14.0f);
    
    // Current pitch label
    currentPitchLabel.setFont(labelFont);
    currentPitchLabel.setColour(juce::Label::textColourId, textColor);
    currentPitchLabel.setText("Current Pitch:", juce::dontSendNotification);
    currentPitchLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(currentPitchLabel);
    
    // Current note label
    currentNoteLabel.setFont(labelFont);
    currentNoteLabel.setColour(juce::Label::textColourId, accentColor);
    currentNoteLabel.setText("---", juce::dontSendNotification);
    currentNoteLabel.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(currentNoteLabel);
    
    // Average pitch label
    averagePitchLabel.setFont(valueFont);
    averagePitchLabel.setColour(juce::Label::textColourId, textColor);
    averagePitchLabel.setText("Average Pitch: ---", juce::dontSendNotification);
    averagePitchLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(averagePitchLabel);
    
    // Stability label
    stabilityLabel.setFont(valueFont);
    stabilityLabel.setColour(juce::Label::textColourId, textColor);
    stabilityLabel.setText("Stability: ---", juce::dontSendNotification);
    stabilityLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(stabilityLabel);
    
    // Confidence label
    confidenceLabel.setFont(valueFont);
    confidenceLabel.setColour(juce::Label::textColourId, textColor);
    confidenceLabel.setText("Confidence: ---", juce::dontSendNotification);
    confidenceLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(confidenceLabel);
    
    // Response time label
    responseTimeLabel.setFont(valueFont);
    responseTimeLabel.setColour(juce::Label::textColourId, textColor);
    responseTimeLabel.setText("Response Time: ---", juce::dontSendNotification);
    responseTimeLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(responseTimeLabel);
    
    // Detection count label
    detectionCountLabel.setFont(valueFont);
    detectionCountLabel.setColour(juce::Label::textColourId, textColor);
    detectionCountLabel.setText("Detections: 0/0", juce::dontSendNotification);
    detectionCountLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(detectionCountLabel);
}

void StatisticsDisplay::updateLabels()
{
    // Update current pitch
    float currentPitch = statisticsManager.getCurrentPitch();
    currentPitchLabel.setText("Current Pitch: " + formatFrequency(currentPitch), juce::dontSendNotification);
    
    // Update current note
    juce::String currentNote = statisticsManager.getCurrentNote();
    currentNoteLabel.setText(currentNote, juce::dontSendNotification);
    
    // Update average pitch
    float avgPitch = statisticsManager.getAveragePitch();
    averagePitchLabel.setText("Average Pitch: " + formatFrequency(avgPitch) + 
                             " (" + statisticsManager.getAverageNote() + ")", 
                             juce::dontSendNotification);
    
    // Update stability
    float stability = statisticsManager.getPitchStability();
    stabilityLabel.setText("Stability: " + formatPercentage(stability), juce::dontSendNotification);
    stabilityLabel.setColour(juce::Label::textColourId, getStabilityColor(stability));
    
    // Update confidence
    float confidence = statisticsManager.getDetectionConfidence();
    confidenceLabel.setText("Confidence: " + formatPercentage(confidence), juce::dontSendNotification);
    confidenceLabel.setColour(juce::Label::textColourId, getConfidenceColor(confidence));
    
    // Update response time
    float responseTime = statisticsManager.getResponseTime();
    responseTimeLabel.setText("Response Time: " + formatTime(responseTime), juce::dontSendNotification);
    
    // Update detection count
    int total = statisticsManager.getTotalDetections();
    int valid = statisticsManager.getValidDetections();
    detectionCountLabel.setText("Detections: " + juce::String(valid) + "/" + juce::String(total), 
                               juce::dontSendNotification);
}

juce::String StatisticsDisplay::formatFrequency(float frequency) const
{
    if (frequency <= 0.0f)
        return "--- Hz";
    
    if (frequency < 1000.0f)
        return juce::String(frequency, 1) + " Hz";
    else
        return juce::String(frequency / 1000.0f, 2) + " kHz";
}

juce::String StatisticsDisplay::formatPercentage(float value) const
{
    return juce::String(static_cast<int>(value * 100.0f)) + "%";
}

juce::String StatisticsDisplay::formatTime(float seconds) const
{
    if (seconds <= 0.0f)
        return "---";
    
    if (seconds < 0.001f)
        return juce::String(static_cast<int>(seconds * 1000000.0f)) + " μs";
    else if (seconds < 1.0f)
        return juce::String(seconds * 1000.0f, 1) + " ms";
    else
        return juce::String(seconds, 2) + " s";
}

juce::Colour StatisticsDisplay::getStabilityColor(float stability) const
{
    if (stability >= 0.8f)
        return successColor;
    else if (stability >= 0.5f)
        return warningColor;
    else
        return errorColor;
}

juce::Colour StatisticsDisplay::getConfidenceColor(float confidence) const
{
    if (confidence >= 0.8f)
        return successColor;
    else if (confidence >= 0.5f)
        return warningColor;
    else
        return errorColor;
} 