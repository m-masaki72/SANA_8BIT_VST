/*
  ==============================================================================

    SimpleVoice.h
    Created: 20 Apr 2018 2:08:09am
	Modified: 11 September 2018
	Author:  MasakiMori, COx2

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "SimpleSynthParameters.h"
#include "Waveforms.h"
#include "AmpEnvelope.h"
#include "SimpleSound.h"

namespace
{
}

//エコーエフェクト用のサンプリングバッファ
class EchoBuffer
{
public:
	EchoBuffer(int freq, float sec, int count)
	{
		sampleRate = freq;
		echoTime = sec;
		echoCount = count;
		init();
	};

	~EchoBuffer()
	{};

	void init() {

		index = 0;
		bufSize = (int)(sampleRate * echoTime);

		if (bufSize <= 0)
		{
			bufSize = 0;
		}

		buf.resize(echoCount);
		for (int i = 0; i < echoCount; ++i)
		{
			buf[i].resize(bufSize);
		}

		for (int i = 0; i < echoCount; ++i)
		{
			for (int j = 0; j < bufSize; ++j)
			{
				buf[i][j] = 0.0f;
			}
		}
	}

	void addSample(float val, float amp)
	{
		if (index >= bufSize)
		{
			init();
		}
		//一つずつ前のバッファのサンプルを補完，1サンプル前のバッファを補完していく
		for (int i = echoCount - 1; i > 0; --i)
		{
			buf[i][index] = buf[i-1][index] * amp;
		}
		//現在のサンプルを補完
		buf[0][index] = val * amp;
	};

	// バッファの最後尾のサンプルを返す．
	// 最後尾はbuffSizeだけ遅れたサンプルになるのでDelayされる
	float getSample(int repeatCount)
	{
		if (repeatCount >= echoCount)
		{
			init();
			return 0.0f;
		}

		if (index >= bufSize)
		{
			init();
			return 0.0f;
		}

		return buf[repeatCount][index+1];
	};

	void updateParam(float sec, int count)
	{
		if (echoTime != sec || echoCount != count)
		{
			echoTime = sec;
			echoCount = count;
			init();
		}
	}

	void cycle()
	{
		index += 1;
		if (index >= bufSize)
		{
			index = 0;
		}
	}

private:
	std::vector<std::vector<float>> buf;
	int sampleRate;
	int echoCount;
	float echoTime;

	int bufSize;
	int index;
};

class SimpleVoice : public SynthesiserVoice
{
public:
	SimpleVoice(
		ChipOscillatorParameters* chipOscParams, 
		SweepParameters* sweepParams, 
		VibratoParameters* vibratoParams,
		VoicingParameters* voicingParams,
		OptionsParameters* optionsParams,
		MidiEchoParameters* midiEchoParams,
		WaveformMemoryParameters* waveformMemoryParams
	);

	virtual ~SimpleVoice();

	// 基底クラスで宣言された純粋仮想関数をオーバーライド宣言する。
	virtual bool canPlaySound(SynthesiserSound* sound) override;
	virtual void startNote(int midiNoteNumber, float velocity, SynthesiserSound* sound, int currentPitchWheelPosition) override;
	virtual void stopNote(float velocity, bool allowTailOff) override;
	virtual void pitchWheelMoved(int newPitchWheelValue) override;
	virtual void controllerMoved(int controllerNumber, int newControllerValue) override;
	virtual void renderNextBlock(AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;

private:
	float calcModulationFactor(float angle);

	float currentAngle, vibratoAngle, angleDelta, portaAngleDelta;
	float level;
	float pitchBend, pitchSweep;
	std::vector<float> echoSamples;
	
	//RingBuffer rb;
	EchoBuffer eb;

	//Waveform用のパラメータ
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
};