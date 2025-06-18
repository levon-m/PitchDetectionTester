#include "PluginProcessor.h"
#include "PluginEditor.h"

PitchDetectionTesterAudioProcessorEditor::PitchDetectionTesterAudioProcessorEditor(PitchDetectionTesterAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    setupUI();
    
    // Set window size
    setSize(600, 500);
}

PitchDetectionTesterAudioProcessorEditor::~PitchDetectionTesterAudioProcessorEditor()
{
}

void PitchDetectionTesterAudioProcessorEditor::paint(juce::Graphics& g)
{
    // Fill background
    g.fillAll(backgroundColor);
    
    // Draw title
    g.setColour(textColor);
    g.setFont(juce::Font(24.0f, juce::Font::bold));
    g.drawText("Pitch Detection Tester", getLocalBounds().removeFromTop(60),
               juce::Justification::centred, true);
    
    // Draw subtitle
    g.setColour(textColor.withAlpha(0.7f));
    g.setFont(juce::Font(14.0f));
    g.drawText("Bass Guitar Pitch Detection Algorithm Testing", 
               getLocalBounds().removeFromTop(80).removeFromTop(20),
               juce::Justification::centred, true);
}

void PitchDetectionTesterAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds().reduced(20);
    bounds.removeFromTop(100); // Space for title
    
    // Top control panel
    auto controlPanel = bounds.removeFromTop(80);
    
    // Algorithm selector
    algorithmLabel.setBounds(controlPanel.removeFromLeft(120));
    algorithmSelector.setBounds(controlPanel.removeFromLeft(150));
    
    controlPanel.removeFromLeft(20); // Spacing
    
    // Buttons
    resetButton.setBounds(controlPanel.removeFromLeft(100));
    controlPanel.removeFromLeft(20); // Spacing
    helpButton.setBounds(controlPanel.removeFromLeft(100));
    
    bounds.removeFromTop(20); // Spacing
    
    // Statistics display
    if (statisticsDisplay)
        statisticsDisplay->setBounds(bounds);
}

void PitchDetectionTesterAudioProcessorEditor::setupUI()
{
    // Algorithm label
    algorithmLabel.setText("Algorithm:", juce::dontSendNotification);
    algorithmLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    algorithmLabel.setColour(juce::Label::textColourId, textColor);
    algorithmLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(algorithmLabel);
    
    // Algorithm selector
    updateAlgorithmSelector();
    algorithmSelector.addListener(this);
    addAndMakeVisible(algorithmSelector);
    
    // Reset button
    resetButton.setButtonText("Reset Stats");
    resetButton.onClick = [this] { resetStatistics(); };
    resetButton.setColour(juce::TextButton::buttonColourId, accentColor);
    resetButton.setColour(juce::TextButton::textColourOffId, textColor);
    addAndMakeVisible(resetButton);
    
    // Help button
    helpButton.setButtonText("Help");
    helpButton.onClick = [this] { showHelp(); };
    helpButton.setColour(juce::TextButton::buttonColourId, panelColor);
    helpButton.setColour(juce::TextButton::textColourOffId, textColor);
    addAndMakeVisible(helpButton);
    
    // Statistics display
    statisticsDisplay = std::make_unique<StatisticsDisplay>(audioProcessor.getStatisticsManager());
    addAndMakeVisible(statisticsDisplay.get());
}

void PitchDetectionTesterAudioProcessorEditor::updateAlgorithmSelector()
{
    algorithmSelector.clear();
    
    juce::StringArray algorithms = audioProcessor.getAlgorithmNames();
    for (int i = 0; i < algorithms.size(); ++i)
    {
        algorithmSelector.addItem(algorithms[i], i + 1);
    }
    
    // Set current selection
    algorithmSelector.setSelectedId(audioProcessor.getCurrentAlgorithmIndex() + 1, juce::dontSendNotification);
}

void PitchDetectionTesterAudioProcessorEditor::algorithmChanged()
{
    int selectedIndex = algorithmSelector.getSelectedId() - 1;
    if (selectedIndex >= 0)
    {
        audioProcessor.setPitchDetectionAlgorithm(selectedIndex);
    }
}

void PitchDetectionTesterAudioProcessorEditor::resetStatistics()
{
    audioProcessor.getStatisticsManager().reset();
}

void PitchDetectionTesterAudioProcessorEditor::showHelp()
{
    juce::AlertWindow::showMessageBoxAsync(
        juce::MessageBoxIconType::InfoIcon,
        "Pitch Detection Tester Help",
        "This plugin tests pitch detection algorithms for bass guitar.\n\n"
        "1. Select an algorithm from the dropdown menu\n"
        "2. Play your bass guitar through the plugin\n"
        "3. View real-time statistics and performance metrics\n"
        "4. Compare different algorithms' performance\n\n"
        "Available Algorithms:\n"
        "• YIN: Robust pitch detection using autocorrelation\n"
        "• FFT: Fast Fourier Transform based detection\n\n"
        "Statistics:\n"
        "• Current Pitch: Real-time detected frequency\n"
        "• Stability: How consistent the detection is\n"
        "• Confidence: Algorithm's confidence in the detection\n"
        "• Response Time: How quickly the algorithm responds\n"
        "• Detection Count: Total vs valid detections",
        "OK"
    );
} 