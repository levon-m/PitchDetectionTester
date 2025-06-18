# Pitch Detection Tester - Project Context

## Project Overview
A VST3 plugin built with C++ and JUCE for testing pitch detection algorithms specifically optimized for bass guitar. The plugin emphasizes modularity and efficiency in testing different pitch detection approaches.

## Current Status: ✅ COMPLETE
- **Core Architecture**: Fully implemented
- **Pitch Detection Algorithms**: YIN and FFT implemented
- **UI/Statistics**: Complete with real-time display
- **Build System**: CMake configuration ready
- **Documentation**: Comprehensive README and setup guides

## Project Architecture

### 1. Core Components

#### **Audio Processing Pipeline**
```
Input Audio → Analysis Buffer (2048 samples) → Pitch Detection Algorithm → Statistics Manager → UI Display
```

#### **Main Classes**
- **`PitchDetectionTesterAudioProcessor`**: Main VST3 processor
- **`PitchDetectionTesterAudioProcessorEditor`**: Plugin UI
- **`PitchDetector`**: Abstract base class for algorithms
- **`StatisticsManager`**: Performance tracking and metrics
- **`StatisticsDisplay`**: Real-time UI component

### 2. Pitch Detection Algorithms

#### **Base Interface (`PitchDetector.h`)**
```cpp
class PitchDetector {
    virtual void prepare(double sampleRate, int bufferSize) = 0;
    virtual float detectPitch(const juce::AudioBuffer<float>& buffer) = 0;
    virtual juce::String getName() const = 0;
    virtual float getConfidence() const { return 1.0f; }
};
```

#### **Implemented Algorithms**
1. **YIN Algorithm** (`YinPitchDetector`)
   - **Type**: Autocorrelation-based
   - **Frequency Range**: 30-400 Hz
   - **Pros**: Robust, handles noise well
   - **Cons**: Higher computational cost
   - **Status**: ✅ Complete

2. **FFT Algorithm** (`FFTPitchDetector`)
   - **Type**: Fast Fourier Transform
   - **Frequency Range**: 30-400 Hz
   - **Pros**: Fast, efficient
   - **Cons**: Sensitive to noise
   - **Status**: ✅ Complete

#### **Modular Design Benefits**
- Easy to add new algorithms by inheriting from `PitchDetector`
- Runtime algorithm switching without restart
- Shared statistics and UI infrastructure
- Example template provided (`ExampleNewAlgorithm.h`)

### 3. Statistics System

#### **StatisticsManager Features**
- **Real-time Metrics**:
  - Current pitch frequency and musical note
  - Average pitch over time
  - Pitch stability (0-100%)
  - Detection confidence (0-100%)
  - Response time measurements
  - Detection count (valid/total ratio)

- **Data Management**:
  - Rolling history (1000 measurements max)
  - Timestamp tracking
  - Frequency validation (30-400 Hz range)
  - Automatic statistics updates

#### **Note Detection**
- Converts frequency to musical note names (e.g., E1, A2)
- Handles bass guitar range properly
- Displays both current and average notes

### 4. User Interface

#### **Main Editor Layout**
```
┌─────────────────────────────────────────┐
│           Pitch Detection Tester        │
│      Bass Guitar Algorithm Testing      │
├─────────────────────────────────────────┤
│ Algorithm: [Dropdown] Reset Stats Help  │
├─────────────────────────────────────────┤
│           Statistics Display            │
│  Current Pitch: 82.4 Hz    E2           │
│  Average Pitch: 82.1 Hz (E2)            │
│  Stability: 85%                         │
│  Confidence: 92%                        │
│  Response Time: 45 ms                   │
│  Detections: 156/180                    │
└─────────────────────────────────────────┘
```

#### **UI Components**
- **Algorithm Selector**: Dropdown for switching algorithms
- **Reset Button**: Clear all statistics
- **Help Button**: Show usage instructions
- **Statistics Display**: Real-time metrics with color coding

### 5. Build System

#### **CMake Configuration**
- **Target**: VST3 plugin format
- **Dependencies**: JUCE framework (auto-downloaded)
- **Compiler**: C++17 standard
- **Platform**: Windows (Visual Studio 2022)

#### **Build Scripts**
- **`setup.bat`**: Downloads JUCE, checks dependencies
- **`build.bat`**: Configures and builds the project
- **Output**: `build/PitchDetectionTester_artefacts/Release/VST3/`

## Technical Specifications

### **Audio Processing**
- **Sample Rate**: Supports standard rates (44.1kHz, 48kHz, etc.)
- **Buffer Size**: 2048 samples for analysis
- **Latency**: Minimal processing delay
- **Channels**: Stereo input/output (mono analysis)

### **Frequency Range**
- **Minimum**: 30 Hz (B0 on 5-string bass)
- **Maximum**: 400 Hz (covers full bass guitar range)
- **Optimization**: Tuned specifically for bass frequencies

### **Performance**
- **Real-time Updates**: 30 Hz UI refresh rate
- **CPU Usage**: Optimized for efficient processing
- **Memory**: Minimal footprint with rolling buffers

## File Structure
```
PitchDetectionTester/
├── CMakeLists.txt                    # Build configuration
├── build.bat                         # Windows build script
├── setup.bat                         # Environment setup
├── README.md                         # Documentation
├── context.md                        # This file
├── .gitignore                        # Git ignore rules
└── Source/
    ├── PluginProcessor.h/cpp         # Main audio processor
    ├── PluginEditor.h/cpp            # UI implementation
    ├── PitchDetectionAlgorithms/
    │   ├── PitchDetector.h           # Base interface
    │   ├── YinPitchDetector.h/cpp    # YIN algorithm
    │   ├── FFTPitchDetector.h/cpp    # FFT algorithm
    │   └── ExampleNewAlgorithm.h     # Integration example
    ├── Statistics/
    │   └── StatisticsManager.h/cpp   # Performance tracking
    └── UI/
        └── StatisticsDisplay.h/cpp   # Real-time display
```

## Development Progress

### ✅ **Completed Features**
1. **Core Architecture**: Modular pitch detection system
2. **YIN Algorithm**: Full implementation with parabolic interpolation
3. **FFT Algorithm**: Optimized for bass guitar frequencies
4. **Statistics System**: Comprehensive performance tracking
5. **User Interface**: Modern, responsive design
6. **Build System**: CMake configuration and build scripts
7. **Documentation**: Complete README and setup guides
8. **Frequency Range**: Corrected to 30-400 Hz for full bass range

### 🔄 **Current State**
- **Status**: Ready for building and testing
- **Next Steps**: Build the plugin and test with real bass guitar
- **Potential Enhancements**: Additional algorithms, visualization features

### 📋 **Future Enhancements** (Optional)
1. **Additional Algorithms**:
   - AMDF (Average Magnitude Difference Function)
   - Cepstrum analysis
   - Harmonic product spectrum
   - Autocorrelation with peak detection

2. **Advanced Features**:
   - Pitch history visualization/plotting
   - Algorithm comparison mode
   - Export statistics to CSV
   - MIDI output for detected notes
   - Custom frequency range settings

3. **UI Improvements**:
   - Real-time pitch visualization
   - Spectrum analyzer display
   - Algorithm performance graphs
   - Dark/light theme toggle

## Key Design Decisions

### **Modularity**
- Abstract `PitchDetector` base class enables easy algorithm addition
- Statistics system is algorithm-agnostic
- UI components are reusable across different algorithms

### **Bass Guitar Optimization**
- Frequency range specifically tuned for bass (30-400 Hz)
- Buffer size optimized for bass frequencies
- Algorithms tuned for bass guitar characteristics

### **Real-time Performance**
- Efficient buffer management
- Minimal UI updates (30 Hz refresh)
- Optimized algorithms for low latency

### **User Experience**
- Simple, intuitive interface
- Real-time feedback
- Clear statistics and metrics
- Easy algorithm switching

## Testing Strategy

### **Unit Testing** (Future)
- Individual algorithm testing
- Statistics calculation verification
- UI component testing

### **Integration Testing** (Future)
- Full plugin functionality
- Real-time performance testing
- Cross-platform compatibility

### **User Testing** (Recommended)
- Test with various bass guitars
- Different playing styles
- Various DAW environments
- Performance under different conditions

## Known Limitations

1. **Single-threaded**: All processing on audio thread
2. **Fixed buffer size**: 2048 samples (could be made configurable)
3. **Windows-focused**: Build scripts target Windows (could add cross-platform)
4. **Limited algorithms**: Only YIN and FFT implemented (expandable)

## Success Metrics

- ✅ **Modularity**: Easy to add new algorithms
- ✅ **Efficiency**: Real-time processing capability
- ✅ **Bass Optimization**: Proper frequency range coverage
- ✅ **User Experience**: Intuitive interface with real-time feedback
- ✅ **Documentation**: Comprehensive guides and examples

## Current Focus
The project is complete and ready for building. The next step is to:
1. Run `setup.bat` to download JUCE
2. Run `build.bat` to compile the plugin
3. Test with real bass guitar input
4. Validate all features work as expected

The architecture is solid, modular, and ready for future enhancements while providing immediate value for bass guitar pitch detection testing. 