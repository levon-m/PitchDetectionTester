# Pitch Detection Tester

A VST3 plugin for testing and comparing pitch detection algorithms specifically optimized for bass guitar. Built with C++ and JUCE framework.

## Features

- **Modular Algorithm Architecture**: Easy to add new pitch detection algorithms
- **Real-time Statistics**: Live performance metrics and measurements
- **Bass Guitar Optimized**: Tuned for bass guitar frequency range (30-400 Hz)
- **Multiple Algorithms**: Currently supports YIN and FFT-based detection
- **Professional UI**: Modern, intuitive interface with real-time feedback

## Available Algorithms

### YIN Algorithm
- **Type**: Autocorrelation-based
- **Pros**: Robust, handles noise well, good for bass frequencies
- **Cons**: Higher computational cost
- **Best for**: Clean bass tones, studio recordings

### FFT Algorithm
- **Type**: Fast Fourier Transform
- **Pros**: Fast, efficient, good frequency resolution
- **Cons**: Sensitive to noise, requires windowing
- **Best for**: Real-time applications, clean signals

## Statistics Displayed

- **Current Pitch**: Real-time detected frequency in Hz
- **Current Note**: Musical note name (e.g., A2, E1)
- **Average Pitch**: Running average of detected frequencies
- **Stability**: Consistency of pitch detection (0-100%)
- **Confidence**: Algorithm's confidence in the detection (0-100%)
- **Response Time**: How quickly the algorithm responds to changes
- **Detection Count**: Total vs valid detections ratio

## Building the Plugin

### Prerequisites
- Visual Studio 2019 or later (Windows)
- CMake 3.15 or later
- JUCE framework (will be downloaded automatically)

### Build Instructions

1. **Clone the repository**:
   ```bash
   git clone https://github.com/yourusername/PitchDetectionTester.git
   cd PitchDetectionTester
   ```

2. **Build on Windows**:
   ```bash
   build.bat
   ```

3. **Manual build**:
   ```bash
   mkdir build
   cd build
   cmake .. -G "Visual Studio 17 2022" -A x64
   cmake --build . --config Release
   ```

### Output
The VST3 plugin will be created in:
```
build/PitchDetectionTester_artefacts/Release/VST3/
```

## Usage

1. **Load the plugin** in your DAW as a VST3 effect
2. **Select an algorithm** from the dropdown menu
3. **Play your bass guitar** through the plugin
4. **Monitor the statistics** in real-time
5. **Compare algorithms** by switching between them
6. **Reset statistics** to start fresh measurements

## Adding New Algorithms

The plugin is designed for easy algorithm integration:

1. **Create a new class** inheriting from `PitchDetector`
2. **Implement required methods**:
   - `prepare(sampleRate, bufferSize)`
   - `detectPitch(buffer)`
   - `getName()`
3. **Add to the processor** in `PluginProcessor.cpp`
4. **Update the algorithm list** in `getAlgorithmNames()`

Example:
```cpp
class MyPitchDetector : public PitchDetector
{
public:
    void prepare(double sampleRate, int bufferSize) override;
    float detectPitch(const juce::AudioBuffer<float>& buffer) override;
    juce::String getName() const override { return "My Algorithm"; }
};
```

## Technical Details

- **Sample Rate**: Supports standard audio sample rates (44.1kHz, 48kHz, etc.)
- **Buffer Size**: Configurable analysis buffer (default: 2048 samples)
- **Frequency Range**: 30-400 Hz (full bass guitar range including 5-string basses)
- **Latency**: Minimal processing latency for real-time use
- **CPU Usage**: Optimized for efficient real-time processing

## Bass Guitar Frequency Range

The plugin is optimized for the complete bass guitar range:
- **4-string bass**: E1 (41.2 Hz) to G4 (392 Hz)
- **5-string bass**: B0 (30.87 Hz) to G4 (392 Hz)
- **Extended range**: Covers drop tunings and extended range basses

## Performance Tips

- **Clean signal**: Use a clean bass signal for best results
- **Proper gain staging**: Ensure adequate signal level without clipping
- **Reduce noise**: Minimize background noise for accurate detection
- **Test different algorithms**: Each algorithm has different strengths

## Troubleshooting

### No pitch detected
- Check input signal level
- Ensure bass guitar is in the correct frequency range (30-400 Hz)
- Try different algorithms
- Verify audio routing in your DAW

### Unstable readings
- Reduce background noise
- Use a cleaner bass tone
- Check for interference from other plugins
- Try the YIN algorithm for better stability

### High CPU usage
- Reduce buffer size if possible
- Close other CPU-intensive applications
- Use the FFT algorithm for lower CPU usage

## Contributing

Contributions are welcome! Please feel free to:
- Add new pitch detection algorithms
- Improve the UI/UX
- Optimize performance
- Add new statistics or visualizations
- Report bugs or suggest features

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgments

- JUCE framework for the audio plugin infrastructure
- YIN algorithm implementation based on academic research
- FFT implementation using JUCE's DSP module
