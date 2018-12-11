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

class SimpleSynthAudioProcessorEditor : public AudioProcessorEditor, public Button::Listener
{
public:
	SimpleSynthAudioProcessorEditor(SimpleSynthAudioProcessor& p);
    ~SimpleSynthAudioProcessorEditor();
    void paint (Graphics& g) override;
    void resized() override;
	void buttonClicked(Button* button) override;

private:
	SimpleSynthAudioProcessor& processor;

	MidiKeyboardComponent keyboardComponent;

	TextButton OscButton;
	TextButton EffectButton;

	//Oscillator Page Component
	ScopeComponent<float> scopeComponent;
	ChipOscillatorComponent chipOscComponent;
	SweepParametersComponent sweepParamsComponent;
	VibratoParametersComponent vibratoParamsComponent;
	VoicingParametersComponent voicingParamsComponent;
	OptionsParametersComponent optionsParamsComponent;
	WaveformMemoryParametersComponent waveformMemoryParamsComponent;
	//Effects Page Component
	MidiEchoParametersComponent midiEchoParamsComponent;
	FilterParametersComponent filterParamsComponent;

	LookAndFeel* customLookAndFeel;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SimpleSynthAudioProcessorEditor)
};


