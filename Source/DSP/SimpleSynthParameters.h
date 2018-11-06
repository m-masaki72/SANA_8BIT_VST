/*
  ==============================================================================

    SimpleSynthParameters.h
    Created: 10 May 2018 12:28:18am
	Modified: 11 September 2018
	Author:  MasakiMori, COx2

  ==============================================================================
*/

#pragma once

// ①JUCEライブラリのヘッダをインクルードする。
#include "../JuceLibraryCode/JuceHeader.h"

// ②クラス宣言。複数のパラメータをまとめるクラス群の基底クラス。
class SynthParametersBase
{
public:
	// デストラクタ
	virtual ~SynthParametersBase() {};

	// ③継承クラス側で実装を必須とする関数を純粋仮想関数として宣言する。
	virtual void addAllParameters(AudioProcessor& processor) = 0;
	virtual void saveParameters(XmlElement& xml) = 0; 
	virtual void loadParameters(XmlElement& xml) = 0;
};

class ChipOscillatorParameters : public SynthParametersBase
{
public:
	// ⑤各波形の音量レベルを管理するパラメータのポインタ変数。
	AudioParameterChoice* OscWaveType;
	AudioParameterFloat* VolumeLevel;
	AudioParameterFloat* Attack;
	AudioParameterFloat* Decay;
	AudioParameterFloat* Sustain;
	AudioParameterFloat* Release;

	// ⑥引数付きコンストラクタ。PluginProcessor.h/cpp側で保持するパラメータのポインタ変数を受け取る。
	ChipOscillatorParameters(
		AudioParameterChoice* OscWaveType ,
		AudioParameterFloat* volumeLevel,
		AudioParameterFloat* attack,
		AudioParameterFloat* decay,
		AudioParameterFloat* sustain,
		AudioParameterFloat* release);
	//chipOscillatorParameters(AudioParameterFloat* sineWaveLevel);

	// ⑦基底クラスで宣言されている純粋仮想関数をオーバーライドして実装する。
	virtual void addAllParameters(AudioProcessor& processor) override;
	virtual void saveParameters(XmlElement& xml) override;
	virtual void loadParameters(XmlElement& xml) override;

private:
	// 引数無しコントラクタをprivate領域に置くことで、外から呼び出せないようにする。
	ChipOscillatorParameters() {};
};

class SweepParameters : public SynthParametersBase
{
public:
	AudioParameterChoice* SweepSwitch;
	AudioParameterFloat*  SweepTime;

	SweepParameters(
		AudioParameterChoice* SweepSwitch,
		AudioParameterFloat*  SweepTime
	);

	virtual void addAllParameters(AudioProcessor& processor) override;
	virtual void saveParameters(XmlElement& xml) override;
	virtual void loadParameters(XmlElement& xml) override;

private:
	SweepParameters() {};
};

class VibratoParameters : public SynthParametersBase
{
public:
	AudioParameterBool * VibratoEnable;
	AudioParameterFloat*  VibratoAmount;
	AudioParameterFloat*  VibratoSpeed;
	AudioParameterFloat*  VibratoAttackTime;

	VibratoParameters(
		AudioParameterBool* vibratoEnable,
		AudioParameterFloat*  vibratoAmount,
		AudioParameterFloat*  vibratoSpeed,
		AudioParameterFloat*  vibratoAttackTime
	);

	virtual void addAllParameters(AudioProcessor& processor) override;
	virtual void saveParameters(XmlElement& xml) override;
	virtual void loadParameters(XmlElement& xml) override;

private:
	VibratoParameters() {};
};

class VoicingParameters : public SynthParametersBase
{
public:
	AudioParameterChoice* VoicingSwitch;
	AudioParameterFloat* PortaTime;
	AudioParameterFloat* ArpFreq;

	VoicingParameters(
		AudioParameterChoice* sweepSwitch,
		AudioParameterFloat* portaTime,
		AudioParameterFloat* arpFreq
	);

	virtual void addAllParameters(AudioProcessor& processor) override;
	virtual void saveParameters(XmlElement& xml) override;
	virtual void loadParameters(XmlElement& xml) override;

private:
	VoicingParameters() {};
};

class OptionsParameters : public SynthParametersBase
{
public:
	AudioParameterBool* IsVelocitySense;
	AudioParameterInt*	PitchBendRange;
	AudioParameterInt*	PitchStandard;

	OptionsParameters(
		AudioParameterBool* isVelocitySense,
		AudioParameterInt* pitchBendRange,
		AudioParameterInt*	pitchStandard
	);

	virtual void addAllParameters(AudioProcessor& processor) override;
	virtual void saveParameters(XmlElement& xml) override;
	virtual void loadParameters(XmlElement& xml) override;

private:
	OptionsParameters() {};
};

class WaveformMemoryParameters : public SynthParametersBase
{
public:
	AudioParameterInt* WaveSamplesArray[32];

	WaveformMemoryParameters();

	virtual void addAllParameters(AudioProcessor& processor) override;
	virtual void saveParameters(XmlElement& xml) override;
	virtual void loadParameters(XmlElement& xml) override;

private:
};
