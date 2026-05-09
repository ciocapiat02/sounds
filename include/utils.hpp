#include "dr_wav.h"
#include <iostream>
#include <string>
#include <vector>

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

void save_file(std::string filename, wav_file_t& output_file){
    drwav wav_out;
    drwav_data_format format;
    format.container = drwav_container_riff;
    format.format = DR_WAVE_FORMAT_IEEE_FLOAT; 
    format.channels = output_file.channels;
    format.sampleRate = output_file.sampleRate;
    format.bitsPerSample = 32; // Standard for float

    if (!drwav_init_file_write(&wav_out, filename.c_str(), &format, NULL)) {
        std::cerr << "Failed to open output file." << std::endl;
        return;
    }

    drwav_uint64 framesWritten = drwav_write_pcm_frames(&wav_out, output_file.totalFrames, output_file.samples.data());

    if (framesWritten != output_file.totalFrames) {
        std::cerr << "Failed to write all frames." << std::endl;
    }

    drwav_uninit(&wav_out); // This triggers the header finalization (essential!)
}
