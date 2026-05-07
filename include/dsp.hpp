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
        // 1. Find the peak absolute value
        T peak = 0;
        for (const auto& s : samples) {
            T abs_val = std::abs(s);
            if (abs_val > peak) peak = abs_val;
        }

        // 2. Avoid division by zero if the track is silent
        if (peak < 1e-9) return samples;

        // 3. Scale linearly (preserves 0 as center)
        std::vector<T> output(samples.size());
        T scalar = 1.0 / peak;

        #pragma omp parallel for
        for (int i = 0; i < (int)samples.size(); i++) {
            output[i] = samples[i] * scalar;
        }
        
        return output;
    }

    template<typename T>
    std::vector<T> slow_low_pass(std::vector<T> samples, uint16_t window){
        std::vector<T> output(samples.size());
        auto start = std::chrono::high_resolution_clock::now();
#pragma omp parallel for
        for(size_t i=0; i<samples.size(); i++){
            T sum = 0;
            for(size_t j=i; j<i+window; j++){
                sum += samples[j]; 
            }
            output[i] = sum/window;
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;
        std::cout << "low pass loop elapsed time: " << elapsed.count() << " ms" << std::endl;
        return output;
    }

    template<typename T>
    std::vector<T> fast_low_pass(std::vector<T> samples, T alpha){
        std::vector<T> output(samples.size());
        auto start = std::chrono::high_resolution_clock::now();
        output[0] = samples[0];
        for(size_t i=1; i<samples.size(); i++){
            T sum = 0;
            output[i] = alpha*samples[i] + output[i-1]*(1-alpha);
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;
        std::cout << "low pass loop elapsed time: " << elapsed.count() << " ms" << std::endl;
        return output;
    }

    template<typename T>
    std::vector<T> firFilter(std::vector<T> input, std::vector<T> coefficients){
        std::vector<T> output(input.size());
        auto start = std::chrono::high_resolution_clock::now();
#pragma omp parallel for
        for(auto i = 0; i<input.size(); i+=1){
            T sum0 = 0.0f;
            for(auto j = 0; j < coefficients.size(); j+=4){
                sum0 += input[i+j] * coefficients[j] +
                            input[i+j+1] * coefficients[j+1]+
                            input[i+j+2] * coefficients[j+2]+
                            input[i+j+3] * coefficients[j+3];
            }
            output[i] = sum0;
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;
        std::cout << "fir filter elapsed time: " << elapsed.count() << " ms" << std::endl;
        return output;
    }

}

#endif //DSP_HPP
