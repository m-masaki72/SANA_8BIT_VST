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

#include "DSP/SimpleSound.h"
#include "DSP/SimpleVoice.h"

#define NUM_OF_PRESETS 12;

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
		new AudioParameterFloat("VOLUME",	"Volume", -32.0f, 8.0f, -12.0f),
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
		new AudioParameterFloat("VIBRATO_DEPTH", "Vibrato-Depth",  0.0f, 13.0f, 0.0f),
		new AudioParameterFloat("VIBRATO_SPEED", "Vibrato-Speed",  0.0f, 20.0f, 0.1000f),
		new AudioParameterFloat("VIBRATO_ATTACKTIME", "Vibrato-AttackTime",  0.0f, 15.0f, 0.0f)
	}
	, voicingParameters{
		new AudioParameterChoice("VOICING_TYPE", "Voicing-Type", {"POLY", "MONO", "PORTAMENTO"}, 0),
		new AudioParameterFloat("PORTAMENTO_TIME", "Portamento-Time",  0.0f, 3.0f, 0.0f)
	}
	, optionsParameters{
		new AudioParameterBool("IS_VELOCITY_SENSE", "Is-Velocity-Sense", true),
		new AudioParameterInt("PITCH_BEND_RANGE", "Pitch-Bend-Range", 1, 13, 2),
		new AudioParameterInt("PITCH_STANDARD", "Pitch-Standard", 400, 500, 440)
	}
	, waveformMemoryParameters()
	, scopeDataCollector(scopeDataQueue)
{
	chipOscParameters.addAllParameters(*this);
	sweepParameters.addAllParameters(*this);
	vibratoParameters.addAllParameters(*this);
	voicingParameters.addAllParameters(*this);
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
    return NUM_OF_PRESETS;
}

int SimpleSynthAudioProcessor::getCurrentProgram()
{
	return currentProgIndex;
}

void SimpleSynthAudioProcessor::setCurrentProgram (int index)
{
	initProgram();
	switch (index)
	{
	case 0:
		break;
	case 1:
		chipOscParameters.OscWaveType = new AudioParameterChoice("OSC_WAVE_TYPE", "Osc-WaveType", OSC_WAVE_TYPES, 2);
		chipOscParameters.Decay = new AudioParameterFloat("AMPENV_DECAY", "Decay", 0.000f, 10.0f, 5.000f);
		chipOscParameters.Sustain = new AudioParameterFloat("AMPENV_SUSTAIN", "Sustain", 0.000f, 1.0f, 0.0f);
		vibratoParameters.VibratoAmount = new AudioParameterFloat("VIBRATO_DEPTH", "Vibrato-Depth", 0.0f, 13.0f, 0.05f);
		vibratoParameters.VibratoSpeed = new AudioParameterFloat("VIBRATO_SPEED", "Vibrato-Speed", 0.0f, 20.0f, 13.0f);
		break;
	case 2:
		chipOscParameters.Attack = new AudioParameterFloat("AMPENV_ATTACK", "Attack", 0.000f, 10.0f, 1.000f);
		vibratoParameters.VibratoAmount = new AudioParameterFloat("VIBRATO_DEPTH", "Vibrato-Depth", 0.0f, 13.0f, 0.05f);
		vibratoParameters.VibratoSpeed = new AudioParameterFloat("VIBRATO_SPEED", "Vibrato-Speed", 0.0f, 20.0f, 5.0f);
		break;
	case 3:
		chipOscParameters.Decay = new AudioParameterFloat("AMPENV_DECAY", "Decay", 0.000f, 10.0f, 0.500f);
		chipOscParameters.Sustain = new AudioParameterFloat("AMPENV_SUSTAIN", "Sustain", 0.000f, 1.0f, 0.0f);
		break;
	case 4:
		chipOscParameters.OscWaveType = new AudioParameterChoice("OSC_WAVE_TYPE", "Osc-WaveType", OSC_WAVE_TYPES, 1);
		chipOscParameters.Decay = new AudioParameterFloat("AMPENV_DECAY", "Decay", 0.000f, 10.0f, 0.050f);
		chipOscParameters.Sustain = new AudioParameterFloat("AMPENV_SUSTAIN", "Sustain", 0.000f, 1.0f, 0.60f);
		vibratoParameters.VibratoAmount = new AudioParameterFloat("VIBRATO_DEPTH", "Vibrato-Depth", 0.0f, 13.0f, 0.05f);
		vibratoParameters.VibratoSpeed = new AudioParameterFloat("VIBRATO_SPEED", "Vibrato-Speed", 0.0f, 20.0f, 6.0f);
		break;
	case 5:
		vibratoParameters.VibratoAmount = new AudioParameterFloat("VIBRATO_DEPTH", "Vibrato-Depth", 0.0f, 13.0f, 1.0f);
		vibratoParameters.VibratoSpeed = new AudioParameterFloat("VIBRATO_SPEED", "Vibrato-Speed", 0.0f, 20.0f, 6.0f);
		vibratoParameters.VibratoAttackTime = new AudioParameterFloat("VIBRATO_ATTACKTIME", "Vibrato-AttackTime", 0.0f, 15.0f, 3.0f);
		break;
	case 6:
		chipOscParameters.Decay = new AudioParameterFloat("AMPENV_DECAY", "Decay", 0.000f, 10.0f, 0.300f);
		chipOscParameters.Sustain = new AudioParameterFloat("AMPENV_SUSTAIN", "Sustain", 0.000f, 1.0f, 0.5f);
		break;
	case 7:
		sweepParameters.SweepSwitch = new AudioParameterChoice("SWEEP_SWITCH", "Sweep-Switch", SWEEP_SWITCH, 1);
		sweepParameters.SweepTime = new AudioParameterFloat("SWEEP_TIME", "Sweep-Time", 0.01f, 10.0f, 3.0f);
		break;
	case 8:
		chipOscParameters.OscWaveType = new AudioParameterChoice("OSC_WAVE_TYPE", "Osc-WaveType", OSC_WAVE_TYPES, 10);
		chipOscParameters.Decay = new AudioParameterFloat("AMPENV_DECAY", "Decay", 0.000f, 10.0f, 0.200f);
		chipOscParameters.Sustain = new AudioParameterFloat("AMPENV_SUSTAIN", "Sustain", 0.000f, 1.0f, 0.0f);
		break;
	case 9:
		chipOscParameters.OscWaveType = new AudioParameterChoice("OSC_WAVE_TYPE", "Osc-WaveType", OSC_WAVE_TYPES, 10);
		chipOscParameters.Decay = new AudioParameterFloat("AMPENV_DECAY", "Decay", 0.000f, 10.0f, 0.800f);
		chipOscParameters.Sustain = new AudioParameterFloat("AMPENV_SUSTAIN", "Sustain", 0.000f, 1.0f, 0.0f);
		break;
	case 10:
		chipOscParameters.OscWaveType = new AudioParameterChoice("OSC_WAVE_TYPE", "Osc-WaveType", OSC_WAVE_TYPES, 10);
		chipOscParameters.Attack = new AudioParameterFloat("AMPENV_ATTACK", "Attack", 0.000f, 10.0f, 0.100f);
		chipOscParameters.Decay = new AudioParameterFloat("AMPENV_DECAY", "Decay", 0.000f, 10.0f, 2.500f);
		chipOscParameters.Sustain = new AudioParameterFloat("AMPENV_SUSTAIN", "Sustain", 0.000f, 1.0f, 0.0f);
		break;
	case 11:
		chipOscParameters.Decay = new AudioParameterFloat("AMPENV_DECAY", "Decay", 0.000f, 10.0f, 0.200f);
		chipOscParameters.Sustain = new AudioParameterFloat("AMPENV_SUSTAIN", "Sustain", 0.000f, 1.0f, 0.0f);
		sweepParameters.SweepSwitch = new AudioParameterChoice("SWEEP_SWITCH", "Sweep-Switch", SWEEP_SWITCH, 2);
		sweepParameters.SweepTime = new AudioParameterFloat("SWEEP_TIME", "Sweep-Time", 0.01f, 10.0f, 0.4f);
		break;
	default:
		break;
	}
	currentProgIndex = index;
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

void SimpleSynthAudioProcessor::initProgram() {

	chipOscParameters.OscWaveType = new AudioParameterChoice("OSC_WAVE_TYPE", "Osc-WaveType", OSC_WAVE_TYPES, 0);
	chipOscParameters.VolumeLevel = new AudioParameterFloat("VOLUME", "Volume", -32.0f, 8.0f, -12.0f);	
	chipOscParameters.Attack = new AudioParameterFloat("AMPENV_ATTACK", "Attack", 0.000f, 10.0f, 0.000f);
	chipOscParameters.Decay =  new AudioParameterFloat("AMPENV_DECAY", "Decay", 0.000f, 10.0f, 0.000f);
	chipOscParameters.Sustain = new AudioParameterFloat("AMPENV_SUSTAIN", "Sustain", 0.000f, 1.0f, 1.0f);
	chipOscParameters.Release = new AudioParameterFloat("AMPENV_RELEASE", "Release", 0.000f, 10.0f, 0.000f);

	sweepParameters.SweepSwitch = new AudioParameterChoice("SWEEP_SWITCH", "Sweep-Switch", SWEEP_SWITCH, 0);
	sweepParameters.SweepTime = new AudioParameterFloat("SWEEP_TIME", "Sweep-Time", 0.01f, 10.0f, 1.0f);

	vibratoParameters.VibratoEnable = new AudioParameterBool("VIBRATO_ENABLE", "Vibrato-Enable", true);
	vibratoParameters.VibratoAmount = new AudioParameterFloat("VIBRATO_DEPTH", "Vibrato-Depth", 0.0f, 13.0f, 0.0f);
	vibratoParameters.VibratoSpeed = new AudioParameterFloat("VIBRATO_SPEED", "Vibrato-Speed", 0.0f, 20.0f, 0.1000f);
	vibratoParameters.VibratoAttackTime = new AudioParameterFloat("VIBRATO_ATTACKTIME", "Vibrato-AttackTime", 0.0f, 15.0f, 0.0f);
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

	int numVoices = (voicingParameters.VoicingSwitch->getCurrentChoiceName() == "POLY") ? 16 : 1;

	for (int i = 0; i < numVoices; ++i)
	{
		synth.addVoice(new SimpleVoice(&chipOscParameters, &sweepParameters, &vibratoParameters, &voicingParameters, &optionsParameters, &waveformMemoryParameters));
	}

	spec.sampleRate = sampleRate;
	spec.numChannels = getTotalNumOutputChannels();
	spec.maximumBlockSize = samplesPerBlock;

	drive.prepare(spec);

	clipper.prepare(spec);
	clipper.functionToUse = clippingFunction;
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

	if ((voicingParameters.VoicingSwitch->getCurrentChoiceName() == "POLY") ? 16 : 1 != synth.getNumVoices())
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
	voicingParameters.saveParameters(*xml);
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
			voicingParameters.loadParameters(*xmlState);
			optionsParameters.loadParameters(*xmlState);
			waveformMemoryParameters.loadParameters(*xmlState);
		}
	}
}

void SimpleSynthAudioProcessor::changeVoiceSize()
{
	int voiceNum = (voicingParameters.VoicingSwitch->getCurrentChoiceName() == "POLY") ? 16 : 1;

	while (synth.getNumVoices() != voiceNum)
	{
		if (synth.getNumVoices() > voiceNum)
		{
			synth.removeVoice(synth.getNumVoices() - 1);
		}
		else
		{
			synth.addVoice(new SimpleVoice(&chipOscParameters, &sweepParameters, &vibratoParameters, &voicingParameters, &optionsParameters, &waveformMemoryParameters));
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
