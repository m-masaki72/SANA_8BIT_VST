#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "AmpEnvelope.h"
#include "MIDIEcho.h"
#include "SimpleSound.h"
#include "Waveforms.h"
#include <set>

class SimpleVoice : public SynthesiserVoice {
 public:
  SimpleVoice(ChipOscillatorParameters* chipOscParams,
              SweepParameters* sweepParams, VibratoParameters* vibratoParams,
              VoicingParameters* voicingParams,
              OptionsParameters* optionsParams,
              MidiEchoParameters* midiEchoParams,
              WaveformMemoryParameters* waveformMemoryParams,
              std::set<int>* midiList);

  virtual ~SimpleVoice() = default;

  virtual bool canPlaySound(SynthesiserSound* sound) override;
  virtual void startNote(int midiNoteNumber, float velocity,
                         SynthesiserSound* sound,
                         int currentPitchWheelPosition) override;
  virtual void stopNote(float velocity, bool allowTailOff) override;
  virtual void pitchWheelMoved(int newPitchWheelValue) override;
  virtual void controllerMoved(int controllerNumber,
                               int newControllerValue) override;
  virtual void renderNextBlock(AudioBuffer<float>& outputBuffer,
                               int startSample, int numSamples) override;

 private:
  float calcModulationFactor(float angle);
  float angle2wave(float currentAngle);
  bool isArpMode();
  bool canStartNote();

  float currentAngle, vibratoAngle, angleDelta, portaAngleDelta;
  float level;
  float pitchBend, pitchSweep;
  std::vector<float> echoSamples;

  EchoBuffer eb;

  // Waveform用のパラメータ
  Waveforms waveForms;
  //各種エンベロープ， アンプ， ビブラート， ポルタメント用
  AmpEnvelope ampEnv, vibratoEnv, portaEnv;

  // パラメータを管理するオブジェクトのポインタ変数。
  ChipOscillatorParameters* _chipOscParamsPtr;
  SweepParameters* _sweepParamsPtr;
  VibratoParameters* _vibratoParamsPtr;
  VoicingParameters* _voicingParamsPtr;
  OptionsParameters* _optionsParamsPtr;
  MidiEchoParameters* _midiEchoParamsPtr;
  WaveformMemoryParameters* _waveformMemoryParamsPtr;
  std::set<int>* _midiList;
};