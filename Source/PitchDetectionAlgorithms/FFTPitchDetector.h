#pragma once

#include "PitchDetector.h"
#include <vector>

class FFTPitchDetector : public PitchDetector
{
public:
    FFTPitchDetector();
    ~FFTPitchDetector() override = default;
    
    void prepare(double sampleRate, int bufferSize) override;
    float detectPitch(const juce::AudioBuffer<float>& buffer) override;
    juce::String getName() const override { return "FFT"; }
    float getConfidence() const override;

private:
    std::vector<float> fftBuffer;
    std::vector<float> magnitudeSpectrum;
    std::vector<float> windowBuffer;
    
    float confidence = 1.0f;
    int fftSize = 2048;
    
    // FFT algorithm parameters - Updated for full bass guitar range
    static constexpr float MIN_FREQUENCY = 30.0f;   // Hz (B0 on 5-string bass)
    static constexpr float MAX_FREQUENCY = 400.0f;  // Hz (bass guitar range)
    static constexpr float MIN_MAGNITUDE_THRESHOLD = 0.01f;
    
    void performFFT(float* buffer, int size);
    void applyWindow(float* buffer, int size);
    int findPeakFrequency() const;
    float parabolicInterpolation(int index) const;
    float frequencyToBin(float frequency) const;
    float binToFrequency(float bin) const;
}; 