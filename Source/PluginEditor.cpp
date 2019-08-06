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

namespace 
{
	const std::int32_t KEY_HEIGHT = 80;
	const float KEY_WIDTH = 32.0f;
	const float KEY_SCROLL_WIDTH = 32.0f;
	const std::int32_t PANEL_MARGIN = 3;
	const float TAB_HEIGHT = 40.0f;

	const Colour BACKGROUND_COLOUR() { return Colours::rebeccapurple.darker().darker(); }
	const Colour TAB_BACKGROUND_COLOUR() { return Colours::rebeccapurple.darker(); }
}

SimpleSynthAudioProcessorEditor::SimpleSynthAudioProcessorEditor(SimpleSynthAudioProcessor& p)
	: AudioProcessorEditor(&p), processor(p)
	, keyboardComponent(p.getKeyboardState(), MidiKeyboardComponent::Orientation::horizontalKeyboard)
	, OscButton("Oscilator", this), EffectButton("Effects", this)
	, chipOscComponent(&p.chipOscParameters)
	, sweepParamsComponent(&p.sweepParameters)
	, vibratoParamsComponent(&p.vibratoParameters)
	, voicingParamsComponent(&p.voicingParameters)
	, optionsParamsComponent(&p.optionsParameters)
	, waveformMemoryParamsComponent(&p.waveformMemoryParameters)
	, midiEchoParamsComponent(&p.midiEchoParameters)
	, filterParamsComponent(&p.filterParameters)
	, scopeComponent(p.getAudioBufferQueue())
{
	/*
		TabComponentを使いたかったが，Tabだとメモリリークが収まらないので現在の形に．
		デストラクタ時にメモリリーク，CustomLookAndFeelまわりでエラーが取れない．
		安定化のためにアルファ切り替えによるGUI管理方式に変更
	*/

	{
		addAndMakeVisible(keyboardComponent);
		keyboardComponent.setKeyWidth(KEY_WIDTH);
		keyboardComponent.setScrollButtonWidth(KEY_SCROLL_WIDTH);

		addAndMakeVisible(OscButton);
		addAndMakeVisible(EffectButton);
		OscButton.setToggleState(true);
		EffectButton.setToggleState(false);
	}
	{
		addAndMakeVisible(chipOscComponent);
		addAndMakeVisible(sweepParamsComponent);
		addAndMakeVisible(vibratoParamsComponent);
		addAndMakeVisible(voicingParamsComponent);
		addAndMakeVisible(optionsParamsComponent);
		addAndMakeVisible(waveformMemoryParamsComponent);
		addAndMakeVisible(scopeComponent);
	}
	{
		addAndMakeVisible(midiEchoParamsComponent);
		addAndMakeVisible(filterParamsComponent);
		addAndMakeVisible(scopeComponent);
	}

	setSize(960, 540 + KEY_HEIGHT);

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

		for (Component* child : AudioProcessorEditor::getChildren())
		{
			child->setLookAndFeel(customLookAndFeel);
		}
	}

	//init each GUI Alpha 
	{
		chipOscComponent.setVisible(true);
		sweepParamsComponent.setVisible(true);
		vibratoParamsComponent.setVisible(true);
		voicingParamsComponent.setVisible(true);
		optionsParamsComponent.setVisible(true);
		waveformMemoryParamsComponent.setVisible(true);
		midiEchoParamsComponent.setVisible(false);
		filterParamsComponent.setVisible(false);
	}
}

SimpleSynthAudioProcessorEditor::~SimpleSynthAudioProcessorEditor()
{
	for (Component* child : getChildren())
	{
		child->setLookAndFeel(nullptr);
	}

	delete customLookAndFeel;
}

void SimpleSynthAudioProcessorEditor::paint (Graphics& g)
{
	g.fillAll(TAB_BACKGROUND_COLOUR());

	g.setColour(Colours::white);
	g.setFont(32.0f);
	//g.drawFittedText("BPM: " + std::to_string(processor.currentPositionInfo.bpm), getLocalBounds(), Justification::topRight, 1);
	g.drawFittedText("Version 1.50", AudioProcessorEditor::getLocalBounds(), Justification::topRight, 1);
}

void SimpleSynthAudioProcessorEditor::resized()
{
	Rectangle<int> bounds = getLocalBounds();

	keyboardComponent.setBounds(bounds.removeFromBottom(KEY_HEIGHT));

	{
		Rectangle<int> area = bounds.removeFromTop(40);
		OscButton.setBounds(area.removeFromLeft(80));
		EffectButton.setBounds(area.removeFromLeft(80));
	}

	//Oscillator Page
	{
		Rectangle<int> mainbounds = bounds;
		{
			Rectangle<int> leftArea = mainbounds.removeFromLeft(bounds.getWidth() * 0.45f);
			chipOscComponent.setBounds(leftArea.removeFromTop(leftArea.getHeight() * 0.5f).reduced(PANEL_MARGIN));
			scopeComponent.setBounds(leftArea.reduced(PANEL_MARGIN));
		}
		{
			Rectangle<int> rightArea = mainbounds;
			auto HEIGHT = rightArea.getHeight();
			waveformMemoryParamsComponent.setBounds(rightArea.removeFromTop(HEIGHT * 0.45f).reduced(PANEL_MARGIN));
			{
				Rectangle<int> area = rightArea.removeFromTop(HEIGHT * 0.33f);
				sweepParamsComponent.setBounds(area.removeFromLeft(area.getWidth() * 0.5f).reduced(PANEL_MARGIN));
				vibratoParamsComponent.setBounds(area.reduced(PANEL_MARGIN));
			}
			{
				Rectangle<int> area = rightArea.reduced(PANEL_MARGIN);
				voicingParamsComponent.setBounds(area.removeFromLeft(area.getWidth() * 0.5f).reduced(PANEL_MARGIN));
				optionsParamsComponent.setBounds(area.reduced(PANEL_MARGIN));
			}
		}
	}

	//EffectPage
	{
		Rectangle<int> mainbounds = bounds;
		{
			Rectangle<int> leftArea = mainbounds.removeFromLeft(bounds.getWidth() * 0.45f);
			midiEchoParamsComponent.setBounds(leftArea.removeFromTop(leftArea.getHeight() * 0.5f).reduced(PANEL_MARGIN));
			scopeComponent.setBounds(leftArea.reduced(PANEL_MARGIN));
		}
		{
			Rectangle<int> rightArea = mainbounds;
			filterParamsComponent.setBounds(rightArea.removeFromTop(rightArea.getHeight() * 0.5f).reduced(PANEL_MARGIN));
		}
	}
}

void SimpleSynthAudioProcessorEditor::buttonClicked(Button* button)
{
	{
		chipOscComponent.setVisible(false);
		sweepParamsComponent.setVisible(false);
		vibratoParamsComponent.setVisible(false);
		voicingParamsComponent.setVisible(false);
		optionsParamsComponent.setVisible(false);
		waveformMemoryParamsComponent.setVisible(false);
		midiEchoParamsComponent.setVisible(false);
		filterParamsComponent.setVisible(false);

		OscButton.setToggleState(false);
		EffectButton.setToggleState(false);
	}

	if (button == &OscButton.button)
	{
		chipOscComponent.setVisible(true);
		sweepParamsComponent.setVisible(true);
		vibratoParamsComponent.setVisible(true);
		voicingParamsComponent.setVisible(true);
		optionsParamsComponent.setVisible(true);
		waveformMemoryParamsComponent.setVisible(true);

		OscButton.setToggleState(true);
	}
	else if (button == &EffectButton.button)
	{
		midiEchoParamsComponent.setVisible(true);
		filterParamsComponent.setVisible(true);

		EffectButton.setToggleState(true);
	}
	resized();
}