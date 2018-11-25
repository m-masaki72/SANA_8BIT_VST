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
	const int KEY_HEIGHT = 80;
	const float KEY_WIDTH = 32.0f;
	const float KEY_SCROLL_WIDTH = 32.0f;
	const int PANEL_MARGIN = 3;
	const float TAB_HEIGHT = 40.0f;

	const Colour BACKGROUND_COLOUR() { return Colours::rebeccapurple.darker().darker(); }
	const Colour TAB_BACKGROUND_COLOUR() { return Colours::rebeccapurple.darker(); }
}

//----------------------------------------------------------------------------------------------------

OscPage::OscPage(SimpleSynthAudioProcessor& p)
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
		Rectangle<int> leftArea = bounds.removeFromLeft(bounds.getWidth() * 0.45f);
		chipOscComponent.setBounds(leftArea.removeFromTop(leftArea.getHeight() * 0.5f).reduced(PANEL_MARGIN));
		scopeComponent.setBounds(leftArea.reduced(PANEL_MARGIN));
	}
	{
		Rectangle<int> rightArea = bounds;
		int HEIGHT = rightArea.getHeight();
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

void OscPage::setLookAndFeel(LookAndFeel* customLookAndFeel)
{
	for (Component* child : getChildren())
	{
		child->setLookAndFeel(customLookAndFeel);
	}
}

//----------------------------------------------------------------------------------------------------

EffectPage::EffectPage(SimpleSynthAudioProcessor& p)
	: midiEchoParamsComponent(&p.midiEchoParameters)
	, filterParamsComponent(&p.filterParameters)
	, scopeComponent(p.getAudioBufferQueue())
{
	addAndMakeVisible(midiEchoParamsComponent);
	addAndMakeVisible(filterParamsComponent);
	addAndMakeVisible(scopeComponent);
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
		Rectangle<int> leftArea = bounds.removeFromLeft(bounds.getWidth() * 0.45f);
		midiEchoParamsComponent.setBounds(leftArea.removeFromTop(leftArea.getHeight() * 0.5f).reduced(PANEL_MARGIN));
		scopeComponent.setBounds(leftArea.reduced(PANEL_MARGIN));
	}
	{
		Rectangle<int> rightArea = bounds;
		filterParamsComponent.setBounds(rightArea.removeFromTop(rightArea.getHeight() * 0.5f).reduced(PANEL_MARGIN));
	}
}

void EffectPage::setLookAndFeel(LookAndFeel* customLookAndFeel)
{
	for (Component* child : getChildren())
	{
		child->setLookAndFeel(customLookAndFeel);
	}
}

//----------------------------------------------------------------------------------------------------

SimpleSynthAudioProcessorEditor::SimpleSynthAudioProcessorEditor(SimpleSynthAudioProcessor& p)
	: AudioProcessorEditor(&p), processor(p)
	, keyboardComponent(p.getKeyboardState(), MidiKeyboardComponent::Orientation::horizontalKeyboard)
	, tabs(TabbedButtonBar::TabsAtTop)
	, p1(p)
	, p2(p)
{
	/*
	順番に注意すること
	以下順番を守らないと，DAWによって表示されたりされなかったりする
	
	1. コンポーネントのコンストラクタ
	2. メインのAddAndMakeVisible
	3. setSize()
	4. LookAndFeelの更新
	*/
	
	addAndMakeVisible(keyboardComponent);
	addAndMakeVisible(p1);
	addAndMakeVisible(p2);
	addAndMakeVisible(tabs);

	keyboardComponent.setKeyWidth(KEY_WIDTH);
	keyboardComponent.setScrollButtonWidth(KEY_SCROLL_WIDTH);

	tabs.addTab("OSCILLATOR", BACKGROUND_COLOUR(), &p1, false);
	tabs.addTab("EFFECTS", BACKGROUND_COLOUR(), &p2, false);

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

		for (Component* child : getChildren()) 
		{
			child->setLookAndFeel(customLookAndFeel);
		}
		p1.setLookAndFeel(customLookAndFeel);
		p2.setLookAndFeel(customLookAndFeel);

		//Set Tabs Style
		{
			//tabs.setTabBarDepth(TAB_HEIGHT);
			//tabs.setIndent(5.0f);
		}
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

	//g.setColour(Colours::white);
	//g.setFont(32.0f);
	//g.drawFittedText("BPM: " + std::to_string(processor.currentPositionInfo.bpm), getLocalBounds(), Justification::topRight, 1);
}

void SimpleSynthAudioProcessorEditor::resized()
{
	Rectangle<int> bounds = getLocalBounds();

	keyboardComponent.setBounds(bounds.removeFromBottom(KEY_HEIGHT));
	tabs.setBounds(bounds);
}