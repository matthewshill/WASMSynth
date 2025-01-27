#pragma once

#include <vector>
#include <tuple>

#include "types.h"

using namespace std;

struct EnvelopePreset {
    float xa;
    float xd;
    float ys;
    float xr;
    float ya;
};

class ADSRModulator {
public:
    ADSRModulator();
    ADSRModulator(int sampleRate);

    enum EnvelopeStage {
        OFF =  0,
        ATTACK,
        DECAY,
        SUSTAIN,
        RELEASE
    };

    bool modulateAmp(vector<Point>&buffer);
    void setXA(float xa);
    void setXD(float xd);
    void setYS(float ys);
    void setXR(float xr);
    void setYA(float ya);
    void setStage(EnvelopeStage stage);
    EnvelopeStage getStage();
private:
    EnvelopeStage stage;
    int sampleRate;
    bool sustainReached;
    float xa, xd, ys, xr, ya, level, xMax;
    float modulate(float x);
    static const float DECAY_UPPER_LIMIT, RELEASE_LOWER_LIMIT;
};
