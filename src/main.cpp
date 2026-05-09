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
#include "utils.hpp"

int main(int argc, char* argv[]) {

    std::string filename = "../test_tracks/the_bass_and_the_melody.wav";
    std::string fir_filename = "../test_tracks/ir-basement-.wav";

    wav_file_t input_file;
    wav_file_t fir_file;

    open_file(filename, input_file);
    open_file(fir_filename, fir_file);
    // std::vector<float> tail(fir_file.samples.begin(), fir_file.samples.end());
    fir_file.samples = std::vector<float>(fir_file.samples.begin(), fir_file.samples.end()-300e3);
    // fir_file.samples = std::vector<float>(input_file.samples.begin()+9389355, input_file.samples.end()-9389355);//-469450);
    std::cout << "samples in filter: " << fir_file.samples.size() << std::endl;
    std::cout << "samples in track: " << input_file.samples.size() << std::endl;

    // samples = dsp::distortion<float>(samples, -0.5, 0.5);
    // samples = dsp::slow_low_pass<float>(samples, 5);
    // input_file.samples = dsp::fast_low_pass<float>(input_file.samples, 0.001);
    input_file.samples = dsp::firFilter<float>(input_file.samples, fir_file.samples);
    input_file.samples = dsp::normalization<float>(input_file.samples);


    save_file(std::string("../outputs/test2.wav"), input_file);

    return 0;
}
