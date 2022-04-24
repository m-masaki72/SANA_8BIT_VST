#include "SimpleVoice.h"

namespace {
const float HALF_PI = MathConstants<float>::halfPi;
const float ONE_PI = MathConstants<float>::pi;
const float TWO_PI = MathConstants<float>::twoPi;
}  // namespace

SimpleVoice::SimpleVoice(
  ChipOscillatorParameters* chipOscParams, 
  SweepParameters* sweepParams,
  VibratoParameters* vibratoParams,
  VoicingParameters* voicingParams,
  OptionsParameters* optionsParams, 
  MidiEchoParameters* midiEchoParams,
  WaveformMemoryParameters* waveformMemoryParams,
  WavePatternParameters* wavePatternParams)
  : _chipOscParamsPtr(chipOscParams),
    _sweepParamsPtr(sweepParams),
    _vibratoParamsPtr(vibratoParams),
    _voicingParamsPtr(voicingParams),
    _optionsParamsPtr(optionsParams),
    _midiEchoParamsPtr(midiEchoParams),
    _waveformMemoryParamsPtr(waveformMemoryParams),
    _wavePatternParams(wavePatternParams),
    ampEnv(chipOscParams->Attack->get(), chipOscParams->Decay->get(),
            chipOscParams->Sustain->get(), chipOscParams->Release->get(),
            midiEchoParams->EchoDuration->get() * midiEchoParams->EchoRepeat->get()),
    vibratoEnv(vibratoParams->VibratoAttackTime->get(), 0.1f, 1.0f, 0.1f, 0.0f),
    portaEnv(voicingParams->StepTime->get(), 0.0f, 1.0f, 0.0f, 0.0f),
    colorEnv(chipOscParams),
    eb((std::int32_t)getSampleRate(),
        (float)midiEchoParams->EchoDuration->get(),
      midiEchoParams->EchoRepeat->get()) {
  clear();
}

bool SimpleVoice::canPlaySound(SynthesiserSound* sound) {
  return dynamic_cast<const SimpleSound*>(sound) != nullptr;
}

void SimpleVoice::startNote(int midiNoteNumber, float velocity,
                            SynthesiserSound* sound,
                            int currentPitchWheelPosition) {
  DBG("[StartNote] NoteNumber: " + juce::String(midiNoteNumber) +
      ", Velocity: " + juce::String(velocity));

  if (!canStartNote()) {
    return;
  }
  SimpleSound* soundForPlay = dynamic_cast<SimpleSound*>(sound);
  if (soundForPlay == nullptr) {
    return;
  }
  clear();

  eb.updateParam(_midiEchoParamsPtr->EchoDuration->get(),
                 _midiEchoParamsPtr->EchoRepeat->get());

  velocity = std::max(0.01f, velocity);
  level = velocity * 0.8f;

  pitchBend = ((float)currentPitchWheelPosition - 8192.0f) / 8192.0f;

  // 生成する波形のピッチを再現するサンプルデータ間の角度差⊿θ[rad]の値を決定する。
  float cyclesPerSecond = (float)MidiMessage::getMidiNoteInHertz(
      midiNoteNumber, _optionsParamsPtr->PitchStandard->get());
  float cyclesPerSample = (float)cyclesPerSecond / (float)getSampleRate();
  angleDelta = cyclesPerSample * TWO_PI;

  ampEnv.attackStart();
  vibratoEnv.attackStart();
  portaEnv.attackStart();
  colorEnv.clear();
  patternWaveClear();

  // 波形パターン初期設定
  if (_wavePatternParams->PatternEnabled->get()) {
    const auto waveType = _wavePatternParams->WaveTypes[0]->getIndex();
    *(_chipOscParamsPtr->OscWaveType) = waveType;
  }
}

/// キーリリースだとallowTailOff == true
/// キーリリース直後のボイススチールではallowTailOff == false
void SimpleVoice::stopNote(float /*velocity*/, bool allowTailOff) {
  DBG("stopNote : " + juce::String((std::int32_t)allowTailOff));
  
  if (allowTailOff) {
    portaAngleDelta = 0.0f;
    ampEnv.releaseStart();
    return;
  }
  // キーホールド中(ADSのいずれか)であればangleDeltaをリリース状態に移行
  if (ampEnv.isHolding()) {
    //ポルタメント処理，一つ前のピッチを記憶しておく
    portaAngleDelta = angleDelta;

    // NOTE: ボイススチールを受けて直ぐに音量を0にしてしまうと、急峻な変化となりノイズの発生を引き起こすため、それを予防する処理。
    ampEnv.releaseStart();
    return;
  }

  portaAngleDelta = 0.0f;

  clear();
  clearCurrentNote();
}

void SimpleVoice::pitchWheelMoved(int newPitchWheelValue) {
  pitchBend = ((float)newPitchWheelValue - 8192.0f) / 8192.0f;
}

void SimpleVoice::controllerMoved(int /*controllerNumber*/,
                                  int /*newControllerValue*/) {}

void SimpleVoice::renderNextBlock(AudioBuffer<float>& outputBuffer,
                                  int startSample, int numSamples) {
  // 現状のパラメータを取得しておく
  // NOTE: 適宜取得すると何度も取得関数が走るため重くなる
  auto currentWaveName = _chipOscParamsPtr->OscWaveType->getCurrentChoiceName();
  auto isEchoEnabled = _midiEchoParamsPtr->IsEchoEnable->get();
  auto echoRepeatCount = _midiEchoParamsPtr->EchoRepeat->get();
  auto isVibratoEnabled = _vibratoParamsPtr->VibratoEnable->get();
  auto isInVibratoDelay = 
    (_vibratoParamsPtr->VibratoAttackDeleySwitch->get() == false) &&
     (vibratoEnv.getState() == AmpEnvelope::AMPENV_STATE::ATTACK);
  auto vibratoSpeed = _vibratoParamsPtr->VibratoSpeed->get();
  auto pitchBendRange = _optionsParamsPtr->PitchBendRange->get();
  auto isPortaMode = (_voicingParamsPtr->VoicingSwitch->getCurrentChoiceName() == "PORTAMENTO");
  auto isPositiveSweepEnbaled = (_sweepParamsPtr->SweepSwitch->getCurrentChoiceName() == "Positive");
  auto isNegativeSweepEnbaled = (_sweepParamsPtr->SweepSwitch->getCurrentChoiceName() == "Negative");
  auto sweepTime = (float)_sweepParamsPtr->SweepTime->get();
  auto isPatternWaveEnabled = (float)_wavePatternParams->PatternEnabled->get();
  auto isPatternLoopEnabled = (float)_wavePatternParams->LoopEnabled->get();
  patternStepNum = (float)_wavePatternParams->StepTime->get() * getSampleRate();

  SimpleSound* playingSound =  static_cast<SimpleSound*>(getCurrentlyPlayingSound().get());
  if (playingSound == nullptr) {
    clear();
    return;
  }

  updateEnvParams(ampEnv, vibratoEnv, portaEnv);
  eb.updateParam(_midiEchoParamsPtr->EchoDuration->get(), _midiEchoParamsPtr->EchoRepeat->get());

  while (--numSamples >= 0) {
    // エンベロープにおいて，エフェクトエコーが終わっている or
    // リリース状態のとき
    if ((ampEnv.isReleaseEnded()) ||
        (isEchoEnabled && ampEnv.isEchoEnded())) {
      clearCurrentNote();
      clear();
      break;
    }

    // 現在のサンプル値を計算する
    auto currentSample = angle2wave(currentAngle, angleDelta, currentWaveName);
    currentSample *= ampEnv.getValue() * level;

    //エコー処理とエコーレンダリング
    if (isEchoEnabled) {
      eb.addSample(currentSample, _midiEchoParamsPtr->VolumeOffset->get() / 100.0f);
      eb.cycle();

      for (auto channelNum = outputBuffer.getNumChannels(); --channelNum >= 0;) {
        for (auto i = 0; i < echoRepeatCount; ++i) {
          outputBuffer.addSample(channelNum, startSample, eb.getSample(i));
        }
      }
    }

    // バッファ書き込み
    for (auto channelNum = outputBuffer.getNumChannels(); --channelNum >= 0;) {
      outputBuffer.addSample(channelNum, startSample, currentSample);
    }
    ++startSample;

    //NOTE: 以降はサイクル更新処理を行う

    // Vibratoのモジュレーション影響度計算
    float modulationFactor = 0.0f;
    if (!(isVibratoEnabled) || isInVibratoDelay) {
      modulationFactor = 0.0f;
    } else {
      modulationFactor = calcModulationFactor(vibratoAngle) *  vibratoEnv.getValue();
    }

    //ピッチ処理
    const auto pitchBendFactor = pow(2.0f, pitchBend / 13.0f * pitchBendRange);
    const auto pitchModulationFactor = pow(2.0f, modulationFactor / 13.0f);
    const auto sweepFactor = pow(2.0f, pitchSweep);
    const auto colorFactor = colorEnv.getManipulateAngle() + 1;
    currentAngle += angleDelta * pitchBendFactor * pitchModulationFactor * sweepFactor * colorFactor;

    // ポルタメントをcurrentAngleに反映
    if (isPortaMode) {
      if (portaAngleDelta > 0.0f) {
        currentAngle -= (angleDelta - portaAngleDelta) * (1 - portaEnv.getValue());
      }
    }

    // ビブラート更新
    vibratoAngle += ((vibratoSpeed) / (float)getSampleRate()) * TWO_PI;

    // スイープ更新
    if (isPositiveSweepEnbaled) {
      pitchSweep += 1 / (float)getSampleRate() / sweepTime;
      pitchSweep = std::min(10.0f, pitchSweep);
    } else if (isNegativeSweepEnbaled) {
      pitchSweep -= 1 / (float)getSampleRate() / sweepTime;
      pitchSweep = std::max(0.0f, pitchSweep);
    }

    // パターンエンベロープ
    if (isPatternWaveEnabled) {
      patternCounter++;

      // パターン更新処理
      if (patternCounter >= patternStepNum) {
        patternCounter = 0;

        patternIndex++;
        if (patternIndex >= WAVEPATTERN_LENGTH) {
          if (isPatternLoopEnabled) {
            patternIndex = 0;
          } else {
            patternIndex = WAVEPATTERN_LENGTH - 1;
          }
        }
        const auto nextIndex = (WAVEPATTERN_TYPES - 1) - _wavePatternParams->WavePatternArray[patternIndex]->get();
        const auto waveType = _wavePatternParams->WaveTypes[nextIndex]->getIndex();
        *(_chipOscParamsPtr->OscWaveType) = waveType;
      }
    }

    // 周期切り捨て
    currentAngle = fmod(currentAngle, TWO_PI);
    vibratoAngle = fmod(vibratoAngle, TWO_PI);

    // エンベロープパラメータを更新して時間分進める
    ampEnv.cycle((float)getSampleRate());
    vibratoEnv.cycle((float)getSampleRate());
    portaEnv.cycle((float)getSampleRate());
    colorEnv.cycle((float)getSampleRate());
  }
}

void SimpleVoice::clear() {
  currentAngle = 0.0f;
  vibratoAngle = 0.0f;
  angleDelta = 0.0f;
  // portaAngleDelta = 0.0f; // portaはリセットしない
  level = 0.0f;
  pitchBend = 0.0f;
  pitchSweep = 0.0f;
  patternWaveClear();
  waveForms.init();
}

void SimpleVoice::patternWaveClear() {
  patternCounter = 0;
  patternIndex = 0;
  patternStepNum = 0.0f;
}

float SimpleVoice::calcModulationFactor(float angle) {
  float factor = waveForms.sine(angle);

  // factorの値が0.5を中心とした0.0～1.0の値となるように調整する。
  factor *= _vibratoParamsPtr->VibratoAmount->get();
  return factor;
}

float SimpleVoice::angle2wave(float angle, float angleDelta, const juce::String& waveName) {
  auto value = 0.0f;
  if (waveName == "NES_Square50%") {
    value = waveForms.nesSquare(angle);
  } else if (waveName == "NES_Square25%") {
    value = waveForms.nesSquare25(angle);
  } else if (waveName == "NES_Square12.5%") {
    value = waveForms.nesSquare125(angle);
  } else if (waveName == "NES_Triangle") {
    value = waveForms.nesTriangle(angle);
  } else if (waveName == "NES_LongNoise") {
    value = waveForms.longNoise(angleDelta);
  } else if (waveName == "NES_ShortNoise") {
    value = waveForms.shortNoise(angleDelta);
  } else if (waveName == "Pure_Sine") {
    value = waveForms.sine(angle);
  } else if (waveName == "Rough_Sine") {
    value = waveForms.roughSine(angle);
  } else if (waveName == "Pure_Saw") {
    value = waveForms.saw(angle);
  } else if (waveName == "Rough_Saw") {
    value = waveForms.roughSaw(angle);
  } else if (waveName == "Pure_Triangle") {
    value = waveForms.triangle(angle);
  } else if (waveName == "Pure_Square50%") {
    value = waveForms.square(angle);
  } else if (waveName == "Pure_Square25%") {
    value = waveForms.square25(angle);
  } else if (waveName == "Pure_Square12.5%") {
    value = waveForms.square125(angle);
  } else if (waveName == "Pure_Noise") {
    value = waveForms.noise(angleDelta);
  } else if (waveName == "Rough_Noise") {
    value = waveForms.lobitNoise(angleDelta);
  } else if (waveName == "Waveform Memory") {
    value = waveForms.waveformMemory(angle, _waveformMemoryParamsPtr);
  }
  return value;
}

bool SimpleVoice::canStartNote() {
  if (ampEnv.isReleasing() || ampEnv.isReleaseEnded() || ampEnv.isEchoEnded()) {
    return true;
  }

  return false;
}

void SimpleVoice::updateEnvParams(AmpEnvelope& ampEnv, AmpEnvelope& vibratoEnv, AmpEnvelope& portaEnv) {
  ampEnv.setParameters(_chipOscParamsPtr->Attack->get(),
                      _chipOscParamsPtr->Decay->get(),
                      _chipOscParamsPtr->Sustain->get(),
                      _chipOscParamsPtr->Release->get(),
                      _midiEchoParamsPtr->EchoDuration->get() *
                          _midiEchoParamsPtr->EchoRepeat->get());
  vibratoEnv.setParameters(_vibratoParamsPtr->VibratoAttackTime->get(), 0.1f, 1.0f, 0.1f, 0.0f);
  portaEnv.setParameters(_voicingParamsPtr->StepTime->get(), 0.0f, 1.0f, 0.0f, 0.0f);
}