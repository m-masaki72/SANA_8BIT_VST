#include "BaseAudioProcessor.h"

BaseAudioProcessor::BaseAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
  : AudioProcessor(BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
    .withInput("Input", AudioChannelSet::stereo(), true)
#endif
    .withOutput("Output", AudioChannelSet::stereo(), true)
#endif
  )
#endif
{}

BaseAudioProcessor::~BaseAudioProcessor() {}

#ifndef JucePlugin_PreferredChannelConfigurations
bool BaseAudioProcessor::isBusesLayoutSupported(
  const BusesLayout& layouts) const {
#if JucePlugin_IsMidiEffect
  ignoreUnused(layouts);
  return true;
#else
  // This is the place where you check if the layout is supported.
  // In this template code we only support mono or stereo.
  if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono() &&
    layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
    return false;

  // This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
  if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
    return false;
#endif

  return true;
#endif
}
#endif

bool BaseAudioProcessor::hasEditor() const {
  return true;  // (change this to false if you choose to not supply an editor)
}

const String BaseAudioProcessor::getName() const {
  return JucePlugin_Name;
}

bool BaseAudioProcessor::acceptsMidi() const {
#if JucePlugin_WantsMidiInput
  return true;
#else
  return false;
#endif
}

bool BaseAudioProcessor::producesMidi() const {
#if JucePlugin_ProducesMidiOutput
  return true;
#else
  return false;
#endif
}

bool BaseAudioProcessor::isMidiEffect() const {
#if JucePlugin_IsMidiEffect
  return true;
#else
  return false;
#endif
}

double BaseAudioProcessor::getTailLengthSeconds() const {
  return 0.0;
}

void BaseAudioProcessor::changeProgramName(int index, const String& newName) {
}

void BaseAudioProcessor::releaseResources() {
  // When playback stops, you can use this as an opportunity to free up any
  // spare memory, etc.
}