#include "PluginProcessor.h"
#include "PluginEditor.h"

PitchDetectionTesterAudioProcessor::PitchDetectionTesterAudioProcessor()
    : AudioProcessor(BusesProperties()
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true))
{
    // Initialize pitch detectors
    yinDetector = std::make_unique<YinPitchDetector>();
    fftDetector = std::make_unique<FFTPitchDetector>();
    
    // Set default algorithm
    currentPitchDetector = std::make_unique<YinPitchDetector>();
    currentAlgorithmIndex = 0;
}

PitchDetectionTesterAudioProcessor::~PitchDetectionTesterAudioProcessor()
{
}

const juce::String PitchDetectionTesterAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PitchDetectionTesterAudioProcessor::acceptsMidi() const
{
    return false;
}

bool PitchDetectionTesterAudioProcessor::producesMidi() const
{
    return false;
}

bool PitchDetectionTesterAudioProcessor::isMidiEffect() const
{
    return false;
}

double PitchDetectionTesterAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PitchDetectionTesterAudioProcessor::getNumPrograms()
{
    return 1;
}

int PitchDetectionTesterAudioProcessor::getCurrentProgram()
{
    return 0;
}

void PitchDetectionTesterAudioProcessor::setCurrentProgram(int index)
{
    juce::ignoreUnused(index);
}

const juce::String PitchDetectionTesterAudioProcessor::getProgramName(int index)
{
    juce::ignoreUnused(index);
    return {};
}

void PitchDetectionTesterAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
    juce::ignoreUnused(index, newName);
}

void PitchDetectionTesterAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    this->sampleRate = sampleRate;
    this->bufferSize = samplesPerBlock;
    
    // Prepare analysis buffer
    analysisBuffer.setSize(1, ANALYSIS_BUFFER_SIZE);
    analysisBuffer.clear();
    analysisBufferIndex = 0;
    
    // Prepare pitch detectors
    if (yinDetector) yinDetector->prepare(sampleRate, ANALYSIS_BUFFER_SIZE);
    if (fftDetector) fftDetector->prepare(sampleRate, ANALYSIS_BUFFER_SIZE);
    if (currentPitchDetector) currentPitchDetector->prepare(sampleRate, ANALYSIS_BUFFER_SIZE);
    
    // Reset statistics
    statisticsManager.reset();
}

void PitchDetectionTesterAudioProcessor::releaseResources()
{
    analysisBuffer.setSize(0, 0);
}

void PitchDetectionTesterAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);
    
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // Clear unused output channels
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // Get input audio for analysis
    const float* inputChannel = buffer.getReadPointer(0);
    int numSamples = buffer.getNumSamples();
    
    // Fill analysis buffer
    for (int i = 0; i < numSamples; ++i)
    {
        analysisBuffer.setSample(0, analysisBufferIndex, inputChannel[i]);
        analysisBufferIndex++;
        
        // When buffer is full, perform pitch detection
        if (analysisBufferIndex >= ANALYSIS_BUFFER_SIZE)
        {
            // Check if there's enough signal
            float rms = 0.0f;
            for (int j = 0; j < ANALYSIS_BUFFER_SIZE; ++j)
            {
                float sample = analysisBuffer.getSample(0, j);
                rms += sample * sample;
            }
            rms = std::sqrt(rms / ANALYSIS_BUFFER_SIZE);
            
            if (rms > MIN_AMPLITUDE_THRESHOLD && currentPitchDetector)
            {
                float detectedPitch = currentPitchDetector->detectPitch(analysisBuffer);
                
                if (detectedPitch > 0.0f)
                {
                    // Update statistics
                    statisticsManager.addPitchMeasurement(detectedPitch, rms);
                }
            }
            
            // Reset buffer index
            analysisBufferIndex = 0;
        }
    }
}

bool PitchDetectionTesterAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* PitchDetectionTesterAudioProcessor::createEditor()
{
    return new PitchDetectionTesterAudioProcessorEditor(*this);
}

void PitchDetectionTesterAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    juce::ignoreUnused(destData);
}

void PitchDetectionTesterAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    juce::ignoreUnused(data, sizeInBytes);
}

void PitchDetectionTesterAudioProcessor::setPitchDetectionAlgorithm(int algorithmIndex)
{
    if (algorithmIndex == currentAlgorithmIndex)
        return;
        
    currentAlgorithmIndex = algorithmIndex;
    
    switch (algorithmIndex)
    {
        case 0: // YIN
            currentPitchDetector = std::make_unique<YinPitchDetector>();
            break;
        case 1: // FFT
            currentPitchDetector = std::make_unique<FFTPitchDetector>();
            break;
        default:
            currentPitchDetector = std::make_unique<YinPitchDetector>();
            break;
    }
    
    if (currentPitchDetector)
        currentPitchDetector->prepare(sampleRate, ANALYSIS_BUFFER_SIZE);
        
    // Reset statistics when changing algorithm
    statisticsManager.reset();
}

juce::StringArray PitchDetectionTesterAudioProcessor::getAlgorithmNames() const
{
    return {"YIN", "FFT"};
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PitchDetectionTesterAudioProcessor();
} 