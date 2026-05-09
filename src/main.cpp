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
#include "utils.hpp"

struct Oscillator_t {
    double phase;
    double frequency;
    double sampleRate;
    double amplitude;
    Oscillator_t(){
        phase=0.0;
        frequency=440.0;
        sampleRate=48000.0;
        amplitude=0.2;
    }
};

int audioCallback(void* outputBuffer,
                  void* inputBuffer,
                  unsigned int nBufferFrames,
                  double streamTime,
                  RtAudioStreamStatus status,
                  void* userData) {

    double* buffer = (double*)outputBuffer;
    Oscillator_t* osc = (Oscillator_t*) userData;
    double phaseIncrement = (2.0 * M_PI * osc->frequency) / osc->sampleRate;

    for (size_t i = 0; i < nBufferFrames; ++i) {
        double sample = (double)(osc->amplitude * sin(osc->phase)); 

        *buffer++ = sample; // left
        *buffer++ = sample; // right

        osc->phase += phaseIncrement;

        if (osc->phase >= 2 * M_PI)
            osc->phase -= 2 * M_PI;
    }

    return 0;
}
int main(int argc, char* argv[]) {

    std::string filename = "../test_tracks/the_bass_and_the_melody.wav";
    std::string fir_filename = "../test_tracks/ir-basement-.wav";

    // wav_file_t input_file;
    // wav_file_t fir_file;
    //
    // open_file(filename, input_file);
    // open_file(fir_filename, fir_file);
    //
    // save_file(std::string("../outputs/test2.wav"), input_file);
    RtAudio dac;
    Oscillator_t osc;

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
    RtAudio::DeviceInfo info;
    for (unsigned int n = 0; n < ids.size(); n++) {

        info = dac.getDeviceInfo(ids[n]);

        // Print, for example, the name and maximum number of output channels for each device
        std::cout << "device name = " << info.name << std::endl;
        std::cout << "device id = " << info.ID << std::endl;
        std::cout << ": maximum output channels = " << info.outputChannels << std::endl;
    }
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

    std::cout << "Playing sine wave at " << osc.frequency << "Hz." << std::endl;
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
