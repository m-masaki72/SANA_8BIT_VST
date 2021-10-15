#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "SynthParameters.h"

class Waveforms {
 public:
  Waveforms();
  void init();
  float nesTriangle(float angle);
  float nesSquare(float angle);
  float nesSquare25(float angle);
  float nesSquare125(float angle);
  float sine(float angle);
  float saw(float angle);
  float square(float angle);
  float square25(float angle);
  float square125(float angle);
  float triangle(float angle);
  float longNoise(const float angleDelta);
  float shortNoise(const float angleDelta);
  float lobitNoise(const float angleDelta);
  float waveformMemory(float angleDelta,
                       WaveformMemoryParameters* _waveformMemoryParamsPtr);
 private:
  static float quantize(float sample);
  static void checkAngleRanage(float &angle);
  float high_pass(float in);

  std::uint16_t _longNoizeReg = 0x0002;
  std::uint16_t _shortNoizeReg = 0x0002;

  float _noiseVal = 1.0f;
  int _freqCounter = 0;
  double _capacitor = 0.0;
  Random _rand;
};
