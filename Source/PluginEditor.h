/*
==============================================================================

	PluginEditor.h
	Created: 16 May 2018 1:55:55am
	Modified: 11 September 2018
	Author:  MasakiMori, COx2
	ChangeLog:
	Modified some components

==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"

#include "GUI/ParametersComponent.h"
#include "GUI/ScopeComponent.h"

//==============================================================================
/**
*/
class SimpleSynthAudioProcessorEditor  : public AudioProcessorEditor
{
public:
    SimpleSynthAudioProcessorEditor (SimpleSynthAudioProcessor&);
    ~SimpleSynthAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
	SimpleSynthAudioProcessor & processor;

	MidiKeyboardComponent keyboardComponent;
	ScopeComponent<float> scopeComponent;
	ChipOscillatorComponent chipOscComponent;
	SweepParametersComponent sweepParamsComponent;
	VibratoParametersComponent vibratoParamsComponent;
	OptionsParametersComponent optionsParamsComponent;
	WaveformMemoryParametersComponent waveformMemoryParamsComponent;

	LookAndFeel* customLookAndFeel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleSynthAudioProcessorEditor)
};