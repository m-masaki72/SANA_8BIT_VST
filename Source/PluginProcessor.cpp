﻿/*
==============================================================================

	PluginProcessor.cpp
	Created: 16 May 2018 1:55:55am
	Modified: 11 September 2018
	Author:  MasakiMori, COx2
	ChangeLog:
	Modified some parameters
	Modified some components

==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

#include "DSP/SimpleSound.h"
#include "DSP/SimpleVoice.h"

//==============================================================================
SimpleSynthAudioProcessor::SimpleSynthAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
	, chipOscParameters{
		new AudioParameterChoice("OSC_WAVE_TYPE", "Osc-WaveType", OSC_WAVE_TYPES, 0),
		new AudioParameterFloat("VOLUME",	"Volume", -16.0f, 16.0f, -8.0f),
		new AudioParameterFloat("AMPENV_ATTACK", "Attack", 0.0f, 10.0f, 0.0f),
		new AudioParameterFloat("AMPENV_DECAY", "Decay",  0.0f, 10.0f, 0.0f),
		new AudioParameterFloat("AMPENV_SUSTAIN", "Sustain", 0.0f, 1.0f, 1.0f),
		new AudioParameterFloat("AMPENV_RELEASE", "Release", 0.0f, 10.0f, 0.f)
	}
	, sweepParameters{
		new AudioParameterChoice("SWEEP_SWITCH", "Sweep-Switch", SWEEP_SWITCH, 0),
		new AudioParameterFloat("SWEEP_TIME", "Sweep-Time",  0.1f, 10.0f, 1.0f),
	}
	, vibratoParameters{
		new AudioParameterBool("VIBRATO_ENABLE", "Vibrato-Enable", true),
		new AudioParameterFloat("VIBRATO_DEPTH", "Vibrato-Depth",  0.0f, 10.0f, 0.0000f),
		new AudioParameterFloat("VIBRATO_SPEED", "Vibrato-Speed",  0.0f, 20.0f, 0.1000f)
	}
	, optionsParameters{
		new AudioParameterBool("IS_POLY_MODE", "Is-Poly-Mode", true),
		new AudioParameterBool("IS_VELOCITY_SENSE", "Is-Velocity-Sense", true),
		new AudioParameterInt("PITCH_BEND_RANGE", "Pitch-Bend-Range", 1, 13, 2),
		new AudioParameterInt("PITCH_STANDARD", "Pitch-Standard", 400, 500, 440)
	}
	, waveformMemoryParameters{
		new AudioParameterInt("W1", "w1", INT32_MIN, INT32_MAX, 1),
		new AudioParameterInt("W2", "w2", INT32_MIN, INT32_MAX, 1),
		new AudioParameterInt("W3", "w3", INT32_MIN, INT32_MAX, 1),
		new AudioParameterInt("W4", "w4", INT32_MIN, INT32_MAX, 1),
		new AudioParameterInt("W5", "w5", INT32_MIN, INT32_MAX, 1),
		new AudioParameterInt("W6", "w6", INT32_MIN, INT32_MAX, 1),
	}
	, scopeDataCollector(scopeDataQueue)
{
	chipOscParameters.addAllParameters(*this);
	sweepParameters.addAllParameters(*this);
	vibratoParameters.addAllParameters(*this);
	optionsParameters.addAllParameters(*this);
	waveformMemoryParameters.addAllParameters(*this);
}

SimpleSynthAudioProcessor::~SimpleSynthAudioProcessor()
{
}

//==============================================================================
const String SimpleSynthAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SimpleSynthAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SimpleSynthAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SimpleSynthAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SimpleSynthAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SimpleSynthAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SimpleSynthAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SimpleSynthAudioProcessor::setCurrentProgram (int index)
{
}

const String SimpleSynthAudioProcessor::getProgramName (int index)
{
    return {};
}

void SimpleSynthAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void SimpleSynthAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

	synth.clearSounds();
	synth.clearVoices();

	synth.setCurrentPlaybackSampleRate(sampleRate);

	// サウンド再生可能なノート番号の範囲を定義する。関数"setRange" にて0～127の値をtrueに設定する。
	BigInteger canPlayNotes;
	canPlayNotes.setRange(0, 127, true);
	// サウンド再生可能なチャンネル番号の範囲を定義する。関数"setRange" にて0～127の値をtrueに設定する。
	BigInteger canPlayChannels;
	canPlayChannels.setRange(1, 16, true);
	synth.addSound(new SimpleSound(canPlayNotes, canPlayChannels));

	int numVoices = optionsParameters.IsPolyMode->get() ? 16 : 1;
	for (int i = 0; i < numVoices; ++i)
	{
		synth.addVoice(new SimpleVoice(&chipOscParameters, &sweepParameters, &vibratoParameters, &optionsParameters, &waveformMemoryParameters));
	}

	spec.sampleRate = sampleRate;
	spec.numChannels = getTotalNumOutputChannels();
	spec.maximumBlockSize = samplesPerBlock;

	drive.prepare(spec);

	clipper.prepare(spec);
	clipper.functionToUse = clippingFunction;

	masterVolume.prepare(spec);
}

void SimpleSynthAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SimpleSynthAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void SimpleSynthAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
	// ⑦MidiKeyboardStateクラスのオブジェクトにMIDIデバイスから入力されたMIDIバッファを渡すとともに、
	//   GUIのキーボードコンポーネントで生成されたMIDIデータをのMIDIバッファに追加する処理を行う。
	keyboardState.processNextMidiBuffer(midiMessages, 0, buffer.getNumSamples(), true);

	if (optionsParameters.IsPolyMode->get() ? 16 : 1 != synth.getNumVoices())
	{
		changeVoiceSize();
	}

    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...

		// シンセサイザーでバッファに対して加算処理を行う前にゼロクリアをしておく。
		buffer.clear(channel, 0, buffer.getNumSamples());

    }

	//================================ ボイスセクション ====================================

	synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());

	//================================ エフェクトセクション ====================================

	// juce::dspモジュールに渡せるように入力信号バッファをラップする
	dsp::AudioBlock<float> audioBlock(buffer);
	dsp::ProcessContextReplacing<float> context(audioBlock);

	// ゲインを上げる
	drive.setGainDecibels(chipOscParameters.VolumeLevel->get());
	drive.process(context);

	// クリッピング処理
	clipper.process(context);

	// マスターボリューム調整 -10fぐらいにしたほうがいい
	masterVolume.setGainDecibels(-8.0f);
	masterVolume.process(context);

	// ⑧現時点でオーディオバッファで保持しているサンプルデータをScopeDataCollectorクラスのオブジェクトに渡す。
	scopeDataCollector.process(buffer.getReadPointer(0), (size_t)buffer.getNumSamples());
}

//==============================================================================
bool SimpleSynthAudioProcessor::hasEditor() const
{
	return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* SimpleSynthAudioProcessor::createEditor()
{
    return new SimpleSynthAudioProcessorEditor (*this);
}

//==============================================================================
void SimpleSynthAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
	
	std::unique_ptr<XmlElement> xml(new XmlElement("SimpleSynthParameters"));

	chipOscParameters.saveParameters(*xml);
	sweepParameters.saveParameters(*xml);
	vibratoParameters.saveParameters(*xml);
	optionsParameters.saveParameters(*xml);
	waveformMemoryParameters.saveParameters(*xml);

	copyXmlToBinary(*xml, destData);
}

void SimpleSynthAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
	
	std::unique_ptr<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

	if (xmlState.get() != nullptr)
	{
		if (xmlState->hasTagName("SimpleSynthParameters")) 
		{
			chipOscParameters.loadParameters(*xmlState);
			sweepParameters.loadParameters(*xmlState);
			vibratoParameters.loadParameters(*xmlState);
			optionsParameters.loadParameters(*xmlState);
			waveformMemoryParameters.loadParameters(*xmlState);
		}
	}
}

void SimpleSynthAudioProcessor::changeVoiceSize()
{
	int voiceNum = optionsParameters.IsPolyMode->get() ? 16 : 1;

	while (synth.getNumVoices() != voiceNum)
	{
		if (synth.getNumVoices() > voiceNum)
		{
			synth.removeVoice(synth.getNumVoices() - 1);
		}
		else
		{
			synth.addVoice(new SimpleVoice(&chipOscParameters, &sweepParameters, &vibratoParameters, &optionsParameters, &waveformMemoryParameters));
		}
	}
}

float SimpleSynthAudioProcessor::clippingFunction(float inputValue)
{
	// 双曲線正接...1の時に0.8の値を, -1の時に-0.8の値を取る
	float threshold = tanhf(inputValue);
	float outputValue = inputValue;

	//inputValueが正の値ならthresholdも正の値，inputValueが負の値ならthresholdも負の値を取るので，絶対値で比較する
	if (abs(inputValue) >= abs(threshold)) outputValue = threshold;

	return outputValue;
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SimpleSynthAudioProcessor();
}