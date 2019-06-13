/*
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
#include <set>    
#include "DSP/SimpleSound.h"
#include "DSP/SimpleVoice.h"


namespace
{
	const std::int32_t NUM_OF_PRESETS = 12;
	const std::int32_t VOICE_MAX = 8;
}

//==============================================================================
SimpleSynthAudioProcessor::SimpleSynthAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
	: AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
		.withInput("Input", AudioChannelSet::stereo(), true)
#endif
		.withOutput("Output", AudioChannelSet::stereo(), true)
#endif
	)
#endif
, presetsParameters{
	new AudioParameterInt("PROGRAM_INDEX", "Program-Index", 0, NUM_OF_PRESETS, 0)
}
, chipOscParameters{
	new AudioParameterChoice("OSC_WAVE_TYPE", "Osc-WaveType", OSC_WAVE_TYPES, 0),
	new AudioParameterFloat("VOLUME",	"Volume", -32.0f, 8.0f, -20.0f),
	new AudioParameterFloat("AMPENV_ATTACK", "Attack", 0.000f, 10.0f, 0.000f),
	new AudioParameterFloat("AMPENV_DECAY", "Decay",  0.000f, 10.0f, 0.000f),
	new AudioParameterFloat("AMPENV_SUSTAIN", "Sustain", 0.000f, 1.0f, 1.0f),
	new AudioParameterFloat("AMPENV_RELEASE", "Release", 0.000f, 10.0f, 0.000f)
}
, sweepParameters{
	new AudioParameterChoice("SWEEP_SWITCH", "Sweep-Switch", SWEEP_SWITCH, 0),
	new AudioParameterFloat("SWEEP_TIME", "Sweep-Time",  0.01f, 10.0f, 1.0f),
}
, vibratoParameters{
	new AudioParameterBool("VIBRATO_ENABLE", "Vibrato-Enable", true),
	new AudioParameterBool("VIBRATO_ATTACK-DELEY-SWITCH", "Vibrato-Attack-Deley-Switch", true),
	new AudioParameterFloat("VIBRATO_DEPTH", "Vibrato-Depth",  0.0f, 24.0f, 0.0f),
	new AudioParameterFloat("VIBRATO_SPEED", "Vibrato-Speed",  0.0f, 20.0f, 0.1000f),
	new AudioParameterFloat("VIBRATO_ATTACKTIME", "Vibrato-AttackTime",  0.0f, 15.0f, 0.0f)
}
, voicingParameters{
	new AudioParameterChoice("VOICING_TYPE", "Voicing-Type", VOICING_SWITCH, 0),
	new AudioParameterFloat("STEP_TIME", "Step-Time",  0.0f, 3.0f, 0.0f)
}
, optionsParameters{
	new AudioParameterInt("PITCH_BEND_RANGE", "Pitch-Bend-Range", 1, 13, 2),
	new AudioParameterInt("PITCH_STANDARD", "Pitch-Standard", 400, 500, 440)
}
, midiEchoParameters{
	new AudioParameterBool("ECHO_ENABLE", "Echo-Enable", false),
	new AudioParameterFloat("ECHO_DURATION", "Echo-Duration", 0.01f, 3.0f, 0.1f),
	new AudioParameterInt("ECHO_REPEAT", "Echo-Repeat", 1, 5, 1),
	new AudioParameterFloat("ECHO_VOLUMEOFFSET", "Echo-VolumeOffset", 0.0f, 200.0f, 50.0f)
}
, filterParameters{
	new AudioParameterBool("HICUT_ENABLE", "Filter-Hicut-Enable", false),
	new AudioParameterBool("LOWCUT_ENABLE", "Filter-Lowcut-Enable", false),
	new AudioParameterFloat("FILTER_HICUT-FREQ", "Filter-Hicut-Freq", 40.0f, 20000.0f, 20000.0f),
	new AudioParameterFloat("FILTER_LOWCUT-FREQ", "Filter-Lowcut-Freq", 40.0f, 20000.0f, 40.0f)
}
, waveformMemoryParameters()
, scopeDataCollector(scopeDataQueue)
{
	presetsParameters.addAllParameters(*this);
	chipOscParameters.addAllParameters(*this);
	sweepParameters.addAllParameters(*this);
	vibratoParameters.addAllParameters(*this);
	voicingParameters.addAllParameters(*this);
	optionsParameters.addAllParameters(*this);
	midiEchoParameters.addAllParameters(*this);
	waveformMemoryParameters.addAllParameters(*this);
	filterParameters.addAllParameters(*this);
}

SimpleSynthAudioProcessor::~SimpleSynthAudioProcessor()
{}

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
    return NUM_OF_PRESETS;
}

int SimpleSynthAudioProcessor::getCurrentProgram()
{
	currentProgIndex = presetsParameters.ProgramIndex->get();
	return presetsParameters.ProgramIndex->get();
}

void SimpleSynthAudioProcessor::setCurrentProgram (int index)
{
	if (currentProgIndex != index)
	{
		initProgram();

		switch (index)
		{
		case 0:
			break;
		case 1:
			*chipOscParameters.OscWaveType = 2;
			*chipOscParameters.Decay =  5.000f;
			*chipOscParameters.Sustain = 0.0f;
			*vibratoParameters.VibratoAmount =  0.05f;
			*vibratoParameters.VibratoSpeed  = 13.0f;
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

const String SimpleSynthAudioProcessor::getProgramName (int index)
{
	switch (index)
	{
	case 0 :
		return("Initial Program");
	case 1:
		return("Voice Lead");
	case 2:
		return("String");
	case 3:
		return("Mallet");
	case 4:
		return("Brass");
	case 5:
		return("Modulated Lead");
	case 6:
		return("Backing Lead");
	case 7:
		return("Riser Lead");
	case 8:
		return("Noise Hihat");
	case 9:
		return("Noise Snare");
	case 10:
		return("Noise Cymbal");
	case 11:
		return("Pulse Bass&Tom");
	default:
		return("");
	}
}

void SimpleSynthAudioProcessor::changeProgramName (int index, const String& newName)
{
}

void SimpleSynthAudioProcessor::initProgram() 
{
	*chipOscParameters.OscWaveType = 0;
	*chipOscParameters.VolumeLevel = -20.0f;	
	*chipOscParameters.Attack =  0.000f;
	*chipOscParameters.Decay =  0.000f;
	*chipOscParameters.Sustain = 1.0f;
	*chipOscParameters.Release = 0.000f;

	*sweepParameters.SweepSwitch = 0;
	*sweepParameters.SweepTime = 1.0f;

	*vibratoParameters.VibratoEnable = true;
	*vibratoParameters.VibratoAmount = 0.0f;
	*vibratoParameters.VibratoSpeed = 0.1000f;
	*vibratoParameters.VibratoAttackTime = 0.0f;
}

//==============================================================================
void SimpleSynthAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

	synth.clearSounds();
	synth.clearVoices();

	synth.setCurrentPlaybackSampleRate(sampleRate * upSamplingFactor);

	// サウンド再生可能なノート番号の範囲を定義する。関数"setRange" にて0～127の値をtrueに設定する。
	BigInteger canPlayNotes;
	canPlayNotes.setRange(0, 127, true);
	// サウンド再生可能なチャンネル番号の範囲を定義する。関数"setRange" にて0～127の値をtrueに設定する。
	BigInteger canPlayChannels;
	canPlayChannels.setRange(1, 2, true);
	synth.addSound(new SimpleSound(canPlayNotes, canPlayChannels));


	for (auto i = 0; i < getNumVoices(); ++i)
	{
		addVoice();
	}

	spec.sampleRate = sampleRate;
	spec.numChannels = getTotalNumOutputChannels();
	spec.maximumBlockSize = samplesPerBlock;

	antiAliasFilter.prepare(sampleRate, upSamplingFactor);

	drive.prepare(spec);

	clipper.prepare(spec);
	clipper.functionToUse = clippingFunction;
	
	hicutFilter.prepare(spec);
	lowcutFilter.prepare(spec);	
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
	//playHead = getPlayHead();
	//if (playHead)
	//{
	//	playHead->getCurrentPosition(currentPositionInfo);
	//}
	//optionsParameters.currentBPM = (float)currentPositionInfo.bpm;

	// ⑦MidiKeyboardStateクラスのオブジェクトにMIDIデバイスから入力されたMIDIバッファを渡すとともに、
	//   GUIのキーボードコンポーネントで生成されたMIDIデータをのMIDIバッファに追加する処理を行う。
	keyboardState.processNextMidiBuffer(midiMessages, 0, buffer.getNumSamples(), true);

	// procMidiMessage
	{
		auto midibuffer = midiMessages;
		auto startSample = 0;
		auto numSamples = buffer.getNumSamples();

		MidiBuffer::Iterator i(midibuffer);
		MidiMessage message;
		int time;

		while (i.getNextEvent(message, time))
		{
			if (message.isNoteOn())
			{
				midiList.insert(message.getNoteNumber());
			}
			else if (message.isNoteOff())
			{
				midiList.erase(message.getNoteNumber());
			}
			else if (message.isAllNotesOff())
			{
				midiList.clear();
			}
		}

		{
			MidiBuffer::Iterator i2(eventsToAdd);
			const int firstEventToAdd = eventsToAdd.getFirstEventTime();
			const double scaleFactor = numSamples / (double)(eventsToAdd.getLastEventTime() + 1 - firstEventToAdd);

			while (i2.getNextEvent(message, time))
			{
				const int pos = jlimit(0, numSamples - 1, roundToInt((time - firstEventToAdd) * scaleFactor));
				midibuffer.addEvent(message, startSample + pos);
			}
		}

		eventsToAdd.clear();
	}

	if (getNumVoices() != synth.getNumVoices())
	{
		changeVoiceSize();
	}

    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto channel = 0; channel < totalNumInputChannels + totalNumInputChannels; ++channel)
    {
		buffer.clear(channel, 0, buffer.getNumSamples());
    }

	AudioBuffer<float> upSampleBuffer(buffer.getNumChannels(), buffer.getNumSamples() * upSamplingFactor);

	for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
	{
		upSampleBuffer.clear(i, 0, upSampleBuffer.getNumSamples());
	}
	
	synth.renderNextBlock(upSampleBuffer, midiMessages, 0, upSampleBuffer.getNumSamples());

	//================================ アンチエイリアス      ====================================
	
	antiAliasFilter.process(buffer, upSampleBuffer, totalNumInputChannels, totalNumOutputChannels);
	
	//================================ エフェクトセクション ====================================

	dsp::AudioBlock<float> audioBlock(buffer);
	dsp::ProcessContextReplacing<float> context(audioBlock);

	// ゲインを上げる
	drive.setGainDecibels(chipOscParameters.VolumeLevel->get());
	drive.process(context);

	// フィルタ処理
	{
		if (filterParameters.HicutEnable->get())
		{
			*hicutFilter.state = *dsp::IIR::Coefficients<float>::makeLowPass(getSampleRate(), filterParameters.HicutFreq->get());
			hicutFilter.process(context);
		}
		if (filterParameters.LowcutEnable->get())
		{
			*lowcutFilter.state = *dsp::IIR::Coefficients<float>::makeHighPass(getSampleRate(), filterParameters.LowcutFreq->get());
			lowcutFilter.process(context);
		}		
	}
	// クリッピング処理
	clipper.process(context);

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

	presetsParameters.saveParameters(*xml);
	chipOscParameters.saveParameters(*xml);
	sweepParameters.saveParameters(*xml);
	vibratoParameters.saveParameters(*xml);
	voicingParameters.saveParameters(*xml);
	optionsParameters.saveParameters(*xml);
	midiEchoParameters.saveParameters(*xml);
	waveformMemoryParameters.saveParameters(*xml);
	filterParameters.saveParameters(*xml);

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
			presetsParameters.loadParameters(*xmlState);
			chipOscParameters.loadParameters(*xmlState);
			sweepParameters.loadParameters(*xmlState);
			vibratoParameters.loadParameters(*xmlState);
			voicingParameters.loadParameters(*xmlState);
			optionsParameters.loadParameters(*xmlState);
			midiEchoParameters.loadParameters(*xmlState);
			waveformMemoryParameters.loadParameters(*xmlState);
			filterParameters.loadParameters(*xmlState);
		}

		//Preset用のパラメータを更新（変数をローカルintで保存しないとシンセ終了時にフリーズする
		currentProgIndex = presetsParameters.ProgramIndex->get();
	}
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SimpleSynthAudioProcessor();
}

//==============================================================================
// class functions
void SimpleSynthAudioProcessor::changeVoiceSize()
{
	const auto voiceNum = getNumVoices();

	while (synth.getNumVoices() != voiceNum)
	{
		if (synth.getNumVoices() > voiceNum)
		{
			synth.removeVoice(synth.getNumVoices() - 1);
		}
		else
		{
			addVoice();
		}
	}
}

float SimpleSynthAudioProcessor::clippingFunction(float inputValue)
{
	// 双曲線正接...1の時に0.8の値を, -1の時に-0.8の値を取る
	const float threshold = tanhf(inputValue);
	float outputValue = inputValue;

	//inputValueが正の値ならthresholdも正の値，inputValueが負の値ならthresholdも負の値を取るので，絶対値で比較する
	if (abs(inputValue) >= abs(threshold)) outputValue = threshold;

	return outputValue;
}

std::int32_t SimpleSynthAudioProcessor::getNumVoices()
{
	if (voicingParameters.VoicingSwitch->getCurrentChoiceName() == "POLY")
	{
		return VOICE_MAX;
	}
	else
	{
		return 1;
	}
}

void SimpleSynthAudioProcessor::addVoice()
{
	synth.addVoice(
		new SimpleVoice(
			&chipOscParameters,
			&sweepParameters,
			&vibratoParameters,
			&voicingParameters,
			&optionsParameters,
			&midiEchoParameters,
			&waveformMemoryParameters,
			&midiList
		)
	);
}