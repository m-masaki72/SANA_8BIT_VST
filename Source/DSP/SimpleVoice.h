/*
  ==============================================================================

    SimpleVoice.h
    Created: 20 Apr 2018 2:08:09am
	Modified: 11 September 2018
	Author:  MasakiMori, COx2

  ==============================================================================
*/

#pragma once

// ①各種ヘッダファイルをインクルードする。
#include "../JuceLibraryCode/JuceHeader.h"
#include "SimpleSynthParameters.h"
#include "Waveforms.h"
#include "AmpEnvelope.h"
#include "SimpleSound.h"
#include <vector>

#define buf_size 7

//レンダリング用の簡易リングバッファの実装
class RingBuffer
{
public:
	RingBuffer()
	{
		for (int i = 0; i < buf_size; ++i)
		{
			data[i] = 0.0f;
		}
	};

	void push_back(float sample)
	{
		data[index] = sample;
		index = (index + 1) % buf_size;
	};

	float getCurrentValue()
	{
		//ガウシアンフィルタを通して出力する
		//3 sample: 1 2 1
		//5 sample: 1 4 6 4 1
		//7 sample 1 6 15 20 15 6 1
		float val = 0.0f;
		for (int i = 0; i < buf_size; ++i)
		{
			switch (i - index)
			{
			case -6:
				val += data[i] * 15.0;
				break;
			case -5:
				val += data[i] * 15.0;
				break;
			case -4:
				val += data[i] * 6.0;
				break;
			case -3:
				val += data[i] * 6.0;
				break;
			case -2:
				val += data[i] * 1.0;
				break;
			case -1:
				val += data[i] * 1.0;
				break;
			case 0:
				val += data[i] * 20.0;
				break;
			case 1:
				val += data[i] * 15.0;
				break;
			case 2:
				val += data[i] * 15.0;
				break;
			case 3:
				val += data[i] * 6.0;
				break;
			case 4:
				val += data[i] * 6.0;
				break;
			case 5:
				val += data[i] * 1.0;
				break;
			case 6:
				val += data[i] * 1.0;
				break;
			}
		}
		return val / 64.0;
	}

private:
	int index = 0;
	float data[buf_size];
};

//エコー用のバッファ
// バッファは エコー秒数 * 最大エコーバッファサイズ を常に確保する．
// 今回は bufSize * 5

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
		bufSize = sampleRate * echoTime;
		if (bufSize <= 0)
		{
			bufSize = 0;
		}
		buf.resize(echoCount);
		for (int i = 0; i < echoCount; ++i)
		{
			buf[i].resize(bufSize);
		}
		clear();
	}

	void clear() {
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
			index = 0;
		}
		for (int i = buf.size() - 1; i > 0; --i)
		{
			buf[i][index] = buf[i-1][index] * amp;
		}
		buf[0][index] = val * amp;
	};

	float getSample(int repeatCount)
	{
		if (repeatCount >= buf.size())
		{
			return 0.0f;
		}

		int preindex = index + 1;
		if (preindex >= bufSize)
		{
			preindex = 0;
		}
		return buf[repeatCount][preindex];
	};

	void update(float sec, int count)
	{
		echoTime = sec;
		echoCount = count;
		init();
	}

	void countUp()
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
	const int ECHOMAX = 5;
	int bufSize;

	int index;
	int echoCount;
	float echoTime;
};

// ②クラス名宣言。SynthesiserVoiceクラスを継承する。
class SimpleVoice : public SynthesiserVoice
{
public:
	// ③引数付きコンストラクタ。
	SimpleVoice(
		ChipOscillatorParameters* chipOscParams, 
		SweepParameters* sweepParams, 
		VibratoParameters* vibratoParams,
		VoicingParameters* voicingParams,
		OptionsParameters* optionsParams,
		MidiEchoParameters* midiEchoParams,
		WaveformMemoryParameters* waveformMemoryParams
	);

	// デストラクタ
	virtual ~SimpleVoice();

	// ④基底クラスで宣言された純粋仮想関数をオーバーライド宣言する。
	virtual bool canPlaySound(SynthesiserSound* sound) override;
	virtual void startNote(int midiNoteNumber, float velocity, SynthesiserSound* sound, int currentPitchWheelPosition) override;
	virtual void stopNote(float velocity, bool allowTailOff) override;
	virtual void pitchWheelMoved(int newPitchWheelValue) override;
	virtual void controllerMoved(int controllerNumber, int newControllerValue) override;
	virtual void renderNextBlock(AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;

private:
	// ⑤クラス内でのみ呼び出す関数をprivate領域に宣言する。
	float calcModulationFactor(float angle);

	// ⑥クラス内変数を宣言する。
	float currentAngle, vibratoAngle, angleDelta, portaAngleDelta;
	float level;
	float pitchBend, pitchSweep;
	std::vector<float> echoSamples;
	
	RingBuffer rb;
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