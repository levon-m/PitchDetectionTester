#pragma once

#include <JuceHeader.h>
#include <vector>
#include <deque>

struct PitchMeasurement
{
    float frequency;
    float amplitude;
    juce::uint32 timestamp;
    
    PitchMeasurement(float freq, float amp, juce::uint32 time)
        : frequency(freq), amplitude(amp), timestamp(time) {}
};

class StatisticsManager
{
public:
    StatisticsManager();
    ~StatisticsManager() = default;
    
    // Add a new pitch measurement
    void addPitchMeasurement(float frequency, float amplitude);
    
    // Reset all statistics
    void reset();
    
    // Get current statistics
    float getCurrentPitch() const { return currentPitch; }
    float getAveragePitch() const { return averagePitch; }
    float getPitchStability() const { return pitchStability; }
    float getDetectionConfidence() const { return detectionConfidence; }
    float getResponseTime() const { return responseTime; }
    int getTotalDetections() const { return totalDetections; }
    int getValidDetections() const { return validDetections; }
    
    // Get recent measurements for visualization
    const std::deque<PitchMeasurement>& getRecentMeasurements() const { return recentMeasurements; }
    
    // Get pitch history for plotting
    std::vector<float> getPitchHistory() const;
    
    // Note detection
    juce::String getCurrentNote() const;
    juce::String getAverageNote() const;
    
private:
    // Current measurements
    float currentPitch = 0.0f;
    float currentAmplitude = 0.0f;
    juce::uint32 lastTimestamp = 0;
    
    // Statistics
    float averagePitch = 0.0f;
    float pitchStability = 0.0f;
    float detectionConfidence = 0.0f;
    float responseTime = 0.0f;
    
    // Counters
    int totalDetections = 0;
    int validDetections = 0;
    
    // History for calculations
    std::deque<PitchMeasurement> recentMeasurements;
    std::vector<float> pitchHistory;
    
    // Configuration - Updated for full bass guitar range
    static constexpr int MAX_HISTORY_SIZE = 1000;
    static constexpr int STABILITY_WINDOW = 50;
    static constexpr float MIN_VALID_FREQUENCY = 30.0f;  // Hz (B0 on 5-string bass)
    static constexpr float MAX_VALID_FREQUENCY = 400.0f; // Hz (bass guitar range)
    
    // Helper methods
    void updateStatistics();
    float calculatePitchStability() const;
    float calculateDetectionConfidence() const;
    float calculateResponseTime() const;
    juce::String frequencyToNote(float frequency) const;
    bool isValidFrequency(float frequency) const;
}; 