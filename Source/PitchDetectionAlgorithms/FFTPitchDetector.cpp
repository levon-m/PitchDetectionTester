#include "FFTPitchDetector.h"
#include <cmath>
#include <algorithm>

// Define M_PI if not already defined (Windows doesn't define it by default)
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

FFTPitchDetector::FFTPitchDetector()
{
}

void FFTPitchDetector::prepare(double newSampleRate, int newBufferSize)
{
    this->sampleRate = newSampleRate;
    this->bufferSize = newBufferSize;
    
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
        windowBuffer[i] = 0.5f - 0.5f * std::cos(2.0f * static_cast<float>(M_PI) * i / (fftSize - 1));
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
    
    // Simple FFT implementation (avoiding JUCE's FFT for threading issues)
    performFFT(fftBuffer.data(), fftSize);
    
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

void FFTPitchDetector::performFFT(float* buffer, int size)
{
    // Simple FFT implementation using Cooley-Tukey algorithm
    // This avoids any threading issues with JUCE's FFT
    
    // Bit-reversal permutation
    int j = 0;
    for (int i = 0; i < size - 1; ++i)
    {
        if (i < j)
        {
            std::swap(buffer[i * 2], buffer[j * 2]);
            std::swap(buffer[i * 2 + 1], buffer[j * 2 + 1]);
        }
        
        int k = size >> 1;
        while (k <= j)
        {
            j -= k;
            k >>= 1;
        }
        j += k;
    }
    
    // FFT computation
    for (int step = 1; step < size; step <<= 1)
    {
        float omega = -static_cast<float>(M_PI) / step;
        
        for (int group = 0; group < size; group += step << 1)
        {
            for (int pair = group; pair < group + step; ++pair)
            {
                int match = pair + step;
                float cos_val = std::cos(omega * (pair - group));
                float sin_val = std::sin(omega * (pair - group));
                
                float real_temp = buffer[match * 2] * cos_val - buffer[match * 2 + 1] * sin_val;
                float imag_temp = buffer[match * 2] * sin_val + buffer[match * 2 + 1] * cos_val;
                
                buffer[match * 2] = buffer[pair * 2] - real_temp;
                buffer[match * 2 + 1] = buffer[pair * 2 + 1] - imag_temp;
                
                buffer[pair * 2] += real_temp;
                buffer[pair * 2 + 1] += imag_temp;
            }
        }
    }
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
    return static_cast<float>(frequency * fftSize / sampleRate);
}

float FFTPitchDetector::binToFrequency(float bin) const
{
    return static_cast<float>(bin * sampleRate / fftSize);
}

float FFTPitchDetector::getConfidence() const
{
    return confidence;
} 