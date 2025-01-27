#include <math.h>
#include <emscripten/bind.h>
#include "adsr_modulator.h"


using namespace std;

const float ADSRModulator::DECAY_UPPER_LIMIT = 1.0f;
const float ADSRModulator::RELEASE_LOWER_LIMIT = 0.0f;

ADSRModulator::ADSRModulator() {}

ADSRModulator::ADSRModulator(int sampleRate) {
    this->sampleRate = sampleRate;
    this->stage = ATTACK;
    this->sustainReached = false;
    this->level = 0.0;
    this->xMax = 0.0;
}

void ADSRModulator::setStage(EnvelopeStage stage) {
    this->stage = stage;
}

ADSRModulator::EnvelopeStage ADSRModulator::getStage() {
    return stage;
}

float ADSRModulator::modulate(float x) {
    if(this->stage == ATTACK) {
        float attack = pow(x / this->xa, 1.0 / 3.0f);

        this->level = attack;
        if(attack <= this->ya) {
            this->xMax = x;
            return attack;
        } else{
            setStage(DECAY);
        }
    }

    if(this->stage == DECAY) {
        float dx =((this->ys - 1) / this->xd) * (x - this->xa) + 1;
        float dl = this->ys;
        float du = DECAY_UPPER_LIMIT;
        float gf = pow((dx - dl) / (du - dl), 3);
        float decay = gf * (du - dl) + dl;

        this->level = decay;
        if(decay >= this->ys) {
            this->xMax = x;
            return decay;
        } else {
            this->sustainReached = true;
            setStage(SUSTAIN);
        }
    }

    if(this->stage == SUSTAIN) {
        this->xMax = x;
        this->level = this->ys;
        return this->ys;
    }

    if(this->stage == RELEASE) {
        float rx = -1.0f * this->level / this->xr * (x - this->xMax) + this->level;
		float rl = RELEASE_LOWER_LIMIT;
        float ru = this->level;
        float rf = pow((rx - rl) / (ru - rl), 3);
        float release = rf * (ru - rl) + rl;
        if(release > rl) {
            return release;
        } else {
            this->xMax = 0.0;
            this->level = 0.0;
            this->sustainReached = false;
            setStage(OFF);
        }
    }

    if(this->stage == OFF) {
        return 0.0;
    }
}

bool ADSRModulator::modulateAmp(vector<Point> &buffer) {
    for(vector<int>::size_type i = 0; i < buffer.size(); i++) {
        float x = buffer[i].x;
        float y = buffer[i].y;
    }
    return ADSRModulator::OFF == stage;
}

void ADSRModulator::setXA(float xa) {
    this->xa = xa;
}

void ADSRModulator::setXD(float xd) {
    this->xd = xd;
}

void ADSRModulator::setYS(float ys) {
    this->ys = ys;
}

void ADSRModulator::setXR(float xr) {
    this->xr = xr;
}

void ADSRModulator::setYA(float ya) {
    this->ya = ya;
}

EMSCRIPTEN_BINDINGS(ADSRModulator) {
    emscripten::class_<ADSRModulator>("ADSRModulator")
        .constructor<>()
        .constructor<int>()
        .function("modulateAmp", &ADSRModulator::modulateAmp)
        .function("setXA", &ADSRModulator::setXA)
        .function("setXD", &ADSRModulator::setXD)
        .function("setYS", &ADSRModulator::setYS)
        .function("setXR", &ADSRModulator::setXR)
        .function("setYA", &ADSRModulator::setYA)
        .function("setStage", &ADSRModulator::setStage)
        .function("getStage", &ADSRModulator::getStage);
}