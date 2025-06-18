#pragma once

#include <JuceHeader.h>

class PitchDetector
{
public:
    virtual ~PitchDetector() = default;
    
    // Initialize the pitch detector with sample rate and buffer size
    virtual void prepare(double sampleRate, int bufferSize) = 0;
    
    // Detect pitch from audio buffer (returns frequency in Hz, 0.0 if no pitch detected)
    virtual float detectPitch(const juce::AudioBuffer<float>& buffer) = 0;
    
    // Get algorithm name for UI
    virtual juce::String getName() const = 0;
    
    // Optional: Get confidence value (0.0 to 1.0)
    virtual float getConfidence() const { return 1.0f; }
    
protected:
    double sampleRate = 44100.0;
    int bufferSize = 2048;
}; 