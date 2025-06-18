#pragma once

#include "PitchDetector.h"
#include <vector>

class YinPitchDetector : public PitchDetector
{
public:
    YinPitchDetector();
    ~YinPitchDetector() override = default;
    
    void prepare(double sampleRate, int bufferSize) override;
    float detectPitch(const juce::AudioBuffer<float>& buffer) override;
    juce::String getName() const override { return "YIN"; }
    float getConfidence() const override;

private:
    std::vector<float> yinBuffer;
    std::vector<float> differenceBuffer;
    std::vector<float> cumulativeMeanNormalizedDifference;
    
    float threshold = 0.15f;
    float confidence = 1.0f;
    
    // YIN algorithm parameters - Updated for full bass guitar range
    static constexpr float MIN_FREQUENCY = 30.0f;   // Hz (B0 on 5-string bass)
    static constexpr float MAX_FREQUENCY = 400.0f;  // Hz (bass guitar range)
    
    void computeDifferenceFunction(const float* buffer, int bufferSize);
    void computeCumulativeMeanNormalizedDifference();
    int findMinimumIndex() const;
    float parabolicInterpolation(int index) const;
}; 