#include <emscripten/bind.h>
#include "voice_manager.h"

VoiceManager::VoiceManager(int sampleRate, int numOfVoices, int numOfOscillators) : voices() {
    this->sampleRate = sampleRate;
    this->numOfVoices = numOfVoices;
    this->numOfOscillators = numOfOscillators;
    this->voices.reserve(this->numOfVoices);

    for(int i = 0; i < this->numOfVoices; ++i) {
        this->voices.push_back(new Voice(sampleRate, this->numOfOscillators));
    }
}

void VoiceManager::updateEnvelope(int i, float xa, float xd, float ys, float xr,
                                  float ya) {
  envelopes[i] = { xa, xd, ys, xr, ya };
  setEnvelope(i);
}

void VoiceManager::setEnvelope(int i) {
  for (Voices::iterator it = this->voices.begin(); it != this->voices.end(); ++it) {
    Voice *v = *it;
    if (v->isActive) {
      v->setEnvelope(i, envelopes[i]);
    }
  }
}

void VoiceManager::setAllEnvelopes() {
  for (int i = 0; i < numOfOscillators; ++i) {
    setEnvelope(i);
  }
}

void VoiceManager::onNoteOn(int key) {
  Voice *v = findFreeVoice();
  v->key = key;
  v->iteration = 0;
  v->isActive = true;
  setAllEnvelopes();
  setAllLevels();
  v->setStage(ADSRModulator::ATTACK);
}

void VoiceManager::onNoteOff(int key) {
  for (Voices::iterator it = this->voices.begin(); it != this->voices.end(); ++it) {
    Voice *v = *it;
    if (v->key == key && v->isActive) {
      v->setStage(ADSRModulator::RELEASE);
    }
  }
}

Voice * VoiceManager::findFreeVoice() {
  for (Voices::iterator it = this->voices.begin(); it != this->voices.end(); ++it) {
    Voice *v = *it;
    if (!v->isActive) {
      return v;
    }
  }
  return NULL;
}

vector<float> VoiceManager::nextSample(int bufferSize) {
  vector<float> sample(bufferSize, 0.0f);
  for (Voices::iterator it = this->voices.begin(); it != this->voices.end(); ++it) {
    Voice *v = *it;
  
    if (v->isActive) {
      vector<float> voiceSample = v->nextSample(bufferSize);
        for (int i = 0; i < bufferSize; i++) {
          // TODO: Make total gain a UI element
          sample[i] += voiceSample[i] * 0.1;
        }
    }
  }

  return sample;
}

void VoiceManager::updateLevel(int i, float value) {
  levels[i] = value;
  setLevel(i);
}

void VoiceManager::setLevel(int i) {
  for (Voices::iterator it = this->voices.begin(); it != this->voices.end(); ++it) {
    Voice *v = *it;
    if (v->isActive) {
      v->setLevel(i, levels[i]);
    }
  }
}

void VoiceManager::setAllLevels() {
  for (int i = 0; i < numOfOscillators; ++i) {
    setLevel(i);
  }
}

void VoiceManager::updateWaveform(int i, int w) {
  Oscillator::Waveform wCast = static_cast<Oscillator::Waveform>(w);
  for (Voices::iterator it = this->voices.begin(); it != this->voices.end(); ++it) {
    Voice *v = *it;
    v->setWaveform(i, wCast);
  }
}

void VoiceManager::enableOscillator(int i, bool b) {
  for (Voices::iterator it = this->voices.begin(); it != this->voices.end(); ++it) {
    Voice *v = *it;
    v->enableOscillator(i, b);
  }
}

EMSCRIPTEN_BINDINGS(VoiceManager) {
  emscripten::class_<VoiceManager>("VoiceManager")
    .constructor<int, int, int>()
    .function("onNoteOn", &VoiceManager::onNoteOn)
    .function("onNoteOff", &VoiceManager::onNoteOff)
    .function("nextSample", &VoiceManager::nextSample)
    .function("updateLevel", &VoiceManager::updateLevel)
    .function("updateEnvelope", &VoiceManager::updateEnvelope)
    .function("updateWaveForm", &VoiceManager::updateWaveform)
    .function("enableOscillator", &VoiceManager::enableOscillator);
  emscripten::register_vector<float>("vector<float>");
}