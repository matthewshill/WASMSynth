#include <emscripten/bind.h>
#include "oscillator.h"

#define _USE_MATH_DEFINES

Oscillator::Oscillator() {}

Oscillator::Oscillator(Waveform waveFrm, int sampleRate) {
    this->waveFrm = waveFrm;
    this->sampleRate = sampleRate;
    level = 0;
    status = true;
}

void Oscillator::setStatus(bool s) {
    status = s;
}

bool Oscillator::getStatus() {
    return status;
}

void Oscillator::setLevel(float value) {
    level = value;
}

void Oscillator::setWaveform(Waveform w) {
    waveFrm = w;
}

float Oscillator::frequencyConstant(int key){
    return 2.0 * M_PI * pow(2.0, (key - 69.0) / 12.0) * 440.0;
}

float Oscillator::digitalSquareWave(float x) {
    float y = sin(x);
    if(y >= 0) {
        y = 1;
    } else {
        y -= 1;
    }
    return y;
}

float Oscillator::sinoidSawWave(float x, int factor) {
  factor *= 2;
  float y = 0.0;
  for (int i = 1; i <= factor; ++i) {
    // TODO: This might not be correct and we should remove the modulo opperation
    if (i % 2 == 1) {
      y += pow(-1, i) * (sin(x * i) / i);
    }
  }
  return (1/2) - ((1/M_PI) * y);
}

float Oscillator::sinoidSquareWave(float x, int factor) {
    factor *= 2;;
    float y = 0.0;
    for(int i = 1; i <= factor; ++i) {
        if(i % 2 == 1) {
            y += sin(x * i) / (i * M_PI);
        }
    }
    return y;
}

float Oscillator::digitalSawWave(float x) {
    return 2 * ((x / M_PI) - floor(0.5 + (x / M_PI)));
}

float Oscillator::digitalTriangleWave(float x) {
    return abs(digitalSawWave(x));
}

vector<Point> Oscillator::nextSample(int key, int iteration, int bufferSize) {
    vector<Point> buffer; 
    for(int i = 0; i <  bufferSize; ++i) {
        float n = i * iteration * bufferSize;
        float f = frequencyConstant(key);
        float x = (n * f) / sampleRate;
        float y = 0.0;

        switch(waveFrm) {
            case SINE:
                y = sin(x);
                break;
            case SQUARE_DIGITAL:
                y = digitalSquareWave(x);
                break;
            case SQUARE_3:
                y = sinoidSquareWave(x,3);
                break;
            case SQUARE_4:
                y = sinoidSquareWave(x,4);
                break;
            case SQUARE_6:
                y = sinoidSquareWave(x,6);
                break;
            case SQUARE_8:
                y = sinoidSquareWave(x,8);
                break;
            case SQUARE_16:
                y = sinoidSquareWave(x,16);
                break;
            case SQUARE_32:
                y = sinoidSquareWave(x,32);
                break;
            case SQUARE_64:
                y = sinoidSquareWave(x,64);
                break;
            case SAW_DIGITAL:
                y = digitalSawWave(x);
                break;
            case SAW_3:
                y = sinoidSawWave(x,3);
                break;
            case SAW_4:
                y = sinoidSawWave(x,4);
                break;
            case SAW_6:
                y = sinoidSawWave(x,6);
                break;
            case SAW_8:
                y = sinoidSawWave(x,8);
                break;
            case SAW_16:
                y = sinoidSawWave(x,16);
                break;
            case SAW_32:
                y = sinoidSawWave(x,32);
                break;
            case SAW_64:
                y = sinoidSawWave(x,64);
                break;
            case TRIANGLE:
                y = digitalTriangleWave(x);
                break;
            default: 
                break;
        }
        buffer.push_back({x, y * level});
    }
    return buffer;
}

EMSCRIPTEN_BINDINGS(ADSRModulator) {
    emscripten::class_<Oscillator>("Oscillator")
        .constructor<>()
        .constructor<Oscillator::Waveform, int>()
        .function("nextSample", &Oscillator::nextSample)
        .function("frequencyConstant", &Oscillator::frequencyConstant)
        .function("setLevel", &Oscillator::setLevel)
        .function("setStatus", &Oscillator::setStatus)
        .function("getStatus", &Oscillator::getStatus)
        .function("setWaveform", &Oscillator::setWaveform);
}