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
	const float KEY_HEIGHT = 80.0f;
	const float KEY_WIDTH = 32.0f;
	const float KEY_SCROLL_WIDTH = 32.0f;
	const int PANEL_MARGIN = 3;
	const float TAB_HEIGHT = 40.0f;

	const Colour BACKGROUND_COLOUR() { return Colours::rebeccapurple.darker().darker(); }
	const Colour TAB_BACKGROUND_COLOUR() { return Colours::rebeccapurple.darker(); }
}

//----------------------------------------------------------------------------------------------------

OscPage::OscPage(SimpleSynthAudioProcessor& p, LookAndFeel* customLookAndFeel)
	: chipOscComponent(&p.chipOscParameters)
	, sweepParamsComponent(&p.sweepParameters)
	, vibratoParamsComponent(&p.vibratoParameters)
	, voicingParamsComponent(&p.voicingParameters)
	, optionsParamsComponent(&p.optionsParameters)
	, waveformMemoryParamsComponent(&p.waveformMemoryParameters)
	, scopeComponent(p.getAudioBufferQueue())
{
	addAndMakeVisible(chipOscComponent);
	addAndMakeVisible(sweepParamsComponent);
	addAndMakeVisible(vibratoParamsComponent);
	addAndMakeVisible(voicingParamsComponent);
	addAndMakeVisible(optionsParamsComponent);
	addAndMakeVisible(waveformMemoryParamsComponent);
	addAndMakeVisible(scopeComponent);

	for (Component* child : getChildren()) 
	{
		child->setLookAndFeel(customLookAndFeel);
	}

}

OscPage::~OscPage()
{}

void OscPage::paint(Graphics& g)
{
	g.fillAll(BACKGROUND_COLOUR());
}

void OscPage::resized()
{
	Rectangle<int> bounds = getLocalBounds();
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
		{
			Rectangle<int> area = rightArea.reduced(PANEL_MARGIN);
			voicingParamsComponent.setBounds(area.removeFromLeft(area.getWidth() * .5).reduced(PANEL_MARGIN));
			optionsParamsComponent.setBounds(area.reduced(PANEL_MARGIN));
		}
	}
}

//----------------------------------------------------------------------------------------------------

EffectPage::EffectPage(SimpleSynthAudioProcessor& p, LookAndFeel* customLookAndFeel)
	: scopeComponent(p.getAudioBufferQueue())
{
	addAndMakeVisible(scopeComponent);

	for (Component* child : getChildren()) 
	{
		child->setLookAndFeel(customLookAndFeel);
	}
}

EffectPage::~EffectPage()
{}

void EffectPage::paint(Graphics& g)
{
	g.fillAll(BACKGROUND_COLOUR());
}

void EffectPage::resized()
{
	Rectangle<int> bounds = getLocalBounds();

	{
		Rectangle<int> leftArea = bounds.removeFromLeft(bounds.getWidth() * 0.45);
		leftArea.removeFromTop(leftArea.getHeight() * 0.5).reduced(PANEL_MARGIN);
		scopeComponent.setBounds(leftArea.reduced(PANEL_MARGIN));
	}
}

//----------------------------------------------------------------------------------------------------

SimpleSynthAudioProcessorEditor::SimpleSynthAudioProcessorEditor(SimpleSynthAudioProcessor& p)
	: AudioProcessorEditor(&p), processor(p)
	, keyboardComponent(p.getKeyboardState(), MidiKeyboardComponent::Orientation::horizontalKeyboard)
	, tabs(TabbedButtonBar::TabsAtTop)
	, p1(p, customLookAndFeel)
	, p2(p, customLookAndFeel)
{
	addAndMakeVisible(keyboardComponent);
	addAndMakeVisible(p1);
	addAndMakeVisible(p2);
	addAndMakeVisible(tabs);

	keyboardComponent.setKeyWidth(KEY_WIDTH);
	keyboardComponent.setScrollButtonWidth(KEY_SCROLL_WIDTH);

	tabs.addTab("OSCILLATOR", BACKGROUND_COLOUR(), &p1, false);
	tabs.addTab("EFFECTS", BACKGROUND_COLOUR(), &p2, false);

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

		for (Component* child : getChildren()) 
		{
			child->setLookAndFeel(customLookAndFeel);
		}

		//Set Tabs Style
		{
			//tabs.setOutline(5.0f);
			tabs.setTabBarDepth(TAB_HEIGHT);
			tabs.setIndent(5.0f);
		}
	}

	//サイズ宣言はコンストラクタなどが終了したあとに呼ぶ
	setSize(960, 540 + KEY_HEIGHT);
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
}

void SimpleSynthAudioProcessorEditor::resized()
{
	Rectangle<int> bounds = getLocalBounds();
	keyboardComponent.setBounds(bounds.removeFromBottom(KEY_HEIGHT));
	tabs.setBounds(bounds);
}