#ifndef DSP_HPP
#define DSP_HPP

#include <vector>
#include <chrono>
#include <iostream>
#include <algorithm>
#include <omp.h>

namespace dsp {
    template<typename T>
    std::vector<T> distortion(std::vector<T> samples, T min_cap, T max_cap){
        std::vector<T> output(samples.size());

        auto start = std::chrono::high_resolution_clock::now();
#pragma omp parallel for
        for(int i=0; i < samples.size(); i++){
            if(samples[i] > max_cap){
                output[i] = max_cap;
            } else if(samples[i] < min_cap){
                output[i] = min_cap;
            } else {
                output[i] = samples[i];
            }
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;
        std::cout << "distortion loop elapsed time: " << elapsed.count() << " ms" << std::endl;
        return output;
    }

    template<typename T>
    std::vector<T> normalization(std::vector<T> samples){
    std::vector<T> output(samples.size());
    T min = *std::min_element(samples.begin(), samples.end());
    T max = *std::max_element(samples.begin(), samples.end());

    auto start = std::chrono::high_resolution_clock::now();
#pragma omp parallel for
    for(int i=0; i<samples.size(); i++){
        output[i] = ((samples[i] - min) / (max-min));
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "normalization loop elapsed time: " << elapsed.count() << " ms" << std::endl;
    return output;

}
}

#endif //DSP_HPP
