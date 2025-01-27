#pragma once
#include <vector>
#include <map>

#include "voice.h"
#include "oscillator.h"

using namespace std;

typedef vector<Voice*> Voices;

class VoiceManager {
private:
    int numOfVoices, sampleRate, numOfOscillators;
    Voices voices;
    Voice * findFreeVoice();
    map<int, EnvelopePreset> envelopes;
    map<int, float> levels;
    void setEnvelope(int i);
    void setAllEnvelopes();
    void setLevel(int i);
    void setAllLevels();

public: 
    VoiceManager(int sampleRate, int numOfVoices, int numOfOscillators);
    void onNoteOn(int key);
    void onNoteOff(int keu);
    vector<float> nextSample(int bufferSize);
    void updateLevel(int i, float value);
    void updateEnvelope(int i, float xa, float xd, float ys, float xr, float ya);
    void enableOscillator(int i, bool b);
    void updateWaveform(int i, int w);
};