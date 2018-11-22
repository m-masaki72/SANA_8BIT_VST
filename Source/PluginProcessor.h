/*
==============================================================================

	PluginProcessor.h
	Created: 16 May 2018 1:55:55am
	Modified: 11 September 2018
	Author:  MasakiMori, COx2
	ChangeLog:
		Modified some parameters
		Modified some components

==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "DSP/SimpleSynthParameters.h"
#include "GUI/ScopeComponent.h"

class MidiSignalBuffer
{
public:

	MidiSignalBuffer(int freq, int block, float sec)
	{
		sampleRate = freq;
		echoTime = sec;
		blockSize = block;
		bufSize = sampleRate / blockSize * echoTime;
		buf = new MidiBuffer[bufSize];
	};

	~MidiSignalBuffer()
	{
		delete[] buf;
	};

	void addSample(MidiBuffer& mb, float amp)
	{
		MidiBuffer::Iterator itr(mb);
		MidiMessage m;
		int midiPositon;

		buf[index].clear();

		while (itr.getNextEvent(m, midiPositon))
		{
			m.setVelocity(0.3f);
			buf[index].addEvent(m, midiPositon);
		}
	};

	MidiBuffer getSample()
	{
		int preindex = index + 1;
		if (preindex >= bufSize) 
		{
			preindex = 0;
		}
		return buf[preindex];
	};

	void changeDeleyTime(float sec)
	{
		delete[] buf;
		echoTime = sec;
		bufSize = sampleRate / blockSize * echoTime;
		buf = new MidiBuffer[bufSize];
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
	MidiBuffer* buf;
	int sampleRate, blockSize;
	int index;
	int bufSize;
	float echoTime;
};

//==============================================================================
class SimpleSynthAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    SimpleSynthAudioProcessor();
    ~SimpleSynthAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;
	void initProgram();

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

	//==============================================================================
	MidiKeyboardState& getKeyboardState() { return keyboardState; }

	AudioBufferQueue<float>& getAudioBufferQueue() { return scopeDataQueue; }

	void changeVoiceSize();

	//Parameterの用意[1]
	const StringArray OSC_WAVE_TYPES{
		"NES_Square50%",
		"NES_Square25%",
		"NES_Square12.5%",
		"NES_Triangle",
		"Pure_Square50%",
		"Pure_Square25%",
		"Pure_Square12.5%",
		"Pure_Triangle",
		"Pure_Sine",
		"Pure_Saw",
		"NES_LongNoise",
		"NES_ShortNoise",
		"Pure_Lo-bitNoise",
		"Waveform Memory",
	};
	const StringArray SWEEP_SWITCH{
		"OFF",
		"Positive",
		"Negative"
	};

	ChipOscillatorParameters chipOscParameters;
	SweepParameters sweepParameters;
	VibratoParameters vibratoParameters;
	VoicingParameters voicingParameters;
	OptionsParameters optionsParameters;
	WaveformMemoryParameters waveformMemoryParameters;
	MidiEchoParameters midiEchoParameters;
	FilterParameters filterParameters;

private:
	Synthesiser synth;

	//preset index
	int currentProgIndex;

	//歪み用の関数[2]
	static float clippingFunction(float inputValue);

	//[3]のオブジェクトの初期化に必要な情報を保持する構造体[4]
	dsp::ProcessSpec spec;

	//DSPエフェクト，クリッパー，ドライブ，フィルタ
	dsp::WaveShaper<float> clipper;
	dsp::Gain<float> drive;
	dsp::ProcessorDuplicator<dsp::IIR::Filter<float>, dsp::IIR::Coefficients<float>> hicutFilter;
	dsp::ProcessorDuplicator<dsp::IIR::Filter<float>, dsp::IIR::Coefficients<float>> lowcutFilter;

	// GUI上のキーボードコンポーネントで生成されたMIDI情報を保持しておくオブジェクト.
	// MIDIキーボードの状態を同期するためのステートオブジェクト
	MidiKeyboardState keyboardState;

	// スコープパネルに波形を表示するためのデータバッファ
	AudioBufferQueue<float> scopeDataQueue;
	ScopeDataCollector<float> scopeDataCollector;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleSynthAudioProcessor)
};
