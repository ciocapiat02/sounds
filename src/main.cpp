#include <algorithm>
#include <cstdint>
#include <iostream>
#include <limits>
#include <vector>
#include <omp.h>
#include <chrono>

#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"
#include "dsp.hpp"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: sounds {filename}" << std::endl;
        return 1;
    }
    std::string filename = argv[1];

    drwav wav_in;
    if (!drwav_init_file(&wav_in, filename.c_str(), NULL)) {
        std::cerr << "failed to open input file " << filename << std::endl;
        return 2;
    }

    std::vector<float> samples(wav_in.totalPCMFrameCount * wav_in.channels);
    drwav_read_pcm_frames_f32(&wav_in, wav_in.totalPCMFrameCount, samples.data());
    
    // Save metadata before closing the reader
    unsigned int channels = wav_in.channels;
    unsigned int sampleRate = wav_in.sampleRate;
    drwav_uint64 totalFrames = wav_in.totalPCMFrameCount;

    drwav_uninit(&wav_in); // Done with the input file

    // samples = dsp::distortion<float>(samples, -0.5, 0.5);
    // samples = dsp::slow_low_pass<float>(samples, 5);
    samples = dsp::fast_low_pass<float>(samples, 0.1);
    samples = dsp::normalization<float>(samples);


    drwav wav_out;
    drwav_data_format format;
    format.container = drwav_container_riff;
    format.format = DR_WAVE_FORMAT_IEEE_FLOAT; 
    format.channels = channels;
    format.sampleRate = sampleRate;
    format.bitsPerSample = 32; // Standard for float

    std::string output_file = "../outputs/test.wav";
    if (!drwav_init_file_write(&wav_out, output_file.c_str(), &format, NULL)) {
        std::cerr << "Failed to open output file." << std::endl;
        return 3;
    }

    drwav_uint64 framesWritten = drwav_write_pcm_frames(&wav_out, totalFrames, samples.data());

    if (framesWritten != totalFrames) {
        std::cerr << "Failed to write all frames." << std::endl;
    }

    drwav_uninit(&wav_out); // This triggers the header finalization (essential!)
    
    std::cout << "File written successfully to: " << output_file << std::endl;
    return 0;
}
