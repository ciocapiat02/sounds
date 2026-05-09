#include <oscillator.hpp>

Oscillator::Oscillator() {
    this->phase = 0.0;
    this->frequency = 440.0;
    this->sampleRate = 48000.0;
    this->amplitude = 0.2;
}

Oscillator::Oscillator(double frequency, double sampleRate, double amplitude) {
    this->phase = 0.0;
    this->frequency = frequency;
    this->sampleRate = sampleRate;
    this->amplitude = amplitude;
}

int Oscillator::OscAudioCallback(void* outputBuffer,
                                 void* inputBuffer,
                                 unsigned int nBufferFrames,
                                 double streamTime,
                                 RtAudioStreamStatus status,
                                 void* userData) {
    double* buffer = (double*)outputBuffer;
    // Oscillator_t* osc = (Oscillator_t*)userData;
    double phaseIncrement = (2.0 * M_PI * this->frequency) / this->sampleRate;

    for (size_t i = 0; i < nBufferFrames; ++i) {
        double sample = (double)(this->amplitude * sin(this->phase));

        *buffer++ = sample; // left
        *buffer++ = sample; // right

        this->phase += phaseIncrement;

        if (this->phase >= 2 * M_PI)
            this->phase -= 2 * M_PI;
    }

    return 0;
}

