#include "FFTPitchDetector.h"
#include <cmath>
#include <algorithm>

FFTPitchDetector::FFTPitchDetector()
{
}

void FFTPitchDetector::prepare(double sampleRate, int bufferSize)
{
    this->sampleRate = sampleRate;
    this->bufferSize = bufferSize;
    
    // Use power of 2 FFT size for efficiency
    fftSize = 1;
    while (fftSize < bufferSize)
        fftSize <<= 1;
    
    // Resize buffers
    fftBuffer.resize(fftSize * 2); // Complex FFT buffer
    magnitudeSpectrum.resize(fftSize / 2);
    windowBuffer.resize(fftSize);
    
    // Create Hann window
    for (int i = 0; i < fftSize; ++i)
    {
        windowBuffer[i] = 0.5f - 0.5f * std::cos(2.0f * M_PI * i / (fftSize - 1));
    }
    
    // Clear buffers
    std::fill(fftBuffer.begin(), fftBuffer.end(), 0.0f);
    std::fill(magnitudeSpectrum.begin(), magnitudeSpectrum.end(), 0.0f);
}

float FFTPitchDetector::detectPitch(const juce::AudioBuffer<float>& buffer)
{
    if (buffer.getNumSamples() != bufferSize)
        return 0.0f;
    
    const float* inputBuffer = buffer.getReadPointer(0);
    
    // Copy input to FFT buffer and apply window
    for (int i = 0; i < fftSize; ++i)
    {
        float sample = (i < bufferSize) ? inputBuffer[i] : 0.0f;
        fftBuffer[i * 2] = sample * windowBuffer[i];     // Real part
        fftBuffer[i * 2 + 1] = 0.0f;                    // Imaginary part
    }
    
    // Perform FFT using JUCE's FFT
    juce::dsp::FFT fft(std::log2(fftSize));
    fft.performFrequencyOnlyForwardTransform(fftBuffer.data());
    
    // Calculate magnitude spectrum
    for (int i = 0; i < fftSize / 2; ++i)
    {
        float real = fftBuffer[i * 2];
        float imag = fftBuffer[i * 2 + 1];
        magnitudeSpectrum[i] = std::sqrt(real * real + imag * imag);
    }
    
    // Find peak frequency in bass guitar range
    int peakBin = findPeakFrequency();
    
    if (peakBin == -1)
    {
        confidence = 0.0f;
        return 0.0f;
    }
    
    // Parabolic interpolation for better precision
    float interpolatedBin = parabolicInterpolation(peakBin);
    
    // Convert to frequency
    float frequency = binToFrequency(interpolatedBin);
    
    // Check if frequency is in valid range
    if (frequency < MIN_FREQUENCY || frequency > MAX_FREQUENCY)
    {
        confidence = 0.0f;
        return 0.0f;
    }
    
    // Calculate confidence based on peak magnitude
    float peakMagnitude = magnitudeSpectrum[peakBin];
    float maxMagnitude = *std::max_element(magnitudeSpectrum.begin(), magnitudeSpectrum.end());
    confidence = (maxMagnitude > 0.0f) ? peakMagnitude / maxMagnitude : 0.0f;
    
    return frequency;
}

void FFTPitchDetector::applyWindow(float* buffer, int size)
{
    for (int i = 0; i < size; ++i)
    {
        buffer[i] *= windowBuffer[i];
    }
}

int FFTPitchDetector::findPeakFrequency() const
{
    int minBin = static_cast<int>(frequencyToBin(MAX_FREQUENCY));
    int maxBin = static_cast<int>(frequencyToBin(MIN_FREQUENCY));
    
    // Ensure bounds
    minBin = std::max(1, minBin);
    maxBin = std::min(static_cast<int>(magnitudeSpectrum.size()) - 2, maxBin);
    
    if (minBin >= maxBin)
        return -1;
    
    int peakBin = -1;
    float peakMagnitude = MIN_MAGNITUDE_THRESHOLD;
    
    // Find local maximum in frequency range
    for (int i = minBin; i <= maxBin; ++i)
    {
        if (magnitudeSpectrum[i] > peakMagnitude &&
            magnitudeSpectrum[i] > magnitudeSpectrum[i - 1] &&
            magnitudeSpectrum[i] > magnitudeSpectrum[i + 1])
        {
            peakBin = i;
            peakMagnitude = magnitudeSpectrum[i];
        }
    }
    
    return peakBin;
}

float FFTPitchDetector::parabolicInterpolation(int index) const
{
    if (index <= 0 || index >= static_cast<int>(magnitudeSpectrum.size()) - 1)
        return static_cast<float>(index);
    
    float alpha = magnitudeSpectrum[index - 1];
    float beta = magnitudeSpectrum[index];
    float gamma = magnitudeSpectrum[index + 1];
    
    if (alpha - 2.0f * beta + gamma == 0.0f)
        return static_cast<float>(index);
    
    float peak = 0.5f * (alpha - gamma) / (alpha - 2.0f * beta + gamma);
    
    return static_cast<float>(index) + peak;
}

float FFTPitchDetector::frequencyToBin(float frequency) const
{
    return frequency * fftSize / sampleRate;
}

float FFTPitchDetector::binToFrequency(int bin) const
{
    return static_cast<float>(bin) * sampleRate / fftSize;
}

float FFTPitchDetector::getConfidence() const
{
    return confidence;
} 