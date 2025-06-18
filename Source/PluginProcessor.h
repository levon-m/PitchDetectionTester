#pragma once

#include <JuceHeader.h>
#include "PitchDetectionAlgorithms/PitchDetector.h"
#include "PitchDetectionAlgorithms/YinPitchDetector.h"
#include "PitchDetectionAlgorithms/FFTPitchDetector.h"
#include "Statistics/StatisticsManager.h"

class PitchDetectionTesterAudioProcessor : public juce::AudioProcessor
{
public:
    PitchDetectionTesterAudioProcessor();
    ~PitchDetectionTesterAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    // Pitch detection methods
    void setPitchDetectionAlgorithm(int algorithmIndex);
    int getCurrentAlgorithmIndex() const { return currentAlgorithmIndex; }
    
    // Statistics access
    StatisticsManager& getStatisticsManager() { return statisticsManager; }
    
    // Algorithm names for UI
    juce::StringArray getAlgorithmNames() const;

private:
    // Audio processing
    double sampleRate = 44100.0;
    int bufferSize = 512;
    
    // Pitch detection
    std::unique_ptr<PitchDetector> currentPitchDetector;
    std::unique_ptr<YinPitchDetector> yinDetector;
    std::unique_ptr<FFTPitchDetector> fftDetector;
    int currentAlgorithmIndex = 0;
    
    // Statistics
    StatisticsManager statisticsManager;
    
    // Audio buffer for analysis
    juce::AudioBuffer<float> analysisBuffer;
    int analysisBufferIndex = 0;
    
    // Processing parameters
    static constexpr int ANALYSIS_BUFFER_SIZE = 2048;
    static constexpr float MIN_AMPLITUDE_THRESHOLD = 0.01f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PitchDetectionTesterAudioProcessor)
}; 