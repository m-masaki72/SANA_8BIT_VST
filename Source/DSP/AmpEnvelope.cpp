/*
  ==============================================================================

    AmpEnvelope.cpp
    Created: 20 Apr 2018 2:08:26am
  Modified: 14 September 2018
  Author:  MasakiMori, COx2

  ==============================================================================
*/

// ヘッダファイルをインクルードする。
#include "AmpEnvelope.h"

namespace {
const float AMP_MAX = 1.0f;
const float AMP_MIN = 0.0f;
const float ATTACK_MIN = 0.001f;
const float DECAY_MIN = 0.001f;
const float RELEASE_MIN = 0.001f;
}  // namespace

AmpEnvelope::AmpEnvelope(float attackTime, float decayTime, float sustain,
                         float releaseTime, float echoTime)
    : _attackTime(attackTime),
      _decayTime(decayTime),
      _sustainValue(sustain),
      _releaseTime(releaseTime),
      _echoTime(echoTime),
      _sampleRate(0.0f),
      _value(0.0f),
      _valueOnReleaseStart(0.0f),
      _timer(0.0f),
      _ampState(AMPENV_STATE::WAIT) {
  checkVarRange();
}

AmpEnvelope::~AmpEnvelope() {}

AmpEnvelope::AMPENV_STATE AmpEnvelope::getState() { return _ampState; }

float AmpEnvelope::getValue() { return _value; }

void AmpEnvelope::setParameters(float attackTime, float decayTime,
                                float sustain, float releaseTime,
                                float echoTime) {
  _attackTime = attackTime;
  _decayTime = decayTime;
  _sustainValue = sustain;
  _releaseTime = releaseTime;
  _echoTime = echoTime;

  checkVarRange();
}

// 各パラメータの値を定数で記述した最大値・最小値の範囲に収める。
void AmpEnvelope::checkVarRange() {
  if (_attackTime <= ATTACK_MIN) {
    _attackTime = ATTACK_MIN;
  }
  if (_decayTime <= DECAY_MIN) {
    _decayTime = DECAY_MIN;
  }
  if (_releaseTime <= RELEASE_MIN) {
    _releaseTime = RELEASE_MIN;
  }
  if (_sustainValue > AMP_MAX) {
    _sustainValue = AMP_MAX;
  }
  if (_sustainValue < AMP_MIN) {
    _sustainValue = AMP_MIN;
  }
}

void AmpEnvelope::attackStart() {
  // エンベロープがRelease状態でなければ、エンベロープの値を最小値(0.0)にしてAttack状態時の計算処理に備える。
  if (!isReleasing()) {
    _value = AMP_MIN;
    _timer = 0.0f;
  }
  _ampState = AMPENV_STATE::ATTACK;
}

void AmpEnvelope::releaseStart() {
  // エンベロープの状態がAttack, Decay,
  // Sustainのいずれかであれば、Release状態に移行する。
  if (isHolding()) {
    _ampState = AMPENV_STATE::RELEASE;
    _valueOnReleaseStart = _value;
    _timer = 0.0f;
  }
}

void AmpEnvelope::releaseEnd() {
  _value = AMP_MIN;
  _timer = 0.0f;
  _ampState = AMPENV_STATE::WAIT;
}

// エンベロープの状態がAttack, Decay, Sustainのいずれかに該当するかを返す関数。
bool AmpEnvelope::isHolding() {
  if (_ampState == AmpEnvelope::AMPENV_STATE::ATTACK ||
      _ampState == AmpEnvelope::AMPENV_STATE::DECAY ||
      _ampState == AmpEnvelope::AMPENV_STATE::SUSTAIN) {
    return true;
  }
  return false;
}

bool AmpEnvelope::isReleasing() {
  return _ampState == AmpEnvelope::AMPENV_STATE::RELEASE;
}

bool AmpEnvelope::isReleaseEnded() {
  return _ampState == AmpEnvelope::AMPENV_STATE::WAIT;
}

bool AmpEnvelope::isEchoEnded() {
  return _ampState == AmpEnvelope::AMPENV_STATE::ECHO;
}

// エンベロープの計算処理を1サンプル分進める。この計算処理を実行することで変数の値が更新される。
void AmpEnvelope::cycle(float sampleRate) {
  _sampleRate = sampleRate;
  switch (_ampState) {
    // Attack状態時の更新処理
    case AMPENV_STATE::ATTACK:
      _value = _timer * (2.0f - _timer);
      _timer += 1.0f / (_sampleRate * _attackTime);
      if (_timer >= 1.0f) {
        _value = AMP_MAX;
        _timer = 0.0f;
        _ampState = AMPENV_STATE::DECAY;
      }
      break;
    // Decay状態時の更新処理
    case AMPENV_STATE::DECAY:
      _value = (1.0f - _sustainValue) * (_timer - 1.0f) * (_timer - 1.0f) +
               _sustainValue;
      _timer += 1.0f / (_sampleRate * _decayTime);
      if (_timer >= 1.0f) {
        _value = _sustainValue;
        _timer = 0.0f;
        _ampState = AMPENV_STATE::SUSTAIN;
      }
      break;
    // Sustain状態時の更新処理
    case AMPENV_STATE::SUSTAIN:
      _value = _sustainValue;
      _timer = 0.0f;
      break;
    // Release状態時の更新処理
    case AMPENV_STATE::RELEASE:
      _value = (_valueOnReleaseStart) * (_timer - 1.0f) * (_timer - 1.0f);
      _timer += 1.0f / (_sampleRate * (_releaseTime));
      if (_timer >= 1.0f) {
        _value = AMP_MIN;
        _timer = 0.0f;
        _ampState = AMPENV_STATE::WAIT;
      }
      break;
    // Wait状態時の更新処理
    case AMPENV_STATE::WAIT:
      _value = AMP_MIN;
      _timer += 1.0f / (_sampleRate * _echoTime);
      if (_timer >= 1.0f) {
        _ampState = AMPENV_STATE::ECHO;
      }
      break;
  }
}