#pragma once
#include <numeric>

#include "oscillator.h"
#include "adsr_modulator.h"

typedef vector<Oscillator*> Oscillators;
typedef vector<ADSRModulator*> ADSRModulators;

class Voice {
private:
    Oscillators oscillators;
    ADSRModulators modulators;
public:
    int key, iteration, numOfOscillators;
    bool isActive;
    Voice();
    Voice(int sampleRate, int numOfOscillators);
    vector<float> nextSample(int bufferSize);
    void setEnvelope(int i, EnvelopePreset);
    void setLevel(int i, float value);
    void setStage(ADSRModulator::EnvelopeStage stage);
    void setWaveform(int i, Oscillator::Waveform wf);
    void enableOscillator(int i, bool b);
};