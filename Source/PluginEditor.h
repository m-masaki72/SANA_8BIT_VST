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

class OscPage : public Component
{
public:
	OscPage(SimpleSynthAudioProcessor& p);
	~OscPage();
	void paint(Graphics& g) override;
	void resized() override;
	void setLookAndFeel(LookAndFeel* customLookAndFeel);

private:
	ScopeComponent<float> scopeComponent;
	ChipOscillatorComponent chipOscComponent;
	SweepParametersComponent sweepParamsComponent;
	VibratoParametersComponent vibratoParamsComponent;
	VoicingParametersComponent voicingParamsComponent;
	OptionsParametersComponent optionsParamsComponent;
	WaveformMemoryParametersComponent waveformMemoryParamsComponent;

	LookAndFeel* customLookAndFeel;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OscPage)
};

//==============================================================================

class EffectPage : public Component
{
public:
	EffectPage(SimpleSynthAudioProcessor& p);
	~EffectPage();
	void paint(Graphics& g) override;
	void resized() override;
	void setLookAndFeel(LookAndFeel* customLookAndFeel);

private:
	MidiEchoParametersComponent midiEchoParamsComponent;
	FilterParametersComponent filterParamsComponent;
	ScopeComponent<float> scopeComponent;

	LookAndFeel* customLookAndFeel;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EffectPage)
};

//==============================================================================

class SimpleSynthAudioProcessorEditor : public AudioProcessorEditor
{
public:
	SimpleSynthAudioProcessorEditor(SimpleSynthAudioProcessor& p);
    ~SimpleSynthAudioProcessorEditor();
    void paint (Graphics& g) override;
    void resized() override;

private:
	SimpleSynthAudioProcessor& processor;

	MidiKeyboardComponent keyboardComponent;
	OscPage p1;
	EffectPage p2;
	TabbedComponent tabs;

	LookAndFeel* customLookAndFeel;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SimpleSynthAudioProcessorEditor)
};


