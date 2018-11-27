/*
  ==============================================================================

    ParametersComponent.cpp
    Created: 16 May 2018 1:55:55am
	Modified: 11 September 2018
	Author:  MasakiMori, COx2

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include "../DSP/SimpleSynthParameters.h"

class ChipOscillatorComponent : public Component, ComboBox::Listener, Slider::Listener, private Timer
{
public:
	ChipOscillatorComponent(ChipOscillatorParameters* oscParams);
	virtual ~ChipOscillatorComponent();

	virtual void paint(Graphics& g) override;
	virtual void resized() override;

private:
	ChipOscillatorComponent();

	virtual void timerCallback() override;
	virtual void sliderValueChanged(Slider* slider) override;
	virtual void comboBoxChanged(ComboBox* comboBoxThatHasChanged) override;

	ChipOscillatorParameters* _oscParamsPtr;

	ComboBox waveTypeSelector;
	Slider volumeLevelSlider;
	Slider attackSlider;
	Slider decaySlider;
	Slider sustainSlider;
	Slider releaseSlider;

	Label waveTypeSelectorLabel;
	Label volumeLevelLabel;
	Label attackLabel;
	Label decayLabel;
	Label sustainLabel;
	Label releaseLabel;
};

class SweepParametersComponent : public Component, Slider::Listener, ComboBox::Listener, private Timer
{
public:
	SweepParametersComponent(SweepParameters* sweepParams);
	virtual ~SweepParametersComponent();

	virtual void paint(Graphics& g) override;
	virtual void resized() override;

private:
	SweepParametersComponent();

	virtual void timerCallback() override;
	virtual void sliderValueChanged(Slider* slider) override;
	virtual void comboBoxChanged(ComboBox* comboBoxThatHasChanged) override;
	bool isEditable();

	SweepParameters* _sweepParamsPtr;

	ComboBox sweepSwitchSelector;
	Slider timeSlider;

	Label switchLabel;
	Label timeLabel;
};

class VibratoParametersComponent : public Component, Button::Listener, Slider::Listener, private Timer
{
public:
	VibratoParametersComponent(VibratoParameters* vibratoParams);
	virtual ~VibratoParametersComponent();

	virtual void paint(Graphics& g) override;
	virtual void resized() override;

private:
	VibratoParametersComponent();

	virtual void timerCallback() override;
	virtual void sliderValueChanged(Slider* slider) override;
	virtual void buttonClicked(Button* button) override;
	bool isEditable();

	VibratoParameters* _vibratoParamsPtr;

	ToggleButton enableButton;
	ComboBox targetSelector;
	ComboBox waveTypeSelector;
	Slider amountSlider;
	Slider speedSlider;
	Slider attackTimeSlider;

	Label targetLabel;
	Label waveTypeLabel;
	Label amountLabel;
	Label speedLabel;
	Label attackTimeLabel;
};

class VoicingParametersComponent : public Component, ComboBox::Listener, Slider::Listener, private Timer
{
public:
	VoicingParametersComponent(VoicingParameters* voicingParams);
	virtual ~VoicingParametersComponent();

	virtual void paint(Graphics& g) override;
	virtual void resized() override;

private:
	VoicingParametersComponent();

	virtual void timerCallback() override;
	virtual void sliderValueChanged(Slider* slider) override;
	virtual void comboBoxChanged(ComboBox* comboBoxThatHasChanged) override;

	VoicingParameters* _voicingParamsPtr;

	ComboBox voicingTypeSelector;
	Slider portaTimeSlider;

	Label voicingTypeSelectorLabel;
	Label portaTimeLabel;
};

class OptionsParametersComponent : public Component, Button::Listener, Slider::Listener, private Timer
{
public:
	OptionsParametersComponent(OptionsParameters* optionsParams);
	virtual ~OptionsParametersComponent();

	virtual void paint(Graphics& g) override;
	virtual void resized() override;

private:
	OptionsParametersComponent();

	virtual void timerCallback() override;
	virtual void buttonClicked(Button* button) override;
	virtual void sliderValueChanged(Slider* slider) override;
	
	OptionsParameters* _optionsParamsPtr;

	ToggleButton velocitySenseButton;
	Slider pitchStandardSlider;
	Slider pitchBendRangeSlider;

	Label pitchStandardLabel;
	Label pitchBendRangeLabel;
};

class MidiEchoParametersComponent : public Component, Button::Listener, Slider::Listener, private Timer
{
public:
	MidiEchoParametersComponent(MidiEchoParameters* midiEchoParams);
	virtual ~MidiEchoParametersComponent();

	virtual void paint(Graphics& g) override;
	virtual void resized() override;

private:
	MidiEchoParametersComponent();

	virtual void timerCallback() override;
	virtual void sliderValueChanged(Slider* slider) override;
	virtual void buttonClicked(Button* button) override;
	bool isEditable();

	MidiEchoParameters* _midiEchoParamsPtr;

	ToggleButton enableButton;
	Slider durationSlider;
	Slider repeatSlider;
	Slider volumeOffsetSlider;

	Label durationLabel;
	Label repeatLabel;
	Label volumeOffsetLabel;
};

class WaveformMemoryParametersComponent : public Component, Button::Listener, private Timer, public FileDragAndDropTarget
{
public:
	WaveformMemoryParametersComponent(WaveformMemoryParameters* waveformMemoryParams);
	virtual ~WaveformMemoryParametersComponent();

	virtual void paint(Graphics& g) override;
	virtual void resized() override;

private:
	WaveformMemoryParametersComponent();

	virtual void timerCallback() override;
	virtual void updateValue();
	virtual void mouseDrag(const MouseEvent& e) override;
	virtual void mouseDown(const MouseEvent& e) override;
	virtual void mouseUp(const MouseEvent& e) override;
	virtual void buttonClicked(Button* button) override;
	virtual bool isInterestedInFileDrag(const StringArray & files) override;
	virtual void fileDragEnter(const StringArray &files, int x, int y) override;
	virtual void fileDragMove(const StringArray &files, int x, int y)override;
	virtual void fileDragExit(const StringArray &files)override;
	virtual void filesDropped(const StringArray &files, int x, int y) override;

	WaveformMemoryParameters* _waveformMemoryParamsPtr;

	Slider waveSampleSlider[32];

	TextButton saveButton;
	TextButton loadButton;
	//File preFilePath = File::getSpecialLocation(File::userDesktopDirectory);
	const int BUTTON_HEIGHT = 32;

	//=====================================================================================

	struct Trail
	{
		Trail(const MouseInputSource& ms)
			: source(ms)
		{}

		void pushPoint(Point<float> newPoint, ModifierKeys newMods, float pressure)
		{
			currentPosition = newPoint;
			modifierKeys = newMods;

			if (lastPoint.getDistanceFrom(newPoint) > 5.0f)
			{
				if (lastPoint != Point<float>())
				{
					Path newSegment;
					newSegment.startNewSubPath(lastPoint);
					newSegment.lineTo(newPoint);

					auto diameter = 20.0f * (pressure > 0 && pressure < 1.0f ? pressure : 1.0f);

					PathStrokeType(diameter, PathStrokeType::curved, PathStrokeType::rounded).createStrokedPath(newSegment, newSegment);
					path.addPath(newSegment);
				}

				lastPoint = newPoint;
			}
		}

		MouseInputSource source;
		Path path;
		Colour colour{ Colours::rebeccapurple.withAlpha(0.6f) };
		Point<float> lastPoint, currentPosition;
		ModifierKeys modifierKeys;

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Trail)
	};

	OwnedArray<Trail> trails;

	Trail* getTrail(const MouseInputSource& source)
	{
		for (auto* trail : trails)
		{
			if (trail->source == source)
				return trail;
		}

		return nullptr;
	}

	//=====================================================================================
};

class FilterParametersComponent : public Component, Button::Listener, Slider::Listener, private Timer
{
public:
	FilterParametersComponent(FilterParameters* filterParams);
	virtual ~FilterParametersComponent();

	virtual void paint(Graphics& g) override;
	virtual void resized() override;

private:
	FilterParametersComponent();

	virtual void timerCallback() override;
	virtual void sliderValueChanged(Slider* slider) override;
	virtual void buttonClicked(Button* button) override;

	FilterParameters* _filterParamsPtr;

	ToggleButton hiCutSwitch;
	ToggleButton lowCutSwitch;
	Slider hicutFreqSlider;
	Slider lowcutFreqSlider;

	Label hicutFreqLabel;
	Label lowcutFreqLabel;
};
