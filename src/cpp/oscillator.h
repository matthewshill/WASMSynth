#pragma once
#include <vector>
#include <tuple>

#include "types.h"

using namespace std;

class Oscillator {
public: 
    enum Waveform {
        SINE = 0,
        SQUARE_DIGITAL,
        SQUARE_3,
        SQUARE_4,
        SQUARE_6,
        SQUARE_8,
        SQUARE_16,
        SQUARE_32,
        SQUARE_64,
        SAW_DIGITAL,
        SAW_3,
        SAW_4,
        SAW_6,
        SAW_8,
        SAW_16,
        SAW_32,
        SAW_64,
        TRIANGLE
    };

    Oscillator();
    Oscillator(Waveform wf, int sampleRate);
    vector<Point> nextSample(int key, int iteration, int bufferSize);
    static float frequencyConstant(int key);
    void setLevel(float value);
    void setWaveform(Waveform wf);
    void setStatus(bool s);
    bool getStatus();

private:
    bool status;
    int sampleRate;
    Waveform waveFrm;
    float level;
    vector<Point> buffer;
    float digitalSquareWave(float x);
    float sinoidSquareWave(float x, int factor);
    float sinoidSawWave(float x, int factor);
    float digitalSawWave(float x);
    float digitalTriangleWave(float x);
};