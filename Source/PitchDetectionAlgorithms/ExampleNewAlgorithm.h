#pragma once

#include "PitchDetector.h"
#include <vector>

// Example of how to add a new pitch detection algorithm
// This demonstrates the modular architecture of the plugin
class ExampleNewAlgorithm : public PitchDetector
{
public:
    ExampleNewAlgorithm();
    ~ExampleNewAlgorithm() override = default;
    
    void prepare(double sampleRate, int bufferSize) override;
    float detectPitch(const juce::AudioBuffer<float>& buffer) override;
    juce::String getName() const override { return "Example Algorithm"; }
    float getConfidence() const override;

private:
    std::vector<float> buffer;
    float confidence = 1.0f;
    
    // Algorithm-specific parameters - Updated for full bass guitar range
    static constexpr float MIN_FREQUENCY = 30.0f;   // Hz (B0 on 5-string bass)
    static constexpr float MAX_FREQUENCY = 400.0f;  // Hz (bass guitar range)
    
    // Example algorithm methods
    float calculatePitch(const float* samples, int numSamples);
    bool isValidPitch(float frequency) const;
};

/*
To integrate this new algorithm:

1. Add the header include to PluginProcessor.h:
   #include "PitchDetectionAlgorithms/ExampleNewAlgorithm.h"

2. Add member variable to PluginProcessor class:
   std::unique_ptr<ExampleNewAlgorithm> exampleDetector;

3. Initialize in constructor:
   exampleDetector = std::make_unique<ExampleNewAlgorithm>();

4. Add to setPitchDetectionAlgorithm() method:
   case 2: // Example Algorithm
       currentPitchDetector = std::make_unique<ExampleNewAlgorithm>();
       break;

5. Update getAlgorithmNames():
   return {"YIN", "FFT", "Example Algorithm"};

6. Prepare in prepareToPlay():
   if (exampleDetector) exampleDetector->prepare(sampleRate, ANALYSIS_BUFFER_SIZE);

This modular design makes it easy to add new algorithms without modifying existing code!
*/ 