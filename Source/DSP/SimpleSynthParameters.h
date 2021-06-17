/*
  ==============================================================================

    SimpleSynthParameters.h
    Created: 10 May 2018 12:28:18am
        Modified: 11 September 2018
        Author:  MasakiMori, COx2

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

namespace {
const std::int32_t WAVESAMPLE_LENGTH = 32;
}

class SynthParametersBase {
 public:
  virtual ~SynthParametersBase(){};

  virtual void addAllParameters(AudioProcessor& processor) = 0;
  virtual void saveParameters(XmlElement& xml) = 0;
  virtual void loadParameters(XmlElement& xml) = 0;
};

class ChipOscillatorParameters : public SynthParametersBase {
 public:
  AudioParameterChoice* OscWaveType;
  AudioParameterFloat* VolumeLevel;
  AudioParameterFloat* Attack;
  AudioParameterFloat* Decay;
  AudioParameterFloat* Sustain;
  AudioParameterFloat* Release;

  ChipOscillatorParameters(AudioParameterChoice* OscWaveType,
                           AudioParameterFloat* volumeLevel,
                           AudioParameterFloat* attack,
                           AudioParameterFloat* decay,
                           AudioParameterFloat* sustain,
                           AudioParameterFloat* release);

  virtual void addAllParameters(AudioProcessor& processor) override;
  virtual void saveParameters(XmlElement& xml) override;
  virtual void loadParameters(XmlElement& xml) override;

 private:
  ChipOscillatorParameters(){};
};

class SweepParameters : public SynthParametersBase {
 public:
  AudioParameterChoice* SweepSwitch;
  AudioParameterFloat* SweepTime;

  SweepParameters(AudioParameterChoice* SweepSwitch,
                  AudioParameterFloat* SweepTime);

  virtual void addAllParameters(AudioProcessor& processor) override;
  virtual void saveParameters(XmlElement& xml) override;
  virtual void loadParameters(XmlElement& xml) override;

 private:
  SweepParameters(){};
};

class VibratoParameters : public SynthParametersBase {
 public:
  AudioParameterBool* VibratoEnable;
  AudioParameterBool* VibratoAttackDeleySwitch;
  AudioParameterFloat* VibratoAmount;
  AudioParameterFloat* VibratoSpeed;
  AudioParameterFloat* VibratoAttackTime;

  VibratoParameters(AudioParameterBool* vibratoEnable,
                    AudioParameterBool* attackDeleySwitch,
                    AudioParameterFloat* vibratoAmount,
                    AudioParameterFloat* vibratoSpeed,
                    AudioParameterFloat* vibratoAttackTime);

  virtual void addAllParameters(AudioProcessor& processor) override;
  virtual void saveParameters(XmlElement& xml) override;
  virtual void loadParameters(XmlElement& xml) override;

 private:
  VibratoParameters(){};
};

class VoicingParameters : public SynthParametersBase {
 public:
  AudioParameterChoice* VoicingSwitch;
  AudioParameterFloat* StepTime;

  VoicingParameters(AudioParameterChoice* sweepSwitch,
                    AudioParameterFloat* stepTime);

  virtual void addAllParameters(AudioProcessor& processor) override;
  virtual void saveParameters(XmlElement& xml) override;
  virtual void loadParameters(XmlElement& xml) override;

 private:
  VoicingParameters(){};
};

class OptionsParameters : public SynthParametersBase {
 public:
  AudioParameterInt* PitchBendRange;
  AudioParameterInt* PitchStandard;
  float currentBPM;

  OptionsParameters(AudioParameterInt* pitchBendRange,
                    AudioParameterInt* pitchStandard);

  virtual void addAllParameters(AudioProcessor& processor) override;
  virtual void saveParameters(XmlElement& xml) override;
  virtual void loadParameters(XmlElement& xml) override;

 private:
  OptionsParameters(){};
};

class WaveformMemoryParameters : public SynthParametersBase {
 public:
  AudioParameterInt* WaveSamplesArray[WAVESAMPLE_LENGTH];

  WaveformMemoryParameters();

  virtual void addAllParameters(AudioProcessor& processor) override;
  virtual void saveParameters(XmlElement& xml) override;
  virtual void loadParameters(XmlElement& xml) override;

 private:
};

class MidiEchoParameters : public SynthParametersBase {
 public:
  AudioParameterBool* IsEchoEnable;
  AudioParameterFloat* EchoDuration;
  AudioParameterInt* EchoRepeat;
  AudioParameterFloat* VolumeOffset;

  MidiEchoParameters(AudioParameterBool* isEchoEnable,
                     AudioParameterFloat* echoDuration,
                     AudioParameterInt* echoRepeat,
                     AudioParameterFloat* volumeOffset);

  virtual void addAllParameters(AudioProcessor& processor) override;
  virtual void saveParameters(XmlElement& xml) override;
  virtual void loadParameters(XmlElement& xml) override;

 private:
  MidiEchoParameters(){};
};

class FilterParameters : public SynthParametersBase {
 public:
  AudioParameterBool* HicutEnable;
  AudioParameterBool* LowcutEnable;
  AudioParameterFloat* HicutFreq;
  AudioParameterFloat* LowcutFreq;

  FilterParameters(AudioParameterBool* hicutEnable,
                   AudioParameterBool* lowcutEnable,
                   AudioParameterFloat* hicutFreq,
                   AudioParameterFloat* lowcutFreq);

  virtual void addAllParameters(AudioProcessor& processor) override;
  virtual void saveParameters(XmlElement& xml) override;
  virtual void loadParameters(XmlElement& xml) override;

 private:
  FilterParameters(){};
};

class PresetsParameters : public SynthParametersBase {
 public:
  AudioParameterInt* ProgramIndex;
  PresetsParameters(AudioParameterInt* programIndex);

  virtual void addAllParameters(AudioProcessor& processor) override;
  virtual void saveParameters(XmlElement& xml) override;
  virtual void loadParameters(XmlElement& xml) override;

 private:
  PresetsParameters(){};
};
