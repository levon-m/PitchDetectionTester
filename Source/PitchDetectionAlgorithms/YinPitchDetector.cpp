#include "YinPitchDetector.h"
#include <cmath>
#include <algorithm>

YinPitchDetector::YinPitchDetector()
{
}

void YinPitchDetector::prepare(double sampleRate, int bufferSize)
{
    this->sampleRate = sampleRate;
    this->bufferSize = bufferSize;
    
    // Resize buffers
    yinBuffer.resize(bufferSize / 2);
    differenceBuffer.resize(bufferSize / 2);
    cumulativeMeanNormalizedDifference.resize(bufferSize / 2);
    
    // Clear buffers
    std::fill(yinBuffer.begin(), yinBuffer.end(), 0.0f);
    std::fill(differenceBuffer.begin(), differenceBuffer.end(), 0.0f);
    std::fill(cumulativeMeanNormalizedDifference.begin(), cumulativeMeanNormalizedDifference.end(), 0.0f);
}

float YinPitchDetector::detectPitch(const juce::AudioBuffer<float>& buffer)
{
    if (buffer.getNumSamples() != bufferSize)
        return 0.0f;
    
    const float* inputBuffer = buffer.getReadPointer(0);
    
    // Step 1: Compute difference function
    computeDifferenceFunction(inputBuffer, bufferSize);
    
    // Step 2: Compute cumulative mean normalized difference
    computeCumulativeMeanNormalizedDifference();
    
    // Step 3: Find minimum index
    int minIndex = findMinimumIndex();
    
    if (minIndex == -1)
    {
        confidence = 0.0f;
        return 0.0f;
    }
    
    // Step 4: Parabolic interpolation for better precision
    float interpolatedIndex = parabolicInterpolation(minIndex);
    
    // Step 5: Convert to frequency
    float frequency = sampleRate / interpolatedIndex;
    
    // Step 6: Check if frequency is in valid range for bass guitar
    if (frequency < MIN_FREQUENCY || frequency > MAX_FREQUENCY)
    {
        confidence = 0.0f;
        return 0.0f;
    }
    
    // Step 7: Calculate confidence based on threshold
    float minValue = cumulativeMeanNormalizedDifference[minIndex];
    confidence = std::max(0.0f, 1.0f - minValue / threshold);
    
    return frequency;
}

void YinPitchDetector::computeDifferenceFunction(const float* buffer, int bufferSize)
{
    int halfBufferSize = bufferSize / 2;
    
    for (int t = 0; t < halfBufferSize; ++t)
    {
        differenceBuffer[t] = 0.0f;
        
        for (int i = 0; i < halfBufferSize; ++i)
        {
            float diff = buffer[i] - buffer[i + t];
            differenceBuffer[t] += diff * diff;
        }
    }
}

void YinPitchDetector::computeCumulativeMeanNormalizedDifference()
{
    int halfBufferSize = cumulativeMeanNormalizedDifference.size();
    
    // First value
    cumulativeMeanNormalizedDifference[0] = 1.0f;
    
    // Compute running sum
    float runningSum = differenceBuffer[0];
    
    for (int t = 1; t < halfBufferSize; ++t)
    {
        runningSum += differenceBuffer[t];
        cumulativeMeanNormalizedDifference[t] = differenceBuffer[t] / (runningSum / (t + 1));
    }
}

int YinPitchDetector::findMinimumIndex() const
{
    int halfBufferSize = cumulativeMeanNormalizedDifference.size();
    int minIndex = -1;
    float minValue = threshold;
    
    // Find first value below threshold
    for (int i = 2; i < halfBufferSize; ++i) // Start from 2 to avoid DC
    {
        if (cumulativeMeanNormalizedDifference[i] < threshold)
        {
            // Find the minimum in this region
            minIndex = i;
            minValue = cumulativeMeanNormalizedDifference[i];
            
            // Look for local minimum
            while (i + 1 < halfBufferSize && cumulativeMeanNormalizedDifference[i + 1] < minValue)
            {
                i++;
                minIndex = i;
                minValue = cumulativeMeanNormalizedDifference[i];
            }
            break;
        }
    }
    
    return minIndex;
}

float YinPitchDetector::parabolicInterpolation(int index) const
{
    if (index <= 0 || index >= static_cast<int>(cumulativeMeanNormalizedDifference.size()) - 1)
        return static_cast<float>(index);
    
    float alpha = cumulativeMeanNormalizedDifference[index - 1];
    float beta = cumulativeMeanNormalizedDifference[index];
    float gamma = cumulativeMeanNormalizedDifference[index + 1];
    
    float peak = 0.5f * (alpha - gamma) / (alpha - 2.0f * beta + gamma);
    
    return static_cast<float>(index) + peak;
}

float YinPitchDetector::getConfidence() const
{
    return confidence;
} 