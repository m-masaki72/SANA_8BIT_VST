#include "Waveforms.h"

namespace {
const float HALF_PI = MathConstants<float>::halfPi;
const float ONE_PI = MathConstants<float>::pi;
const float TWO_PI = MathConstants<float>::twoPi;
const float PITCH_SHIFT = (2 << 3);
}  // namespace

Waveforms::Waveforms() { init(); }

void Waveforms::init() {
  _rand = Random(0);
  _noiseVal = 0.0f;
  _freqCounter = 0;
}

float Waveforms::sine(float angle) {
  checkAngleRanage(angle);

  return sinf(angle);
}

float Waveforms::roughSine(float angle) {
  return quantize(sine(angle), 4);
}


float Waveforms::saw(float angle) {
  checkAngleRanage(angle);

  if (angle <= ONE_PI) {
    return (angle / ONE_PI);
  } else {
    return -2.0f + (angle / ONE_PI);
  }
}

float Waveforms::roughSaw(float angle) {
  return quantize(saw(angle), 2);
}

float Waveforms::square(float angle) {
  checkAngleRanage(angle);

  if (angle > TWO_PI) {
    angle = fmodf(angle, TWO_PI);
  }

  if (angle <= ONE_PI) {
    return 1.0f;
  } else {
    return -1.0f;
  }
}

float Waveforms::square25(float angle) {
  checkAngleRanage(angle);

  if (angle <= HALF_PI) {
    return 1.0f;
  } else {
    return -1.0f;
  }
}

float Waveforms::square125(float angle) {
  checkAngleRanage(angle);

  if (angle <= HALF_PI / 2) {
    return 1.0f;
  } else {
    return -1.0f;
  }
}

float Waveforms::triangle(float angle) {
  checkAngleRanage(angle);

  if (angle <= HALF_PI) {
    return (angle / HALF_PI);
  } else if (angle > HALF_PI && angle <= (ONE_PI + HALF_PI)) {
    return 2.0f - (2.0f * angle / ONE_PI);
  } else {
    return -4.0f + (angle / HALF_PI);
  }
}

// NESの長周期ノイズの再現
float Waveforms::longNoise(const float angleDelta) {
  if (++_freqCounter > TWO_PI / angleDelta / PITCH_SHIFT) {
    _freqCounter = 0;
    std::uint16_t result =
        ((_longNoizeReg & (0x0002)) >> 1) ^ ((_longNoizeReg & (0x0004)) >> 2);
    result = result << 15;
    _longNoizeReg = (_longNoizeReg >> 1) | result;
    _noiseVal = (_longNoizeReg & 0x0001) * 2.0f - 1.0f;
  }
  return _noiseVal;
}

// NESの短周期ノイズの再現
float Waveforms::shortNoise(const float angleDelta) {
  if (++_freqCounter > TWO_PI / angleDelta / PITCH_SHIFT) {
    _freqCounter = 0;
    std::uint16_t result =
        ((_shortNoizeReg & (0x0002)) >> 1) ^ ((_shortNoizeReg & (0x00B0)) >> 7);
    result = result << 15;
    _shortNoizeReg = (_shortNoizeReg >> 1) | result;
    _noiseVal = (_shortNoizeReg & 0x0001) * 2.0f - 1.0f;
  }
  return _noiseVal;
}

float Waveforms::noise(const float angleDelta) {
  if (++_freqCounter > TWO_PI / angleDelta / PITCH_SHIFT) {
    _freqCounter = 0;
    _noiseVal = _rand.nextFloat() * 2.0f - 1.0f;
  }
  return _noiseVal;
}

//乱数を時間軸と振幅軸側でクオンタイズしたもの
float Waveforms::lobitNoise(const float angleDelta) {
  return quantize(noise(angleDelta), 2);
}

float Waveforms::nesSquare(float angle) {
  checkAngleRanage(angle);

  if (angle <= ONE_PI) {
    return high_pass(1.0f);
  } else {
    return high_pass(-1.0f);
  }
}

float Waveforms::nesSquare25(float angle) {
  checkAngleRanage(angle);

  if (angle <= HALF_PI) {
        return high_pass(1.0f);
  } else {
    return high_pass(-1.0f);
  }
}

float Waveforms::nesSquare125(float angle) {
  checkAngleRanage(angle);

  if (angle <= HALF_PI / 2) {
    return high_pass(1.0f);
  } else {
    return high_pass(-1.0f);
  }
}

float Waveforms::nesTriangle(float angle) {
  checkAngleRanage(angle);

  if (angle < ONE_PI) {
    std::int32_t value = std::int32_t(angle / ONE_PI * 16) * 2;
    return (-1.0f + value / 15.0f);
  } else {
    std::int32_t value = std::int32_t((angle - ONE_PI) / ONE_PI * 16) * 2;
    return (1.0f - value / 15.0f);
  }
}

float Waveforms::waveformMemory(
    float angle, WaveformMemoryParameters* _waveformMemoryParamsPtr) {
  checkAngleRanage(angle);

  // valの範囲を変換 0~15 -> -1.0~1.0
  auto index = (std::int32_t)(angle * WAVESAMPLE_LENGTH / TWO_PI);
  std::int32_t val = *_waveformMemoryParamsPtr->WaveSamplesArray[index];
  return val / 8.0f - 1.0f;
}

// 4bitクオンタイズ関数 qNum * 2倍の数でクオンタイズする
float Waveforms::quantize(float sample, int qNum) {
  return round(sample * qNum) / qNum;
}

// 角度（ラジアン）の値が2πを超えている場合は、変数angleの値が0～2πの範囲内に収まるよう剰余を求める。
void Waveforms::checkAngleRanage(float &angle) {
  if (angle > TWO_PI) {
    angle = fmodf(angle, TWO_PI);
  }
}

float Waveforms::high_pass(float in) {
  auto out = in - _capacitor;
  _capacitor = in - out * 0.996;
  return out;
}