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

struct wav_file_t {
    unsigned int channels;
    unsigned int sampleRate;
    drwav_uint64 totalFrames;
    std::vector<float> samples;
};

void open_file(std::string filename, wav_file_t& input_file){
    drwav wav_in;
    if (!drwav_init_file(&wav_in, filename.c_str(), NULL)) {
        std::cerr << "failed to open input file " << filename << std::endl;
        return;
    }

    std::vector<float> samples(wav_in.totalPCMFrameCount * wav_in.channels);
    drwav_read_pcm_frames_f32(&wav_in, wav_in.totalPCMFrameCount, samples.data());
    
    // Save metadata before closing the reader
    input_file.channels = wav_in.channels;
    input_file.sampleRate = wav_in.sampleRate;
    input_file.totalFrames = wav_in.totalPCMFrameCount;
    input_file.samples = samples;

    drwav_uninit(&wav_in); // Done with the input file
}

int main(int argc, char* argv[]) {

    std::string filename = "../test_tracks/the_bass_and_the_melody.wav";
    std::string fir_filename = "../test_tracks/pipe.wav";

    wav_file_t input_file;
    wav_file_t fir_file;

    open_file(filename, input_file);
    open_file(fir_filename, fir_file);
    // std::vector<float> tail(fir_file.samples.begin(), fir_file.samples.end());
    fir_file.samples = std::vector<float>(fir_file.samples.begin()+349e3, fir_file.samples.end());
    std::cout << "samples in filter: " << fir_file.samples.size() << std::endl;
    std::cout << "samples in track: " << input_file.totalFrames << std::endl;
    // samples = dsp::distortion<float>(samples, -0.5, 0.5);
    // samples = dsp::slow_low_pass<float>(samples, 5);
    // *input_file.samples = dsp::fast_low_pass<float>(*input_file.samples, 0.1);
    input_file.samples = dsp::firFilter<float>(input_file.samples, fir_file.samples);
    input_file.samples = dsp::normalization<float>(input_file.samples);


    drwav wav_out;
    drwav_data_format format;
    format.container = drwav_container_riff;
    format.format = DR_WAVE_FORMAT_IEEE_FLOAT; 
    format.channels = input_file.channels;
    format.sampleRate = input_file.sampleRate;
    format.bitsPerSample = 32; // Standard for float

    std::string output_file = "../outputs/test.wav";
    if (!drwav_init_file_write(&wav_out, output_file.c_str(), &format, NULL)) {
        std::cerr << "Failed to open output file." << std::endl;
        return 3;
    }

    drwav_uint64 framesWritten = drwav_write_pcm_frames(&wav_out, input_file.totalFrames, input_file.samples.data());

    if (framesWritten != input_file.totalFrames) {
        std::cerr << "Failed to write all frames." << std::endl;
    }

    drwav_uninit(&wav_out); // This triggers the header finalization (essential!)
    
    std::cout << "File written successfully to: " << output_file << std::endl;
    return 0;
}
