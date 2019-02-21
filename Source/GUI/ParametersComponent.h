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
#include "ComponentUtil.h"

class BaseComponent : public Component, private Timer
{
public:
	BaseComponent() 
	{
		startTimerHz(30);
	};
	virtual void paint(Graphics& g) = 0;
	virtual void resized() = 0;

private:
	virtual void timerCallback() = 0;
};

class ChipOscillatorComponent : public BaseComponent, ComboBox::Listener, Slider::Listener
{
public:
	ChipOscillatorComponent(ChipOscillatorParameters* oscParams);

	virtual void paint(Graphics& g) override;
	virtual void resized() override;

private:
	ChipOscillatorComponent();

	virtual void timerCallback() override;
	virtual void sliderValueChanged(Slider* slider) override;
	virtual void comboBoxChanged(ComboBox* comboBoxThatHasChanged) override;

	ChipOscillatorParameters* _oscParamsPtr;

	TextSelector waveTypeSelector;
	TextSlider volumeLevelSlider;
	TextSlider attackSlider;
	TextSlider decaySlider;
	TextSlider sustainSlider;
	TextSlider releaseSlider;
};

class SweepParametersComponent : public BaseComponent, Slider::Listener, ComboBox::Listener
{
public:
	SweepParametersComponent(SweepParameters* sweepParams);

	virtual void paint(Graphics& g) override;
	virtual void resized() override;

private:
	SweepParametersComponent();

	virtual void timerCallback() override;
	virtual void sliderValueChanged(Slider* slider) override;
	virtual void comboBoxChanged(ComboBox* comboBoxThatHasChanged) override;
	bool isEditable();

	SweepParameters* _sweepParamsPtr;

	TextSelector sweepSwitchSelector;
	TextSlider timeSlider;
};

class VibratoParametersComponent : public BaseComponent, Button::Listener, Slider::Listener
{
public:
	VibratoParametersComponent(VibratoParameters* vibratoParams);

	virtual void paint(Graphics& g) override;
	virtual void resized() override;

private:
	VibratoParametersComponent();

	virtual void timerCallback() override;
	virtual void sliderValueChanged(Slider* slider) override;
	virtual void buttonClicked(Button* button) override;
	bool isEditable();

	VibratoParameters* _vibratoParamsPtr;

	SwitchButton enableButton;
	TextSlider amountSlider;
	TextSlider speedSlider;
	TextSlider attackTimeSlider;

};

class VoicingParametersComponent : public BaseComponent, ComboBox::Listener, Slider::Listener
{
public:
	VoicingParametersComponent(VoicingParameters* voicingParams);

	virtual void paint(Graphics& g) override;
	virtual void resized() override;

private:
	VoicingParametersComponent();

	virtual void timerCallback() override;
	virtual void sliderValueChanged(Slider* slider) override;
	virtual void comboBoxChanged(ComboBox* comboBoxThatHasChanged) override;

	VoicingParameters* _voicingParamsPtr;

	TextSelector voicingTypeSelector;
	TextSlider portaTimeSlider;
};

class OptionsParametersComponent : public BaseComponent, Slider::Listener
{
public:
	OptionsParametersComponent(OptionsParameters* optionsParams);

	virtual void paint(Graphics& g) override;
	virtual void resized() override;

private:
	OptionsParametersComponent();

	virtual void timerCallback() override;
	virtual void sliderValueChanged(Slider* slider) override;
	
	OptionsParameters* _optionsParamsPtr;

	TextSliderIncDec pitchStandardSlider;
	TextSliderIncDec pitchBendRangeSlider;

};

class MidiEchoParametersComponent : public BaseComponent, Button::Listener, Slider::Listener
{
public:
	MidiEchoParametersComponent(MidiEchoParameters* midiEchoParams);

	virtual void paint(Graphics& g) override;
	virtual void resized() override;

private:
	MidiEchoParametersComponent();

	virtual void timerCallback() override;
	virtual void sliderValueChanged(Slider* slider) override;
	virtual void buttonClicked(Button* button) override;
	bool isEditable();

	MidiEchoParameters* _midiEchoParamsPtr;

	SwitchButton enableButton;
	TextSlider durationSlider;
	TextSlider repeatSlider;
	TextSlider volumeOffsetSlider;
};

<<<<<<< HEAD
class RangeSlider : public Component, private Timer
{
public:
	RangeSlider(WaveformMemoryParameters* waveformMemoryParams);
=======
class WaveformMemoryParametersComponent : public BaseComponent, Button::Listener, public FileDragAndDropTarget
{
public:
	WaveformMemoryParametersComponent(WaveformMemoryParameters* waveformMemoryParams);
>>>>>>> e3142f42cde34501752a4826bc26deed9a937194

	virtual void paint(Graphics& g) override;

private:
	virtual void timerCallback() override;
	virtual void updateValue();
	virtual void mouseDrag(const MouseEvent& e) override;
	virtual void mouseDown(const MouseEvent& e) override;
	virtual void mouseUp(const MouseEvent& e) override;

	Slider waveSampleSlider[32];
<<<<<<< HEAD
=======

	TextButton saveButton;
	TextButton loadButton;
>>>>>>> e3142f42cde34501752a4826bc26deed9a937194
	const int BUTTON_HEIGHT = 32;

	WaveformMemoryParameters* _waveformMemoryParamsPtr;

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

<<<<<<< HEAD
class WaveformMemoryParametersComponent : public Component, Button::Listener, public FileDragAndDropTarget
{
public:
	WaveformMemoryParametersComponent(WaveformMemoryParameters* waveformMemoryParams);
	virtual ~WaveformMemoryParametersComponent();

	virtual void paint(Graphics& g) override;
	virtual void resized() override;

private:
	WaveformMemoryParametersComponent();

	virtual void buttonClicked(Button* button) override;
	virtual bool isInterestedInFileDrag(const StringArray & files) override;
	virtual void fileDragEnter(const StringArray &files, int x, int y) override;
	virtual void fileDragMove(const StringArray &files, int x, int y)override;
	virtual void fileDragExit(const StringArray &files)override;
	virtual void filesDropped(const StringArray &files, int x, int y) override;

	WaveformMemoryParameters* _waveformMemoryParamsPtr;

	//Slider waveSampleSlider[32];
	RangeSlider waveRangeSlider;

	TextButton saveButton;
	TextButton loadButton;
	//File preFilePath = File::getSpecialLocation(File::userDesktopDirectory);
	const int BUTTON_HEIGHT = 32;
};

class FilterParametersComponent : public Component, Button::Listener, Slider::Listener, private Timer
=======
class FilterParametersComponent : public BaseComponent, Button::Listener, Slider::Listener
>>>>>>> e3142f42cde34501752a4826bc26deed9a937194
{
public:
	FilterParametersComponent(FilterParameters* filterParams);

	virtual void paint(Graphics& g) override;
	virtual void resized() override;

private:
	FilterParametersComponent();

	virtual void timerCallback() override;
	virtual void sliderValueChanged(Slider* slider) override;
	virtual void buttonClicked(Button* button) override;

	FilterParameters* _filterParamsPtr;

	SwitchButton hiCutSwitch;
	SwitchButton lowCutSwitch;

	TextSlider hicutFreqSlider;
	TextSlider lowcutFreqSlider;
};
