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

namespace {
	const int WAVESAMPLE_LENGTH = 32;
}

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

	SweepParameters* _sweepParamsPtr;

	ComboBox sweepSwitchSelector;

	Slider timeSlider;

	Label switchLabel;
	Label timeLabel;
};

class VibratoParametersComponent : public Component, Button::Listener, Slider::Listener, ComboBox::Listener, private Timer
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
	virtual void comboBoxChanged(ComboBox* comboBoxThatHasChanged) override;
	virtual void buttonClicked(Button* button) override;

	VibratoParameters* _vibratoParamsPtr;

	ToggleButton enableButton;

	ComboBox targetSelector;
	ComboBox waveTypeSelector;

	Slider amountSlider;
	Slider speedSlider;

	Label targetLabel;
	Label waveTypeLabel;
	Label amountLabel;
	Label speedLabel;
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
	virtual void sliderValueChanged(Slider * slider) override;
	
	OptionsParameters* _optionsParamsPtr;

	ToggleButton polyModeButton;
	ToggleButton velocitySenseButton;
	Slider pitchStandardSlider;
	Slider pitchBendRangeSlider;

	Label pitchStandardLabel;
	Label pitchBendRangeLabel;
};

class WaveformMemoryParametersComponent : public Component, private Timer
{
public:
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

	WaveformMemoryParameters* _waveformMemoryParamsPtr;

	Slider waveSampleSlider[WAVESAMPLE_LENGTH];
};