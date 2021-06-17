#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "BaseAudioProcessor.h"
#include "DSP/DspUtils.h"
#include "DSP/SynthParameters.h"
#include "GUI/ScopeComponent.hpp"

class PluginProcessor : public BaseAudioProcessor {
 public:
  PluginProcessor ();
  ~PluginProcessor ();
  
  AudioProcessorEditor* createEditor() override;

  // プリセット用関数
  int getNumPrograms() override;
  int getCurrentProgram() override;
  void setCurrentProgram(int index) override;
  const String getProgramName(int index) override;

  void prepareToPlay(double sampleRate, std::int32_t samplesPerBlock) override;
  void processBlock(AudioBuffer<float>&, MidiBuffer&) override;
  
  void getStateInformation(MemoryBlock& destData) override;
  void setStateInformation(const void* data, int sizeInBytes) override;
  
  MidiKeyboardState& getKeyboardState() { return keyboardState; }
  AudioBufferQueue<float>& getAudioBufferQueue() { return scopeDataQueue; }

  const StringArray OSC_WAVE_TYPES{
      "NES_Square50%",    "NES_Square25%",   "NES_Square12.5%",
      "NES_Triangle",     "Pure_Square50%",  "Pure_Square25%",
      "Pure_Square12.5%", "Pure_Triangle",   "Pure_Sine",
      "Pure_Saw",         "NES_LongNoise",   "NES_ShortNoise",
      "Pure_Lo-bitNoise", "Waveform Memory",
  };
  const StringArray SWEEP_SWITCH{"OFF", "Positive", "Negative"};
  const StringArray VOICING_SWITCH{"POLY", "MONO", "PORTAMENTO"};

  PresetsParameters presetsParameters;
  ChipOscillatorParameters chipOscParameters;
  SweepParameters sweepParameters;
  VibratoParameters vibratoParameters;
  VoicingParameters voicingParameters;
  OptionsParameters optionsParameters;
  WaveformMemoryParameters waveformMemoryParameters;
  MidiEchoParameters midiEchoParameters;
  FilterParameters filterParameters;

 private:
  void initProgram();
  std::int32_t getNumVoices();
  void addVoice();
  void changeVoiceSize();
  void clearBuffers(AudioBuffer<float>& buffer, AudioBuffer<float>& upSampleBuffer);
  static float clippingFunction(float inputValue);
  void initEffecters(dsp::ProcessSpec& spec);
  void procMidiMessages(const AudioBuffer<float>& buffer, const MidiBuffer& midiMessages);

  Synthesiser synth;

  // preset index
  std::int32_t currentProgIndex;

  //アンチエイリアスフィルタ用
  antiAliasFilter antiAliasFilter;

  // DSPエフェクト，クリッパー，ドライブ，フィルタ
  dsp::WaveShaper<float> clipper;
  dsp::Gain<float> drive;
  dsp::ProcessorDuplicator<dsp::IIR::Filter<float>,
                           dsp::IIR::Coefficients<float>>
      hicutFilter;
  dsp::ProcessorDuplicator<dsp::IIR::Filter<float>,
                           dsp::IIR::Coefficients<float>>
      lowcutFilter;

  // GUI上のキーボードコンポーネントで生成されたMIDI情報を保持しておくオブジェクト.
  // MIDIキーボードの状態を同期するためのステートオブジェクト
  MidiKeyboardState keyboardState;

  // スコープパネルに波形を表示するためのデータバッファ
  AudioBufferQueue<float> scopeDataQueue;
  ScopeDataCollector<float> scopeDataCollector;

  MidiBuffer eventsToAdd;
  
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginProcessor )
};
