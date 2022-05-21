#include "ColorEnvelope.h"

#include "SynthParameters.h"

ColorEnvelope::ColorEnvelope(ChipOscillatorParameters* chipOscParam) {
  _chipOscParam = chipOscParam;
}

float ColorEnvelope::getManipulateAngle() {
  const auto name = _chipOscParam->ColorType->getCurrentChoiceName();
  if (name == "ARP_Octave") {
    if (_index % 2 == 0) {
      return 1;
    } else {
      return 0;
    } 
  } else if (name ==  "ARP_4th") {
    if (_index % 2 == 0) {
      return degreeFactor(5);
    } else {
      return 0;
    }
  } else if (name ==  "ARP_5th") {
    if (_index % 2 == 0) {
      return degreeFactor(7);
    } else {
      return 0;
    }
  } else if (name ==  "ARP_Major") {
    if (_index % 3 == 0) {
      return degreeFactor(4);
    } else if (_index % 3 == 1) {
      return degreeFactor(7);
    } else {
      return 0;
    }  
  } else if (name == "ARP_Major7th") {
    if (_index % 4 == 0) {
      return degreeFactor(4);
    } else if (_index % 4 == 1) {
        return degreeFactor(7);
    } else if (_index % 4 == 2) {
        return degreeFactor(11);
    } else {
      return 0;
    }
  } else if (name == "ORC_HIT") {
    _isLoop = false;
    if (_index == 0) {
      return degreeFactor(12);
    } else {
      return 0;
    }
  } else if (name == "ORC_HIT2") {
    _isLoop = false;
    if (_index == 0) {
      return degreeFactor(24);
    } else if (_index == 1) {
      return degreeFactor(12);
    } else {
      return 0;
    }
  } else if (name == "ORC_HIT3") {
    _isLoop = false;
    if (_index == 0) {
      return degreeFactor(-2);
    } else if (_index == 1) {
      return degreeFactor(-1);
    } else {
      return 0;
    }
  }
}

void ColorEnvelope::clear() {
  _timer = 0;
  _index = 0;
  _isLoop = true;
}

void ColorEnvelope::cycle(float sampleRate) {
  _envDuration = _chipOscParam->ColorDuration->get();
  _timer += 1 / sampleRate;
  if (_timer >= _envDuration) {
    _index = (_index + 1);
    _timer = 0.0f;

    // index‚ðƒ‹[ƒv‚³‚¹‚é‚©‚Ç‚¤‚©
    if (_isLoop) {
      _index = _index  % 12;
    } else {
      _index = std::min(_index, 12);
    }
  }
}

float ColorEnvelope::degreeFactor(int degree) {
  return  (pow(2.0f, (float)degree / 12.f) - 1.0f);
}