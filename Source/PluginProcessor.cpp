#include "PluginProcessor.h"

#include "DSP/SimpleSound.h"
#include "DSP/SimpleVoice.h"
#include "EditorGUI.h"

namespace {
const float MIN_DELTA = 0.0001f;
}

// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
  return new PluginProcessor();
}

PluginProcessor ::PluginProcessor ()
    : BaseAudioProcessor(),
      chipOscParameters(
        new AudioParameterChoice("OSC_WAVE_TYPE", "Osc-WaveType", OSC_WAVE_TYPES, 0),
        new AudioParameterFloat("VOLUME", "Volume", -32.0f, 8.0f, -20.0f),
        new AudioParameterFloat("AMPENV_ATTACK", "Attack", {0.000f, 10.0f, MIN_DELTA}, 0.000f),
        new AudioParameterFloat("AMPENV_DECAY", "Decay", {0.000f, 10.0f, MIN_DELTA}, 0.000f),
        new AudioParameterFloat("AMPENV_SUSTAIN", "Sustain", {0.000f, 1.0f, MIN_DELTA}, 1.0f),
        new AudioParameterFloat("AMPENV_RELEASE", "Release",  {0.000f, 10.0f, MIN_DELTA}, 0.000f),
        new AudioParameterChoice("OSC_COLOR_TYPE", "Osc-ColorType", OSC_COLOR_TYPES, 0),
        new AudioParameterFloat("COLOR_DURATION", "Color-Duration", {0.001f, 0.5f, MIN_DELTA}, 0.1f)),
      sweepParameters(
        new AudioParameterChoice("SWEEP_SWITCH", "Sweep-Switch", SWEEP_SWITCH, 0),
        new AudioParameterFloat("SWEEP_TIME", "Sweep-Time", 0.01f, 10.0f, 1.0f)),
      vibratoParameters(
        new AudioParameterBool("VIBRATO_ENABLE", "Vibrato-Enable", false),
        new AudioParameterBool("VIBRATO_ATTACK-DELEY-SWITCH", "Vibrato-Attack-Deley-Switch", true),
        new AudioParameterFloat("VIBRATO_DEPTH", "Vibrato-Depth", {0.0f, 12.0f, MIN_DELTA}, 0.5f),
        new AudioParameterFloat("VIBRATO_SPEED", "Vibrato-Speed", {0.0f, 20.0f, MIN_DELTA}, 4.f),
        new AudioParameterFloat("VIBRATO_ATTACKTIME", "Vibrato-AttackTime", {0.0f, 15.0f, MIN_DELTA}, 0.0f)),
      voicingParameters(
        new AudioParameterChoice("VOICING_TYPE", "Voicing-Type", VOICING_SWITCH, 0),
        new AudioParameterFloat("STEP_TIME", "Step-Time", {0.0f, 3.0f, MIN_DELTA}, 0.5f)),
      optionsParameters(
        new AudioParameterInt("PITCH_BEND_RANGE", "Pitch-Bend-Range", 1, 13, 2),
        new AudioParameterInt("PITCH_STANDARD", "Pitch-Standard", 400, 500, 440)),
      waveformMemoryParameters(),
      midiEchoParameters(
        new AudioParameterBool("ECHO_ENABLE", "Echo-Enable", false),
        new AudioParameterFloat("ECHO_DURATION", "Echo-Duration", {0.01f, 3.0f, MIN_DELTA}, 0.1f),
        new AudioParameterInt("ECHO_REPEAT", "Echo-Repeat", 1, 5, 1),
        new AudioParameterFloat("ECHO_VOLUMEOFFSET", "Echo-VolumeOffset", 0.0f, 200.0f, 50.0f)),
      filterParameters(
        new AudioParameterBool("HICUT_ENABLE", "Filter-Hicut-Enable", false),
        new AudioParameterBool("LOWCUT_ENABLE", "Filter-Lowcut-Enable", false),
        new AudioParameterFloat("FILTER_HICUT-FREQ", "Filter-Hicut-Freq", 40.0f, 20000.0f, 20000.0f),
        new AudioParameterFloat("FILTER_LOWCUT-FREQ", "Filter-Lowcut-Freq", 40.0f, 20000.0f, 40.0f)),
      presetsParameters(),
      wavePatternParameters(),
      scopeDataCollector(scopeDataQueue) {
  presetsParameters.addAllParameters(*this);
  chipOscParameters.addAllParameters(*this);
  sweepParameters.addAllParameters(*this);
  vibratoParameters.addAllParameters(*this);
  voicingParameters.addAllParameters(*this);
  optionsParameters.addAllParameters(*this);
  midiEchoParameters.addAllParameters(*this);
  waveformMemoryParameters.addAllParameters(*this);
  filterParameters.addAllParameters(*this);
  wavePatternParameters.addAllParameters(*this);
}

PluginProcessor ::~PluginProcessor () {}

int PluginProcessor::getNumPrograms() { return NUM_OF_PRESETS; }

int PluginProcessor::getCurrentProgram() {
  currentProgIndex = presetsParameters.ProgramIndex->get();
  return presetsParameters.ProgramIndex->get();
}

void PluginProcessor::setCurrentProgram(int index) {
  if (currentProgIndex != index) {
    initProgram();

    switch (index) {
      case 0:
        break;
      case 1:
        *chipOscParameters.OscWaveType = 2;
        *chipOscParameters.Decay = 5.000f;
        *chipOscParameters.Sustain = 0.0f;
        *vibratoParameters.VibratoAmount = 0.05f;
        *vibratoParameters.VibratoSpeed = 13.0f;
        break;
      case 2:
        *chipOscParameters.Attack = 1.000f;
        *vibratoParameters.VibratoAmount = 0.05f;
        *vibratoParameters.VibratoSpeed = 5.0f;
        break;
      case 3:
        *chipOscParameters.Decay = 0.500f;
        *chipOscParameters.Sustain = 0.0f;
        break;
      case 4:
        *chipOscParameters.OscWaveType = 1;
        *chipOscParameters.Decay = 0.050f;
        *chipOscParameters.Sustain = 0.60f;
        *vibratoParameters.VibratoAmount = 0.05f;
        *vibratoParameters.VibratoSpeed = 6.0f;
        break;
      case 5:
        *vibratoParameters.VibratoAmount = 1.0f;
        *vibratoParameters.VibratoSpeed = 6.0f;
        *vibratoParameters.VibratoAttackTime = 3.0f;
        break;
      case 6:
        *chipOscParameters.Decay = 0.300f;
        *chipOscParameters.Sustain = 0.5f;
        break;
      case 7:
        *sweepParameters.SweepSwitch = 1;
        *sweepParameters.SweepTime = 3.0f;
        break;
      case 8:
        *chipOscParameters.OscWaveType = 10;
        *chipOscParameters.Decay = 0.200f;
        *chipOscParameters.Sustain = 0.0f;
        break;
      case 9:
        *chipOscParameters.OscWaveType = 10;
        *chipOscParameters.Decay = 0.800f;
        *chipOscParameters.Sustain = 0.0f;
        break;
      case 10:
        *chipOscParameters.OscWaveType = 10;
        *chipOscParameters.Attack = 0.100f;
        *chipOscParameters.Decay = 2.500f;
        *chipOscParameters.Sustain = 0.0f;
        break;
      case 11:
        *chipOscParameters.Decay = 0.200f;
        *chipOscParameters.Sustain = 0.0f;
        *sweepParameters.SweepSwitch = 2;
        *sweepParameters.SweepTime = 0.4f;
        break;
      default:
        break;
    }
    *presetsParameters.ProgramIndex = currentProgIndex = index;
  }
}

const String PluginProcessor::getProgramName(int index) {
  switch (index) {
    case 0:
      return ("Initial Program");
    case 1:
      return ("Voice Lead");
    case 2:
      return ("String");
    case 3:
      return ("Mallet");
    case 4:
      return ("Brass");
    case 5:
      return ("Modulated Lead");
    case 6:
      return ("Backing Lead");
    case 7:
      return ("Riser Lead");
    case 8:
      return ("Noise Hihat");
    case 9:
      return ("Noise Snare");
    case 10:
      return ("Noise Cymbal");
    case 11:
      return ("Pulse Bass&Tom");
    default:
      return ("");
  }
}

void PluginProcessor::prepareToPlay(double sampleRate, int32_t samplesPerBlock) {
  synth.clearSounds();
  synth.clearVoices();
  synth.setCurrentPlaybackSampleRate(sampleRate * UP_SAMPLING_FACTOR);

  // サウンド再生可能なノート番号の範囲を定義する。関数"setRange"
  // にて0～127の値をtrueに設定する。
  BigInteger canPlayNotes;
  canPlayNotes.setRange(0, 127, true);
  // サウンド再生可能なチャンネル番号の範囲を定義する。関数"setRange"
  // にて0～127の値をtrueに設定する。
  BigInteger canPlayChannels;
  canPlayChannels.setRange(1, 2, true);
  synth.addSound(new SimpleSound(canPlayNotes, canPlayChannels));

  for (auto i = 0, voiceNum = getNumVoices(); i < voiceNum; ++i) {
    addVoice();
  }

  dsp::ProcessSpec spec = dsp::ProcessSpec();
  spec.sampleRate = sampleRate;
  spec.numChannels = getTotalNumOutputChannels();
  spec.maximumBlockSize = samplesPerBlock;
  initEffecters(spec);

  antiAliasFilter.prepare((int32_t)sampleRate, UP_SAMPLING_FACTOR);
}

void PluginProcessor::processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages) {
  // 高サンプルのバッファを用意しておく  
  AudioBuffer<float> upSampleBuffer(buffer.getNumChannels(), buffer.getNumSamples() * UP_SAMPLING_FACTOR);
  clearBuffers(buffer, upSampleBuffer);

  // MIDIキーボードUI情報の更新
  keyboardState.processNextMidiBuffer(midiMessages, 0, buffer.getNumSamples(),true);

  if (getNumVoices() != synth.getNumVoices()) {
    changeVoiceSize();
  }

  procMidiMessages(buffer, midiMessages);

  // 波形生成
  synth.renderNextBlock(upSampleBuffer, midiMessages, 0, upSampleBuffer.getNumSamples());

  // アンチエイリアス
  antiAliasFilter.process(buffer, upSampleBuffer, getTotalNumInputChannels(), getTotalNumOutputChannels());

  // エフェクトセクション
  dsp::AudioBlock<float> audioBlock(buffer);
  dsp::ProcessContextReplacing<float> context(audioBlock);

  // ゲインを上げる
  drive.setGainDecibels(chipOscParameters.VolumeLevel->get());
  drive.process(context);

  // フィルタ処理
  {
    if (filterParameters.HicutEnable->get()) {
      *hicutFilter.state = *dsp::IIR::Coefficients<float>::makeLowPass(
          getSampleRate(), filterParameters.HicutFreq->get());
      hicutFilter.process(context);
    }
    if (filterParameters.LowcutEnable->get()) {
      *lowcutFilter.state = *dsp::IIR::Coefficients<float>::makeHighPass(
          getSampleRate(), filterParameters.LowcutFreq->get());
      lowcutFilter.process(context);
    }
  }
  // クリッピング処理
  clipper.process(context);

  // ⑧現時点でオーディオバッファで保持しているサンプルデータをScopeDataCollectorクラスのオブジェクトに渡す。
  scopeDataCollector.process(buffer.getReadPointer(0),
                             (size_t)buffer.getNumSamples());
}

AudioProcessorEditor* PluginProcessor::createEditor() {
  return new EditorGUI(*this);
}


void PluginProcessor::getStateInformation(MemoryBlock& destData) {
  // You should use this method to store your parameters in the memory block.
  // You could do that either as raw data, or use the XML or ValueTree classes
  // as intermediaries to make it easy to save and load complex data.

  std::unique_ptr<XmlElement> xml(new XmlElement("SimpleSynthParameters"));

  presetsParameters.saveParameters(*xml);
  chipOscParameters.saveParameters(*xml);
  sweepParameters.saveParameters(*xml);
  vibratoParameters.saveParameters(*xml);
  voicingParameters.saveParameters(*xml);
  optionsParameters.saveParameters(*xml);
  midiEchoParameters.saveParameters(*xml);
  waveformMemoryParameters.saveParameters(*xml);
  filterParameters.saveParameters(*xml);
  wavePatternParameters.saveParameters(*xml);

  copyXmlToBinary(*xml, destData);
}

void PluginProcessor::setStateInformation(const void* data,
                                                    int sizeInBytes) {
  // You should use this method to restore your parameters from this memory
  // block, whose contents will have been created by the getStateInformation()
  // call.

  std::unique_ptr<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

  if (xmlState.get() != nullptr) {
    if (xmlState->hasTagName("SimpleSynthParameters")) {
      presetsParameters.loadParameters(*xmlState);
      chipOscParameters.loadParameters(*xmlState);
      sweepParameters.loadParameters(*xmlState);
      vibratoParameters.loadParameters(*xmlState);
      voicingParameters.loadParameters(*xmlState);
      optionsParameters.loadParameters(*xmlState);
      midiEchoParameters.loadParameters(*xmlState);
      waveformMemoryParameters.loadParameters(*xmlState);
      filterParameters.loadParameters(*xmlState);
      wavePatternParameters.loadParameters(*xmlState);
    }

    // Preset用のパラメータを更新（変数をローカルintで保存しないとシンセ終了時にフリーズする
    currentProgIndex = presetsParameters.ProgramIndex->get();
  }
}

void PluginProcessor::initProgram() {
  *chipOscParameters.OscWaveType = 0;
  *chipOscParameters.VolumeLevel = -20.0f;
  *chipOscParameters.Attack = 0.000f;
  *chipOscParameters.Decay = 0.000f;
  *chipOscParameters.Sustain = 1.0f;
  *chipOscParameters.Release = 0.000f;

  *sweepParameters.SweepSwitch = 0;
  *sweepParameters.SweepTime = 1.0f;

  *vibratoParameters.VibratoEnable = true;
  *vibratoParameters.VibratoAmount = 0.0f;
  *vibratoParameters.VibratoSpeed = 0.1000f;
  *vibratoParameters.VibratoAttackTime = 0.0f;
}

std::int32_t PluginProcessor::getNumVoices() {
  if (voicingParameters.VoicingSwitch->getCurrentChoiceName() == "POLY") {
    return VOICE_MAX;
  } else {
    return 1;
  }
}

void PluginProcessor::addVoice() {
  synth.addVoice(new SimpleVoice(&chipOscParameters, &sweepParameters,
                                 &vibratoParameters, &voicingParameters,
                                 &optionsParameters, &midiEchoParameters,
                                 &waveformMemoryParameters, &wavePatternParameters));
}

void PluginProcessor::changeVoiceSize() {
  const auto voiceNum = getNumVoices();

  while (synth.getNumVoices() != voiceNum) {
    if (synth.getNumVoices() > voiceNum) {
      synth.removeVoice(synth.getNumVoices() - 1);
    } else {
      addVoice();
    }
  }
}

void PluginProcessor::clearBuffers(AudioBuffer<float>& buffer, AudioBuffer<float>& upSampleBuffer) {
  const auto totalNumInputChannels = getTotalNumInputChannels();
  const auto totalNumOutputChannels = getTotalNumOutputChannels();
  const auto totalChannel = totalNumInputChannels + totalNumInputChannels;
  for (auto channel = 0; channel < totalChannel; ++channel) {
    buffer.clear(channel, 0, buffer.getNumSamples());
  }

  for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i) {
    upSampleBuffer.clear(i, 0, upSampleBuffer.getNumSamples());
  }
}

float PluginProcessor::clippingFunction(float inputValue) {
  // 双曲線正接...1の時に0.8の値を, -1の時に-0.8の値を取る
  const float threshold = tanhf(inputValue);
  float outputValue = inputValue;

  // inputValueが正の値ならthresholdも正の値，inputValueが負の値ならthresholdも負の値を取るので，絶対値で比較する
  if (abs(inputValue) >= abs(threshold)) outputValue = threshold;

  return outputValue;
}

void PluginProcessor::initEffecters(dsp::ProcessSpec& spec) {
  drive.prepare(spec);

  clipper.prepare(spec);
  clipper.functionToUse = clippingFunction;

  hicutFilter.prepare(spec);
  lowcutFilter.prepare(spec);
}

void PluginProcessor::procMidiMessages(const AudioBuffer<float>& buffer, const MidiBuffer& midiMessages) {
  auto midibuffer = midiMessages;
  auto startSample = 0;
  auto numSamples = buffer.getNumSamples();

  MidiBuffer::Iterator i(midibuffer);
  MidiMessage message;
  int time;

  {
    MidiBuffer::Iterator i2(eventsToAdd);
    const auto firstEventToAdd = eventsToAdd.getFirstEventTime();
    const auto scaleFactor =
        numSamples /
        (float)(eventsToAdd.getLastEventTime() + 1 - firstEventToAdd);

    while (i2.getNextEvent(message, time)) {
      const int pos =
          jlimit(0, numSamples - 1,
                  roundToInt((time - firstEventToAdd) * scaleFactor));
      midibuffer.addEvent(message, startSample + pos);
    }
  }

  eventsToAdd.clear();
}