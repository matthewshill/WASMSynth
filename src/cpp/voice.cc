#include <emscripten/bind.h>
#include "voice.h"

Voice::Voice(){
    this->isActive = false;
}

Voice::Voice(int sampleRate, int numOfOscillators) : oscillators(), modulators() {
    this->isActive = false;
    this->numOfOscillators = numOfOscillators;

    this->oscillators.reserve(this->numOfOscillators);
    this->modulators.reserve(this->numOfOscillators);
    for(int i = 0; i < this->numOfOscillators; ++i) {
        this->oscillators.push_back(new Oscillator(Oscillator::SINE, sampleRate));
        this->modulators.push_back(new ADSRModulator(sampleRate));
    }
}

void Voice::setLevel(int i, float value) {
    Oscillator *o = oscillators[i];
    o->setLevel(value);
}

void Voice::setStage(ADSRModulator::EnvelopeStage stage) {
    for(int i = 0; i < numOfOscillators; ++i) {
        ADSRModulator *m = modulators[i];
        m->setStage(stage);
    }
}

vector<float> Voice::nextSample(int bufferSize) {
    vector<float> buf(bufferSize, 0.0);
    vector<bool> modsOn(numOfOscillators, false);

    for(int i = 0; i < numOfOscillators; ++i) {
        Oscillator *o = oscillators[i];
        
        if(!o->getStatus()) {
            // oscillator is decativated
            continue;
        }
        ADSRModulator *m = modulators[i];
        vector<Point> newFrame = o->nextSample(key, iteration, bufferSize);
        modsOn[i] = m->getStage() != ADSRModulator::EnvelopeStage::OFF;

        if(modsOn[i]) {
            m->modulateAmp(newFrame);
            for(int j = 0; j < bufferSize; ++j) {
                buf[j] += newFrame[j].y;
            }
        }
    }
    int sum = accumulate(modsOn.begin(), modsOn.end(), 0);
    isActive = sum != 0;
    this->iteration++;
    return buf;
}

void Voice::setEnvelope(int i, EnvelopePreset envelope) {
    ADSRModulator *m = modulators[i];
    const float microseconds = 1000.0 * 1000.0;
    float c = Oscillator::frequencyConstant(this->key);
    m->setXA(c * envelope.xa / microseconds);
    m->setXD(c * envelope.xd / microseconds);
    m->setYS(c * envelope.ys / microseconds);
    m->setXR(c * envelope.xr / microseconds);
    m->setYA(c * envelope.ya / microseconds);
}

void Voice::setWaveform(int i , Oscillator::Waveform w) {
    Oscillator *o = oscillators[i];
    o->setWaveform(w);
}

void Voice::enableOscillator(int i, bool b) {
    Oscillator *o = oscillators[i];
    o->setStatus(b);
}

EMSCRIPTEN_BINDINGS(Voice) {
  emscripten::class_<Voice>("Voice")
    .constructor<>()
    .constructor<int, int>()
    .function("nextSample", &Voice::nextSample)
    .function("setEnvelope", &Voice::setEnvelope)
    .function("setLevel", &Voice::setEnvelope)
    .function("setStage", &Voice::setEnvelope)
    .function("setWaveForm", &Voice::setEnvelope)
    .function("enableOscillator", &Voice::enableOscillator);
}
