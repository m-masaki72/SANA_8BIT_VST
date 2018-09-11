/*
==============================================================================

	PluginEditor.cpp
	Created: 16 May 2018 1:55:55am
	Modified: 11 September 2018
	Author:  MasakiMori, COx2
	ChangeLog:
	Modified some components

==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace {
	const float KEY_HEIGHT = 80.0f;
	const float KEY_WIDTH = 32.0f;
	const float KEY_SCROLL_WIDTH = 32.0f;
	const int PANEL_MARGIN = 4;
}

SimpleSynthAudioProcessorEditor::SimpleSynthAudioProcessorEditor(SimpleSynthAudioProcessor& p)
	: AudioProcessorEditor(&p), processor(p)
	, keyboardComponent(p.getKeyboardState(), MidiKeyboardComponent::Orientation::horizontalKeyboard)
	, chipOscComponent(&p.chipOscParameters)
	, sweepParamsComponent(&p.sweepParameters)
	, vibratoParamsComponent(&p.vibratoParameters)
	, optionsParamsComponent(&p.optionsParameters)
	, waveformMemoryParamsComponent(&p.waveformMemoryParameters)
	, scopeComponent(p.getAudioBufferQueue())
{
	keyboardComponent.setKeyWidth(KEY_WIDTH);
	keyboardComponent.setScrollButtonWidth(KEY_SCROLL_WIDTH);

	addAndMakeVisible(keyboardComponent);
	addAndMakeVisible(chipOscComponent);
	addAndMakeVisible(sweepParamsComponent);
	addAndMakeVisible(vibratoParamsComponent);
	addAndMakeVisible(optionsParamsComponent);
	addAndMakeVisible(waveformMemoryParamsComponent);
	addAndMakeVisible(scopeComponent);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
	setSize (960, 540 + KEY_HEIGHT);

	// assign custom look and feel.
	{
		customLookAndFeel = new LookAndFeel_V4(LookAndFeel_V4::getLightColourScheme());

		customLookAndFeel->setColour(Slider::ColourIds::trackColourId, Colours::lime);
		customLookAndFeel->setColour(Slider::ColourIds::thumbColourId, Colours::darkslategrey);
		customLookAndFeel->setColour(Slider::ColourIds::backgroundColourId, Colours::lightgrey);

		customLookAndFeel->setColour(ToggleButton::ColourIds::textColourId, Colours::black);
		customLookAndFeel->setColour(ToggleButton::ColourIds::tickColourId, Colours::black);
		customLookAndFeel->setColour(ToggleButton::ColourIds::tickDisabledColourId, Colours::black);

		customLookAndFeel->setColour(Slider::ColourIds::textBoxBackgroundColourId, Colours::white);
		customLookAndFeel->setColour(Slider::ColourIds::textBoxTextColourId, Colours::black);

		customLookAndFeel->setColour(Label::textColourId, Colours::black);

		for (Component* child : getChildren()) {
			child->setLookAndFeel(customLookAndFeel);
		}
	}
}

SimpleSynthAudioProcessorEditor::~SimpleSynthAudioProcessorEditor()
{
	for (Component* child : getChildren()) {
		child->setLookAndFeel(nullptr);
	}

	delete customLookAndFeel;
}

void SimpleSynthAudioProcessorEditor::paint (Graphics& g)
{
	g.fillAll(Colours::rebeccapurple.darker().darker());
}

void SimpleSynthAudioProcessorEditor::resized()
{
	Rectangle<int> bounds = getLocalBounds();
	keyboardComponent.setBounds(bounds.removeFromBottom(KEY_HEIGHT));
	
	{
		Rectangle<int> leftArea = bounds.removeFromLeft(bounds.getWidth() * 0.45);
		chipOscComponent.setBounds(leftArea.removeFromTop(leftArea.getHeight() * 0.5).reduced(PANEL_MARGIN));
		scopeComponent.setBounds(leftArea.reduced(PANEL_MARGIN));
	}
	{
		Rectangle<int> rightArea = bounds;
		int HEIGHT = rightArea.getHeight();
		waveformMemoryParamsComponent.setBounds(rightArea.removeFromTop(HEIGHT * 0.45).reduced(PANEL_MARGIN));
		{
			Rectangle<int> area = rightArea.removeFromTop(HEIGHT * 0.33);
			sweepParamsComponent.setBounds(area.removeFromLeft(area.getWidth() * .5).reduced(PANEL_MARGIN));
			vibratoParamsComponent.setBounds(area.reduced(PANEL_MARGIN));
		}
		optionsParamsComponent.setBounds(rightArea.reduced(PANEL_MARGIN));
	}

	/*
	{
		Rectangle<int> upperArea = bounds.removeFromTop(bounds.getHeight() * 0.5);
		chipOscComponent.setBounds(upperArea.removeFromLeft(bounds.getWidth() * 0.5).reduced(PANEL_MARGIN));
		waveMemoryParamsComponent.setBounds(upperArea.reduced(PANEL_MARGIN));

	}

	{
		Rectangle<int> lowerArea = bounds;
		scopeComponent.setBounds(lowerArea.removeFromLeft(bounds.getWidth() * 0.35).reduced(PANEL_MARGIN));

		Rectangle<int> restArea = lowerArea.removeFromLeft(bounds.getWidth() * 0.35);
		sweepParamsComponent.setBounds(restArea.removeFromTop(restArea.getHeight() * 0.5).reduced(PANEL_MARGIN));
		vibratoParamsComponent.setBounds(restArea.reduced(PANEL_MARGIN));

		optionsParamsComponent.setBounds(lowerArea.reduced(PANEL_MARGIN));
	}
	*/
}