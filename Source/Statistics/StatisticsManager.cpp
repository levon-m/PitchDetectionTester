#include "StatisticsManager.h"
#include <cmath>
#include <algorithm>
#include <numeric>

StatisticsManager::StatisticsManager()
{
    reset();
}

void StatisticsManager::addPitchMeasurement(float frequency, float amplitude)
{
    totalDetections++;
    currentPitch = frequency;
    currentAmplitude = amplitude;

    juce::int64 currentTime = juce::Time::getHighResolutionTicks();
    
    if (isValidFrequency(frequency))
    {
        validDetections++;
        
        // Add to recent measurements for stability/confidence/response time calculations
        recentMeasurements.emplace_back(frequency, amplitude, currentTime);
        if (recentMeasurements.size() > STABILITY_WINDOW)
        {
            recentMeasurements.pop_front();
        }
    }
    else
    {
        // Add a "zero" frequency to indicate no valid pitch
        recentMeasurements.emplace_back(0.0f, amplitude, currentTime);
        if (recentMeasurements.size() > STABILITY_WINDOW)
        {
            recentMeasurements.pop_front();
        }
    }

    // Add to pitch history for long-term average/plotting
    pitchHistory.push_back(frequency);
    if (pitchHistory.size() > MAX_HISTORY_SIZE)
    {
        pitchHistory.erase(pitchHistory.begin());
    }
    
    updateStatistics();
    lastTimestamp = currentTime;
}

void StatisticsManager::reset()
{
    currentPitch = 0.0f;
    currentAmplitude = 0.0f;
    averagePitch = 0.0f;
    pitchStability = 0.0f;
    detectionConfidence = 0.0f;
    responseTime = 0.0f;
    totalDetections = 0;
    validDetections = 0;
    lastTimestamp = 0;
    
    recentMeasurements.clear();
    pitchHistory.clear();
}

std::vector<float> StatisticsManager::getPitchHistory() const
{
    return pitchHistory;
}

juce::String StatisticsManager::getCurrentNote() const
{
    return frequencyToNote(currentPitch);
}

juce::String StatisticsManager::getAverageNote() const
{
    return frequencyToNote(averagePitch);
}

void StatisticsManager::updateStatistics()
{
    if (pitchHistory.empty())
        return;
    
    // Calculate average pitch (only valid frequencies)
    float sum = 0.0f;
    int count = 0;
    
    for (float pitch : pitchHistory)
    {
        if (isValidFrequency(pitch))
        {
            sum += pitch;
            count++;
        }
    }
    
    averagePitch = (count > 0) ? sum / count : 0.0f;
    
    // Calculate other statistics
    pitchStability = calculatePitchStability();
    detectionConfidence = calculateDetectionConfidence();
    responseTime = calculateResponseTime();
}

float StatisticsManager::calculatePitchStability() const
{
    if (pitchHistory.size() < 2)
        return 0.0f;
    
    // Calculate standard deviation of recent pitches
    float sum = 0.0f;
    float sumSquared = 0.0f;
    int count = 0;
    
    int startIndex = std::max(0, static_cast<int>(pitchHistory.size()) - STABILITY_WINDOW);
    
    for (int i = startIndex; i < static_cast<int>(pitchHistory.size()); ++i)
    {
        float pitch = pitchHistory[i];
        if (isValidFrequency(pitch))
        {
            sum += pitch;
            sumSquared += pitch * pitch;
            count++;
        }
    }
    
    if (count < 2)
        return 0.0f;
    
    float mean = sum / count;
    float variance = (sumSquared / count) - (mean * mean);
    float stdDev = std::sqrt(std::max(0.0f, variance));
    
    // Convert to stability score (0-1, higher is more stable)
    float stability = std::max(0.0f, 1.0f - (stdDev / 50.0f)); // 50 Hz as reference
    return stability;
}

float StatisticsManager::calculateDetectionConfidence() const
{
    if (totalDetections == 0)
        return 0.0f;
    
    // Calculate confidence based on valid detection ratio and amplitude
    float validRatio = static_cast<float>(validDetections) / totalDetections;
    float amplitudeFactor = std::min(1.0f, currentAmplitude / 0.1f); // Normalize amplitude
    
    return validRatio * amplitudeFactor;
}

float StatisticsManager::calculateResponseTime() const
{
    if (recentMeasurements.size() < 2)
    {
        return 0.0f;
    }
    
    juce::int64 timeDiffSum = 0;
    int count = 0;

    for (size_t i = 1; i < recentMeasurements.size(); ++i)
    {
        // Only measure time between consecutive valid detections
        if (isValidFrequency(recentMeasurements[i].frequency) && isValidFrequency(recentMeasurements[i - 1].frequency))
        {
            juce::int64 timeDiff = recentMeasurements[i].timestamp - recentMeasurements[i - 1].timestamp;
            timeDiffSum += timeDiff;
            count++;
        }
    }
    
    if (count == 0)
    {
        return 0.0f;
    }
    
    double averageTimeDiffTicks = static_cast<double>(timeDiffSum) / count;
    return static_cast<float>(averageTimeDiffTicks * 1000.0 / juce::Time::getHighResolutionTicksPerSecond());
}

juce::String StatisticsManager::frequencyToNote(float frequency) const
{
    if (!isValidFrequency(frequency))
        return "---";
    
    // A4 = 440 Hz
    const float A4 = 440.0f;
    const float A4_INDEX = 69.0f; // MIDI note number for A4
    
    // Calculate MIDI note number
    float midiNote = A4_INDEX + 12.0f * std::log2(frequency / A4);
    int noteNumber = static_cast<int>(std::round(midiNote));
    
    // Convert to note name
    const char* noteNames[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
    int octave = (noteNumber / 12) - 1;
    int noteIndex = noteNumber % 12;
    
    if (noteIndex < 0) noteIndex += 12;
    
    return juce::String(noteNames[noteIndex]) + juce::String(octave);
}

bool StatisticsManager::isValidFrequency(float frequency) const
{
    return frequency >= MIN_VALID_FREQUENCY && frequency <= MAX_VALID_FREQUENCY;
} 