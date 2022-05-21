#pragma once
class ChipOscillatorParameters;

class ColorEnvelope {
 public:
  ColorEnvelope(ChipOscillatorParameters* chipOscParam);
  float getManipulateAngle();
  void clear();
  void cycle(float sampleRate);
 private:
  float degreeFactor(int degree);

  ChipOscillatorParameters* _chipOscParam = nullptr;
  float _envDuration = 0.05f;
  float _timer = 0.0f;
  int _index = 0;
  bool _isLoop = true;
};