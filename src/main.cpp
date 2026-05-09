#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>
#include <omp.h>
#include <rtaudio/RtAudio.h>
#include <stdexcept>
#include <unistd.h>
#include <vector>

#define DR_WAV_IMPLEMENTATION

#include "dr_wav.h"
#include "dsp.hpp"
#include "oscillator.hpp"
#include "utils.hpp"

int audioCallback(void* outputBuffer,
                  void* inputBuffer,
                  unsigned int nBufferFrames,
                  double streamTime,
                  RtAudioStreamStatus status,
                  void* userData) {

    Oscillator* osc = static_cast<Oscillator*>(userData);
    osc->OscAudioCallback(outputBuffer, inputBuffer, nBufferFrames, streamTime, status, NULL);

    return 0;
}

int main(int argc, char* argv[]) {

    std::string filename = "../test_tracks/the_bass_and_the_melody.wav";
    std::string fir_filename = "../test_tracks/ir-basement-.wav";

    RtAudio dac;
    Oscillator osc;

    // Check if there are any audio devices available
    if (dac.getDeviceCount() < 1) {
        std::cout << "No audio devices found!" << std::endl;
        return 1;
    }

    std::vector<unsigned int> ids = dac.getDeviceIds();
    if (ids.size() == 0) {
        std::cout << "No devices found." << std::endl;
        return 0;
    }
    // Scan through devices for various capabilities

    // Set output parameters
    RtAudio::StreamParameters parameters;
    parameters.deviceId = dac.getDefaultOutputDevice();
    parameters.nChannels = 2; // Stereo
    parameters.firstChannel = 0;

    unsigned int bufferFrames = 256; // Low latency
    double fs = 48000.0;

    try {
        std::cout << "using device " << parameters.deviceId << std::endl;
        dac.openStream(&parameters, NULL, RTAUDIO_FLOAT64, fs, &bufferFrames, &audioCallback, &osc);
        dac.startStream();
    } catch (const std::runtime_error& e) {                      // Use std::runtime_error
        std::cerr << "RtAudio Error: " << e.what() << std::endl; // Use .what()
        return 1;
    }

    std::cout << "Playing sine wave at " << osc.getFrequency() << "Hz." << std::endl;
    std::cout << "Press Enter to stop..." << std::endl;

    // The main thread waits here while the audio thread hums along
    std::cin.get();

    // Cleanup
    try {
        dac.stopStream();
    } catch (const std::runtime_error& e) {
        std::cerr << "Stop Stream Error: " << e.what() << std::endl;
    }

    if (dac.isStreamOpen())
        dac.closeStream();
    return 0;
}
