#ifndef OSCILLATOR_HPP
#define OSCILLATOR_HPP
#include <rtaudio/RtAudio.h>
#include <cmath>

class Oscillator {
  private:
    double phase;
    double frequency;
    double sampleRate;
    double amplitude;

  public:
    Oscillator();
    Oscillator(double frequency, double sampleRate, double amplitude);

    int OscAudioCallback(void* outputBuffer,
                         void* inputBuffer,
                         unsigned int nBufferFrames,
                         double streamTime,
                         RtAudioStreamStatus status,
                         void* userData);

    double getFrequency(){return frequency;}
};
#endif //OSCILLATOR_HPP
