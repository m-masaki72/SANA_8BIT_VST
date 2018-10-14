/*
  ==============================================================================

    ParametersComponent.cpp
    Created: 16 May 2018 1:55:55am
	Modified: 11 September 2018
	Author:  MasakiMori, COx2

  ==============================================================================
*/

#include "ParametersComponent.h"

namespace {
	const Colour PANEL_COLOUR() { return Colours::cornsilk; }
	const Colour HEADER_COLOUR() { return Colours::darkorange; }
	const Colour FONT_COLOUR() { return Colours::black; }

	const float PANEL_NAME_FONT_SIZE = 24.0f;
	const float PARAM_LABEL_FONT_SIZE = 18.0f;
	const int PANEL_NAME_HEIGHT = 32;
	const int LOCAL_MARGIN = 2;
}

ChipOscillatorComponent::ChipOscillatorComponent(ChipOscillatorParameters* oscParams)
	: _oscParamsPtr(oscParams)
	, waveTypeSelector("waveForm")
	, volumeLevelSlider(Slider::SliderStyle::LinearHorizontal, Slider::TextEntryBoxPosition::TextBoxLeft)
	, attackSlider(Slider::SliderStyle::LinearHorizontal, Slider::TextEntryBoxPosition::TextBoxLeft)
	, decaySlider(Slider::SliderStyle::LinearHorizontal, Slider::TextEntryBoxPosition::TextBoxLeft)
	, sustainSlider(Slider::SliderStyle::LinearHorizontal, Slider::TextEntryBoxPosition::TextBoxLeft)
	, releaseSlider(Slider::SliderStyle::LinearHorizontal, Slider::TextEntryBoxPosition::TextBoxLeft)
{
	waveTypeSelector.addItemList(_oscParamsPtr->OscWaveType->getAllValueStrings(), 1);
	waveTypeSelector.setSelectedItemIndex(_oscParamsPtr->OscWaveType->getIndex(), dontSendNotification);
	waveTypeSelector.setJustificationType(Justification::centred);
	waveTypeSelector.addListener(this);
	addAndMakeVisible(waveTypeSelector);

	volumeLevelSlider.setRange(_oscParamsPtr->VolumeLevel->range.start, _oscParamsPtr->VolumeLevel->range.end, 0.01);
	volumeLevelSlider.setValue(_oscParamsPtr->VolumeLevel->get(), dontSendNotification);
	volumeLevelSlider.setTextValueSuffix(" dB");
	volumeLevelSlider.addListener(this);
	addAndMakeVisible(volumeLevelSlider);

	attackSlider.setRange(_oscParamsPtr->Attack->range.start, _oscParamsPtr->Attack->range.end, 0.0001);
	attackSlider.setSkewFactorFromMidPoint(1.0f);
	attackSlider.setValue(_oscParamsPtr->Attack->get(), dontSendNotification);
	attackSlider.setTextValueSuffix(" sec");
	attackSlider.addListener(this);
	addAndMakeVisible(attackSlider);

	decaySlider.setRange(_oscParamsPtr->Decay->range.start, _oscParamsPtr->Decay->range.end, 0.0001);
	decaySlider.setSkewFactorFromMidPoint(1.0f);
	decaySlider.setValue(_oscParamsPtr->Decay->get(), dontSendNotification);
	decaySlider.setTextValueSuffix(" sec");
	decaySlider.addListener(this);
	addAndMakeVisible(decaySlider);

	sustainSlider.setRange(_oscParamsPtr->Sustain->range.start, _oscParamsPtr->Sustain->range.end, 0.0001);
	sustainSlider.setValue(_oscParamsPtr->Sustain->get(), dontSendNotification);
	sustainSlider.addListener(this);
	addAndMakeVisible(sustainSlider);

	releaseSlider.setRange(_oscParamsPtr->Release->range.start, _oscParamsPtr->Release->range.end, 0.0001);
	releaseSlider.setSkewFactorFromMidPoint(1.0f);
	releaseSlider.setValue(_oscParamsPtr->Release->get(), dontSendNotification);
	releaseSlider.setTextValueSuffix(" sec");
	releaseSlider.addListener(this);
	addAndMakeVisible(releaseSlider);

	Font paramLabelFont = Font(PARAM_LABEL_FONT_SIZE, Font::plain).withTypefaceStyle("Regular");

	waveTypeSelectorLabel.setFont(paramLabelFont);
	waveTypeSelectorLabel.setText("Type", dontSendNotification);
	waveTypeSelectorLabel.setJustificationType(Justification::centred);
	waveTypeSelectorLabel.setEditable(false, false, false);
	addAndMakeVisible(waveTypeSelectorLabel);

	volumeLevelLabel.setFont(paramLabelFont);
	volumeLevelLabel.setText("Volume", dontSendNotification);
	volumeLevelLabel.setJustificationType(Justification::centred);
	volumeLevelLabel.setEditable(false, false, false);
	addAndMakeVisible(volumeLevelLabel);

	attackLabel.setFont(paramLabelFont);
	attackLabel.setText("Attack", dontSendNotification);
	attackLabel.setJustificationType(Justification::centred);
	attackLabel.setEditable(false, false, false);
	addAndMakeVisible(attackLabel);

	decayLabel.setFont(paramLabelFont);
	decayLabel.setText("Decay", dontSendNotification);
	decayLabel.setJustificationType(Justification::centred);
	decayLabel.setEditable(false, false, false);
	addAndMakeVisible(decayLabel);

	sustainLabel.setFont(paramLabelFont);
	sustainLabel.setText("Sustain", dontSendNotification);
	sustainLabel.setJustificationType(Justification::centred);
	sustainLabel.setEditable(false, false, false);
	addAndMakeVisible(sustainLabel);

	releaseLabel.setFont(paramLabelFont);
	releaseLabel.setText("Release", dontSendNotification);
	releaseLabel.setJustificationType(Justification::centred);
	releaseLabel.setEditable(false, false, false);
	addAndMakeVisible(releaseLabel);

	startTimerHz(30.0f);
}

ChipOscillatorComponent::~ChipOscillatorComponent()
{
}

void ChipOscillatorComponent::paint(Graphics & g)
{
	Font panelNameFont = Font(PANEL_NAME_FONT_SIZE, Font::plain).withTypefaceStyle("Italic");

	{
		float x = 0.0f, y = 0.0f, width = (float)getWidth(), height = (float)getHeight();
		g.setColour(PANEL_COLOUR());
		g.fillRoundedRectangle(x, y, width, height, 10.0f);
	}

	{
		float x = 0.0f, y = 0.0f, width = (float)getWidth(), height = PANEL_NAME_HEIGHT;
		g.setColour(HEADER_COLOUR());
		g.fillRoundedRectangle(x, y, width, height, 10.0f);
	}

	{
		Rectangle<int> bounds = getLocalBounds(); // コンポーネント基準の値
		Rectangle<int> textArea = bounds.removeFromTop(PANEL_NAME_HEIGHT).reduced(LOCAL_MARGIN);

		String text("OSCILLATOR");
		Colour textColour = Colours::white;
		g.setColour(textColour);
		g.setFont(panelNameFont);
		g.drawText(text, textArea, Justification::centred, false);
	}

}

void ChipOscillatorComponent::resized()
{
	float rowSize = 6.0f;
	float divide = 1.0f / rowSize;
	int labelWidth = 80;

	float compHeight = (getHeight() - PANEL_NAME_HEIGHT) * divide;

	Rectangle<int> bounds = getLocalBounds(); // コンポーネント基準の値
	bounds.removeFromTop(PANEL_NAME_HEIGHT);
	
	{
		Rectangle<int> area = bounds.removeFromTop(compHeight);
		waveTypeSelectorLabel.setBounds(area.removeFromLeft(labelWidth).reduced(LOCAL_MARGIN));
		waveTypeSelector.setBounds(area.reduced(LOCAL_MARGIN * 2));
	}
	{
		Rectangle<int> area = bounds.removeFromTop(compHeight);
		volumeLevelLabel.setBounds(area.removeFromLeft(labelWidth).reduced(LOCAL_MARGIN));
		volumeLevelSlider.setBounds(area.reduced(LOCAL_MARGIN));
	}
	{
		Rectangle<int> area = bounds.removeFromTop(compHeight);
		attackLabel.setBounds(area.removeFromLeft(labelWidth).reduced(LOCAL_MARGIN));
		attackSlider.setBounds(area.reduced(LOCAL_MARGIN));
	}
	{
		Rectangle<int> area = bounds.removeFromTop(compHeight);
		decayLabel.setBounds(area.removeFromLeft(labelWidth).reduced(LOCAL_MARGIN));
		decaySlider.setBounds(area.reduced(LOCAL_MARGIN));
	}
	{
		Rectangle<int> area = bounds.removeFromTop(compHeight);
		sustainLabel.setBounds(area.removeFromLeft(labelWidth).reduced(LOCAL_MARGIN));
		sustainSlider.setBounds(area.reduced(LOCAL_MARGIN));
	}
	{
		Rectangle<int> area = bounds.removeFromTop(compHeight);
		releaseLabel.setBounds(area.removeFromLeft(labelWidth).reduced(LOCAL_MARGIN));
		releaseSlider.setBounds(area.reduced(LOCAL_MARGIN));
	}
}

void ChipOscillatorComponent::timerCallback()
{
	waveTypeSelector.setSelectedItemIndex(_oscParamsPtr->OscWaveType->getIndex(), dontSendNotification);
	volumeLevelSlider.setValue(_oscParamsPtr->VolumeLevel->get(), dontSendNotification);
	attackSlider.setValue(_oscParamsPtr->Attack->get(), dontSendNotification);
	decaySlider.setValue(_oscParamsPtr->Decay->get(), dontSendNotification);
	sustainSlider.setValue(_oscParamsPtr->Sustain->get(), dontSendNotification);
	releaseSlider.setValue(_oscParamsPtr->Release->get(), dontSendNotification);

}

void ChipOscillatorComponent::sliderValueChanged(Slider* slider)
{
	if (slider == &volumeLevelSlider)
	{
		*_oscParamsPtr->VolumeLevel = (float)volumeLevelSlider.getValue();
	}
	else if (slider == &attackSlider)
	{
		*_oscParamsPtr->Attack = (float)attackSlider.getValue();
	}
	else if (slider == &decaySlider)
	{
		*_oscParamsPtr->Decay = (float)decaySlider.getValue();
	}
	else if (slider == &sustainSlider)
	{
		*_oscParamsPtr->Sustain = (float)sustainSlider.getValue();
	}
	else if (slider == &releaseSlider)
	{
		*_oscParamsPtr->Release = (float)releaseSlider.getValue();
	}
}

void ChipOscillatorComponent::comboBoxChanged(ComboBox* comboBoxThatHasChanged)
{
	if (comboBoxThatHasChanged == &waveTypeSelector)
	{
		*_oscParamsPtr->OscWaveType = waveTypeSelector.getSelectedItemIndex();
	}
}

//----------------------------------------------------------------------------------------------------

SweepParametersComponent::SweepParametersComponent(SweepParameters * sweepParams)
	:_sweepParamsPtr(sweepParams)
	, sweepSwitchSelector("Sweep-Swetch")
	, timeSlider(Slider::SliderStyle::LinearHorizontal, Slider::TextEntryBoxPosition::TextBoxLeft)
{
	sweepSwitchSelector.addItemList(_sweepParamsPtr->SweepSwitch ->getAllValueStrings(), 1);
	sweepSwitchSelector.setSelectedItemIndex(_sweepParamsPtr->SweepSwitch->getIndex(), dontSendNotification);
	sweepSwitchSelector.setJustificationType(Justification::centred);
	sweepSwitchSelector.addListener(this);
	addAndMakeVisible(sweepSwitchSelector);

	timeSlider.setRange(_sweepParamsPtr->SweepTime->range.start, _sweepParamsPtr->SweepTime->range.end, 0.01);
	timeSlider.setValue(_sweepParamsPtr->SweepTime->get(), dontSendNotification);
	timeSlider.setSkewFactorFromMidPoint(1.0f);
	timeSlider.setTextValueSuffix(" sec");
	timeSlider.addListener(this);
	addAndMakeVisible(timeSlider);

	Font paramLabelFont = Font(PARAM_LABEL_FONT_SIZE, Font::plain).withTypefaceStyle("Regular");

	switchLabel.setFont(paramLabelFont);
	switchLabel.setText("Switch", dontSendNotification);
	switchLabel.setJustificationType(Justification::centred);
	switchLabel.setEditable(false, false, false);
	addAndMakeVisible(switchLabel);
	
	timeLabel.setFont(paramLabelFont);
	timeLabel.setText("Speed", dontSendNotification);
	timeLabel.setJustificationType(Justification::centred);
	timeLabel.setEditable(false, false, false);
	addAndMakeVisible(timeLabel);

	startTimerHz(30.0f);
}

SweepParametersComponent::~SweepParametersComponent()
{
}

void SweepParametersComponent::paint(Graphics & g)
{
	Font panelNameFont = Font(PANEL_NAME_FONT_SIZE, Font::plain).withTypefaceStyle("Italic");

	{
		float x = 0.0f, y = 0.0f, width = (float)getWidth(), height = (float)getHeight();
		g.setColour(PANEL_COLOUR());
		g.fillRoundedRectangle(x, y, width, height, 10.0f);
	}

	{
		float x = 0.0f, y = 0.0f, width = (float)getWidth(), height = PANEL_NAME_HEIGHT;
		g.setColour(HEADER_COLOUR());
		g.fillRoundedRectangle(x, y, width, height, 10.0f);
	}

	{
		Rectangle<int> bounds = getLocalBounds();
		Rectangle<int> textArea = bounds.removeFromTop(PANEL_NAME_HEIGHT).reduced(LOCAL_MARGIN);

		String text("PITCH SWEEP");
		Colour textColour = Colours::white;
		g.setColour(textColour);
		g.setFont(panelNameFont);
		g.drawText(text, textArea, Justification::centred, false);
	}
}

void SweepParametersComponent::resized()
{
	float rowSize = 2.0f;
	float divide = 1.0f / rowSize;
	int labelWidth = 60;

	float compHeight = (getHeight() - PANEL_NAME_HEIGHT) * divide;

	Rectangle<int> bounds = getLocalBounds(); // コンポーネント基準の値
	bounds.removeFromTop(PANEL_NAME_HEIGHT);

	{
		Rectangle<int> area = bounds.removeFromTop(compHeight);
		switchLabel.setBounds(area.removeFromLeft(labelWidth).reduced(LOCAL_MARGIN));
		sweepSwitchSelector.setBounds(area.reduced(LOCAL_MARGIN * 2));
	}
	{
		Rectangle<int> area = bounds.removeFromTop(compHeight);
		timeLabel.setBounds(area.removeFromLeft(labelWidth).reduced(LOCAL_MARGIN));
		timeSlider.setBounds(area.reduced(LOCAL_MARGIN));
	}
}

void SweepParametersComponent::timerCallback()
{
	sweepSwitchSelector.setSelectedItemIndex(_sweepParamsPtr->SweepSwitch->getIndex(), dontSendNotification);
	timeSlider.setValue(_sweepParamsPtr->SweepTime->get(), dontSendNotification);
}

void SweepParametersComponent::sliderValueChanged(Slider* slider)
{
	if (slider == &timeSlider)
	{
		*_sweepParamsPtr->SweepTime = (float)timeSlider.getValue();
	}
}

void SweepParametersComponent::comboBoxChanged(ComboBox* comboBoxThatHasChanged)
{
	if (comboBoxThatHasChanged == &sweepSwitchSelector)
	{
		*_sweepParamsPtr->SweepSwitch = sweepSwitchSelector.getSelectedItemIndex();
	}
}

//----------------------------------------------------------------------------------------------------

VibratoParametersComponent::VibratoParametersComponent(VibratoParameters * vibratoParams)
	:_vibratoParamsPtr(vibratoParams)
	, enableButton("Vibrato-Enable")
	, targetSelector("LFO-Target")
	, waveTypeSelector("LFO-WaveType")
	, amountSlider(Slider::SliderStyle::LinearHorizontal, Slider::TextEntryBoxPosition::TextBoxLeft)
	, speedSlider(Slider::SliderStyle::LinearHorizontal, Slider::TextEntryBoxPosition::TextBoxLeft)
	, attackTimeSlider(Slider::SliderStyle::LinearHorizontal, Slider::TextEntryBoxPosition::TextBoxLeft)
{
	enableButton.setButtonText("ON / OFF");
	enableButton.setToggleState(_vibratoParamsPtr->VibratoEnable->get(), dontSendNotification);
	enableButton.addListener(this);
	addAndMakeVisible(enableButton);

	amountSlider.setRange(_vibratoParamsPtr->VibratoAmount->range.start, _vibratoParamsPtr->VibratoAmount->range.end, 0.01);
	amountSlider.setSkewFactorFromMidPoint(2.0f);
	amountSlider.setValue(_vibratoParamsPtr->VibratoAmount->get(), dontSendNotification);	
	amountSlider.setTextValueSuffix(" HarfTone");
	amountSlider.addListener(this);
	addAndMakeVisible(amountSlider);

	speedSlider.setRange(_vibratoParamsPtr->VibratoSpeed->range.start, _vibratoParamsPtr->VibratoSpeed->range.end, 0.001);
	speedSlider.setSkewFactorFromMidPoint(2.0f);
	speedSlider.setValue(_vibratoParamsPtr->VibratoSpeed->get(), dontSendNotification);
	speedSlider.setTextValueSuffix(" hz");
	speedSlider.addListener(this);
	addAndMakeVisible(speedSlider);

	attackTimeSlider.setRange(_vibratoParamsPtr->VibratoAttackTime->range.start, _vibratoParamsPtr->VibratoAttackTime->range.end, 0.1);
	attackTimeSlider.setValue(_vibratoParamsPtr->VibratoAttackTime->get(), dontSendNotification);
	attackTimeSlider.setTextValueSuffix(" sec");
	attackTimeSlider.addListener(this);
	addAndMakeVisible(attackTimeSlider);

	Font paramLabelFont = Font(PARAM_LABEL_FONT_SIZE, Font::plain).withTypefaceStyle("Regular");

	amountLabel.setFont(paramLabelFont);
	amountLabel.setText("Depth", dontSendNotification);
	amountLabel.setJustificationType(Justification::centred);
	amountLabel.setEditable(false, false, false);
	addAndMakeVisible(amountLabel);

	speedLabel.setFont(paramLabelFont);
	speedLabel.setText("Speed", dontSendNotification);
	speedLabel.setJustificationType(Justification::centred);
	speedLabel.setEditable(false, false, false);
	addAndMakeVisible(speedLabel);
	
	attackTimeLabel.setFont(paramLabelFont);
	attackTimeLabel.setText("Attack", dontSendNotification);
	attackTimeLabel.setJustificationType(Justification::centred);
	attackTimeLabel.setEditable(false, false, false);
	addAndMakeVisible(attackTimeLabel);

	startTimerHz(30.0f);
}

VibratoParametersComponent::~VibratoParametersComponent()
{
}

void VibratoParametersComponent::paint(Graphics & g)
{
	Font panelNameFont = Font(PANEL_NAME_FONT_SIZE, Font::plain).withTypefaceStyle("Italic");

	{
		float x = 0.0f, y = 0.0f, width = (float)getWidth(), height = (float)getHeight();
		g.setColour(PANEL_COLOUR());
		g.fillRoundedRectangle(x, y, width, height, 10.0f);
	}

	{
		float x = 0.0f, y = 0.0f, width = (float)getWidth(), height = PANEL_NAME_HEIGHT;
		g.setColour(HEADER_COLOUR());
		g.fillRoundedRectangle(x, y, width, height, 10.0f);
	}

	{
		Rectangle<int> bounds = getLocalBounds();
		Rectangle<int> textArea = bounds.removeFromTop(PANEL_NAME_HEIGHT).reduced(LOCAL_MARGIN);

		String text("VIBRATO");
		Colour textColour = Colours::white;
		g.setColour(textColour);
		g.setFont(panelNameFont);
		g.drawText(text, textArea, Justification::centred, false);
	}
}

void VibratoParametersComponent::resized()
{
	float rowSize = 4.0f;
	float divide = 1.0f / rowSize;
	int labelWidth = 60;

	float compHeight = (getHeight() - PANEL_NAME_HEIGHT) * divide;

	Rectangle<int> bounds = getLocalBounds(); // コンポーネント基準の値
	bounds.removeFromTop(PANEL_NAME_HEIGHT);

	{
		Rectangle<int> area = bounds.removeFromTop(compHeight);
		area.removeFromLeft(labelWidth / 2);
		enableButton.setBounds(area.reduced(LOCAL_MARGIN));
	}
	{
		Rectangle<int> area = bounds.removeFromTop(compHeight);
		amountLabel.setBounds(area.removeFromLeft(labelWidth).reduced(LOCAL_MARGIN));
		amountSlider.setBounds(area.reduced(LOCAL_MARGIN));

	}
	{
		Rectangle<int> area = bounds.removeFromTop(compHeight);
		speedLabel.setBounds(area.removeFromLeft(labelWidth).reduced(LOCAL_MARGIN));
		speedSlider.setBounds(area.reduced(LOCAL_MARGIN));
	}
	{
		Rectangle<int> area = bounds.removeFromTop(compHeight);
		attackTimeLabel.setBounds(area.removeFromLeft(labelWidth).reduced(LOCAL_MARGIN));
		attackTimeSlider.setBounds(area.reduced(LOCAL_MARGIN));
	}
}

void VibratoParametersComponent::timerCallback()
{
	enableButton.setToggleState(_vibratoParamsPtr->VibratoEnable->get(), dontSendNotification);
	amountSlider.setValue(_vibratoParamsPtr->VibratoAmount->get(), dontSendNotification);
	speedSlider.setValue(_vibratoParamsPtr->VibratoSpeed->get(), dontSendNotification);
	attackTimeSlider.setValue(_vibratoParamsPtr->VibratoAttackTime->get(), dontSendNotification);
}

void VibratoParametersComponent::sliderValueChanged(Slider* slider)
{
	if (slider == &amountSlider)
	{
		*_vibratoParamsPtr->VibratoAmount = (float)amountSlider.getValue();
	}
	else if (slider == &speedSlider)
	{
		*_vibratoParamsPtr->VibratoSpeed = (float)speedSlider.getValue();
	}
	else if (slider == &attackTimeSlider)
	{
		*_vibratoParamsPtr->VibratoAttackTime = (float)attackTimeSlider.getValue();
	}
}

void VibratoParametersComponent::buttonClicked(Button* button)
{
	if (button == &enableButton)
	{
		*_vibratoParamsPtr->VibratoEnable = enableButton.getToggleState();
	}
}

//----------------------------------------------------------------------------------------------------

VoicingParametersComponent::VoicingParametersComponent(VoicingParameters* voicingParams)
	: _voicingParamsPtr(voicingParams)
	, voicingTypeSelector("voicing")
	, portaTimeSlider(Slider::SliderStyle::LinearHorizontal, Slider::TextEntryBoxPosition::TextBoxLeft)
{
	voicingTypeSelector.addItemList(_voicingParamsPtr->VoicingSwitch->getAllValueStrings(), 1);
	voicingTypeSelector.setSelectedItemIndex(_voicingParamsPtr->VoicingSwitch->getIndex(), dontSendNotification);
	voicingTypeSelector.setJustificationType(Justification::centred);
	voicingTypeSelector.addListener(this);
	addAndMakeVisible(voicingTypeSelector);

	portaTimeSlider.setRange(_voicingParamsPtr->PortaTime->range.start, _voicingParamsPtr->PortaTime->range.end, 0.01);
	portaTimeSlider.setValue(_voicingParamsPtr->PortaTime->get(), dontSendNotification);
	portaTimeSlider.setSkewFactorFromMidPoint(0.5f);
	portaTimeSlider.setTextValueSuffix(" sec");
	portaTimeSlider.addListener(this);
	addAndMakeVisible(portaTimeSlider);

	Font paramLabelFont = Font(PARAM_LABEL_FONT_SIZE, Font::plain).withTypefaceStyle("Regular");

	voicingTypeSelectorLabel.setFont(paramLabelFont);
	voicingTypeSelectorLabel.setText("Type", dontSendNotification);
	voicingTypeSelectorLabel.setJustificationType(Justification::centred);
	voicingTypeSelectorLabel.setEditable(false, false, false);
	addAndMakeVisible(voicingTypeSelectorLabel);

	portaTimeLabel.setFont(paramLabelFont);
	portaTimeLabel.setText("Portamento", dontSendNotification);
	portaTimeLabel.setJustificationType(Justification::centred);
	portaTimeLabel.setEditable(false, false, false);
	addAndMakeVisible(portaTimeLabel);

	startTimerHz(30.0f);
}

VoicingParametersComponent::~VoicingParametersComponent()
{
}

void VoicingParametersComponent::paint(Graphics & g)
{
	Font panelNameFont = Font(PANEL_NAME_FONT_SIZE, Font::plain).withTypefaceStyle("Italic");

	{
		float x = 0.0f, y = 0.0f, width = (float)getWidth(), height = (float)getHeight();
		g.setColour(PANEL_COLOUR());
		g.fillRoundedRectangle(x, y, width, height, 10.0f);
	}

	{
		float x = 0.0f, y = 0.0f, width = (float)getWidth(), height = PANEL_NAME_HEIGHT;
		g.setColour(HEADER_COLOUR());
		g.fillRoundedRectangle(x, y, width, height, 10.0f);
	}

	{
		Rectangle<int> bounds = getLocalBounds(); // コンポーネント基準の値
		Rectangle<int> textArea = bounds.removeFromTop(PANEL_NAME_HEIGHT).reduced(LOCAL_MARGIN);

		String text("VOICING");
		Colour textColour = Colours::white;
		g.setColour(textColour);
		g.setFont(panelNameFont);
		g.drawText(text, textArea, Justification::centred, false);
	}

}

void VoicingParametersComponent::resized()
{
	float rowSize = 2.0f;
	float divide = 1.0f / rowSize;
	int labelWidth = 80;

	float compHeight = (getHeight() - PANEL_NAME_HEIGHT) * divide;

	Rectangle<int> bounds = getLocalBounds(); // コンポーネント基準の値
	bounds.removeFromTop(PANEL_NAME_HEIGHT);

	{
		Rectangle<int> area = bounds.removeFromTop(compHeight);
		voicingTypeSelectorLabel.setBounds(area.removeFromLeft(labelWidth).reduced(LOCAL_MARGIN));
		voicingTypeSelector.setBounds(area.reduced(LOCAL_MARGIN * 2));
	}
	{
		Rectangle<int> area = bounds.removeFromTop(compHeight);
		portaTimeLabel.setBounds(area.removeFromLeft(labelWidth).reduced(LOCAL_MARGIN));
		portaTimeSlider.setBounds(area.reduced(LOCAL_MARGIN));
	}
}

void VoicingParametersComponent::timerCallback()
{
	voicingTypeSelector.setSelectedItemIndex(_voicingParamsPtr->VoicingSwitch->getIndex(), dontSendNotification);
	portaTimeSlider.setValue(_voicingParamsPtr->PortaTime->get(), dontSendNotification);
}

void VoicingParametersComponent::sliderValueChanged(Slider* slider)
{
	if (slider == &portaTimeSlider)
	{
		*_voicingParamsPtr->PortaTime = (float)portaTimeSlider.getValue();
	}
}

void VoicingParametersComponent::comboBoxChanged(ComboBox* comboBoxThatHasChanged)
{
	if (comboBoxThatHasChanged == &voicingTypeSelector)
	{
		*_voicingParamsPtr->VoicingSwitch = voicingTypeSelector.getSelectedItemIndex();
	}
}

//----------------------------------------------------------------------------------------------------

OptionsParametersComponent::OptionsParametersComponent(OptionsParameters* optionsParams)
	: _optionsParamsPtr(optionsParams)
	, velocitySenseButton()
	, pitchStandardSlider(Slider::SliderStyle::IncDecButtons, Slider::TextEntryBoxPosition::TextBoxLeft)
	, pitchBendRangeSlider(Slider::SliderStyle::IncDecButtons, Slider::TextEntryBoxPosition::TextBoxLeft)
{
	//velocitySenseButton.setButtonText("Key Velocity Sense");
	//velocitySenseButton.setToggleState(_optionsParamsPtr->IsVelocitySense->get(), dontSendNotification);
	//velocitySenseButton.addListener(this);
	//addAndMakeVisible(velocitySenseButton);

	pitchStandardSlider.setRange(_optionsParamsPtr->PitchStandard->getRange().getStart(), _optionsParamsPtr->PitchStandard->getRange().getEnd(), 1);
	pitchStandardSlider.setValue(_optionsParamsPtr->PitchStandard->get(), dontSendNotification);
	pitchStandardSlider.addListener(this);
	addAndMakeVisible(pitchStandardSlider);

	pitchBendRangeSlider.setRange(_optionsParamsPtr->PitchBendRange->getRange().getStart(), _optionsParamsPtr->PitchBendRange->getRange().getEnd(), 1);
	pitchBendRangeSlider.setValue(_optionsParamsPtr->PitchBendRange->get(), dontSendNotification);
	pitchBendRangeSlider.addListener(this);
	addAndMakeVisible(pitchBendRangeSlider);

	Font paramLabelFont = Font(PARAM_LABEL_FONT_SIZE, Font::plain).withTypefaceStyle("Regular");

	pitchStandardLabel.setFont(paramLabelFont);
	pitchStandardLabel.setText("Tunes", dontSendNotification);
	pitchStandardLabel.setJustificationType(Justification::centred);
	pitchStandardLabel.setEditable(false, false, false);
	addAndMakeVisible(pitchStandardLabel);

	pitchBendRangeLabel.setFont(paramLabelFont);
	pitchBendRangeLabel.setText("PB Range", dontSendNotification);
	pitchBendRangeLabel.setJustificationType(Justification::centred);
	pitchBendRangeLabel.setEditable(false, false, false);
	addAndMakeVisible(pitchBendRangeLabel);

	startTimerHz(30.0f);
}

OptionsParametersComponent::~OptionsParametersComponent()
{
}

void OptionsParametersComponent::paint(Graphics & g)
{
	Font panelNameFont = Font(PANEL_NAME_FONT_SIZE, Font::plain).withTypefaceStyle("Italic");

	{
		int x = 0.0f, y = 0.0f, width = getWidth(), height = getHeight();
		g.setColour(PANEL_COLOUR());
		g.fillRoundedRectangle(x, y, width, height, 10.0f);
	}

	{
		float x = 0.0f, y = 0.0f, width = (float)getWidth(), height = PANEL_NAME_HEIGHT;
		g.setColour(HEADER_COLOUR());
		g.fillRoundedRectangle(x, y, width, height, 10.0f);
	}

	{
		Rectangle<int> bounds = getLocalBounds();
		Rectangle<int> textArea = bounds.removeFromTop(PANEL_NAME_HEIGHT).reduced(LOCAL_MARGIN);

		String text("OPTIONS");
		Colour textColour = Colours::white;
		g.setColour(textColour);
		g.setFont(panelNameFont);
		g.drawText(text, textArea, Justification::centred, false);
	}
}

void OptionsParametersComponent::resized()
{
	float columnSize = 2.0f;
	float divide = 1.0f / columnSize;
	int labelWidth = 80;

	float compHeight = (getHeight() - PANEL_NAME_HEIGHT) * divide;

	Rectangle<int> bounds = getLocalBounds(); // コンポーネント基準の値
	bounds.removeFromTop(PANEL_NAME_HEIGHT);

	{
		Rectangle<int> area = bounds.removeFromTop(compHeight);
		pitchStandardLabel.setBounds(area.removeFromLeft(labelWidth).reduced(LOCAL_MARGIN));
		pitchStandardSlider.setBounds(area.reduced(LOCAL_MARGIN));
	}
	{
		Rectangle<int> area = bounds.removeFromTop(compHeight);
		pitchBendRangeLabel.setBounds(area.removeFromLeft(labelWidth).reduced(LOCAL_MARGIN));
		pitchBendRangeSlider.setBounds(area.reduced(LOCAL_MARGIN));
	}
}

void OptionsParametersComponent::timerCallback()
{
	velocitySenseButton.setToggleState(_optionsParamsPtr->IsVelocitySense->get(), dontSendNotification);
	pitchStandardSlider.setValue(_optionsParamsPtr->PitchStandard->get(), dontSendNotification);
	pitchBendRangeSlider.setValue(_optionsParamsPtr->PitchBendRange->get(), dontSendNotification);
}

void OptionsParametersComponent::buttonClicked(Button* button)
{
	if (button == &velocitySenseButton)
	{
		*_optionsParamsPtr->IsVelocitySense = velocitySenseButton.getToggleState();
	}
}

void OptionsParametersComponent::sliderValueChanged(Slider * slider)
{
	if (slider == &pitchStandardSlider)
	{
		*_optionsParamsPtr->PitchStandard = (float)pitchStandardSlider.getValue();
	}
	else if (slider == &pitchBendRangeSlider)
	{
		*_optionsParamsPtr->PitchBendRange = (float)pitchBendRangeSlider.getValue();
	}
}

//----------------------------------------------------------------------------------------------------

WaveformMemoryParametersComponent::WaveformMemoryParametersComponent(WaveformMemoryParameters* waveformMemoryParams)
	: _waveformMemoryParamsPtr(waveformMemoryParams)
	, waveSampleSlider{}
{
	for (int i = 0; i < WAVESAMPLE_LENGTH; i++)
	{
		waveSampleSlider[i].setRange(0, 15, 1.0);
		waveSampleSlider[i].setValue(7, dontSendNotification);
	}
	startTimerHz(30.0f);
	timerCallback();
}

WaveformMemoryParametersComponent::~WaveformMemoryParametersComponent()
{
}

void WaveformMemoryParametersComponent::paint(Graphics & g)
{
	Font panelNameFont = Font(PANEL_NAME_FONT_SIZE, Font::plain).withTypefaceStyle("Italic");

	{
		int x = 0.0f, y = 0.0f, width = getWidth(), height = getHeight();
		g.setColour(PANEL_COLOUR());
		g.fillRoundedRectangle(x, y, width, height, 10.0f);
	}
	{
		float x = 0.0f, y = 0.0f, width = (float)getWidth(), height = PANEL_NAME_HEIGHT;
		g.setColour(HEADER_COLOUR());
		g.fillRoundedRectangle(x, y, width, height, 10.0f);

		Rectangle<int> bounds = getLocalBounds();
		Rectangle<int> textArea = bounds.removeFromTop(PANEL_NAME_HEIGHT).reduced(LOCAL_MARGIN);

		String text("WAVEFORM MEMORY");
		Colour textColour = Colours::white;
		g.setColour(textColour);
		g.setFont(panelNameFont);
		g.drawText(text, textArea, Justification::centred, false);
	}

	//update slider Params
	for (auto* trail : trails)
	{	
		float compWidth = getWidth() - 12; // 10は微調整値		
		int index = (int)(trail->currentPosition.x * 32.0 / compWidth);
		if (index < 0)
		{
			index = 0;
		}
		else if (index >= 31)
		{
			index = 31;
		}

		float compHeight = getHeight() - PANEL_NAME_HEIGHT; 
		float point = trail->currentPosition.y - PANEL_NAME_HEIGHT;
		int value = 15 - (int)(point * 16.0 / compHeight);

		waveSampleSlider[index].setValue(value, dontSendNotification);
		updateValue();
	}
	//repaint Sliders
	{	
		float columnSize = (float)32;
		float divide = 1.0f / columnSize;
		float compWidth = (getWidth()) * divide;
		Rectangle<int> bounds = getLocalBounds();
		bounds.removeFromTop(PANEL_NAME_HEIGHT).reduced(10);

		for (int i = 0; i < WAVESAMPLE_LENGTH; i++)
		{
			Rectangle<int> area = bounds.removeFromLeft(compWidth);
			area.removeFromTop(bounds.getHeight() / 16 * (waveSampleSlider[i].getMaximum() - waveSampleSlider[i].getValue()));
			g.setColour(Colours::lime);
			g.fillRect(area.reduced(1.5));
		}
	}
}

void WaveformMemoryParametersComponent::resized()
{
	/*
	float columnSize = (float) 32;
	float divide = 1.0f / columnSize;


	float compWidth = (getWidth()) * divide;

	Rectangle<int> bounds = getLocalBounds(); // コンポーネント基準の値
	
	bounds.removeFromTop(PANEL_NAME_HEIGHT);

	for(int i = 0; i < WAVESAMPLE_LENGTH; i++)
	{
		Rectangle<int> area = bounds.removeFromLeft(compWidth);
		waveSampleSlider[i].setBounds(area.reduced(0.1f, LOCAL_MARGIN));
	}
	*/
}

void WaveformMemoryParametersComponent::timerCallback()
{
	for (int i = 0; i < 32; i++)
	{
		waveSampleSlider[i].setValue(_waveformMemoryParamsPtr->WaveSamplesArray[i]->get()  , dontSendNotification);
	}
}

void WaveformMemoryParametersComponent::updateValue()
{
	for (int i = 0; i < 32; i++)
	{
		*_waveformMemoryParamsPtr->WaveSamplesArray[i] = (int)waveSampleSlider[i].getValue();
	}
}

void WaveformMemoryParametersComponent::mouseDown(const MouseEvent& e)
{
	auto* t = getTrail(e.source);

	if (t == nullptr)
	{
		t = new Trail(e.source);
		t->path.startNewSubPath(e.position);
		trails.add(t);
	}

	t->pushPoint(e.position, e.mods, e.pressure);

	repaint();
}

void WaveformMemoryParametersComponent::mouseUp(const MouseEvent& e)
{
	trails.removeObject(getTrail(e.source));
	repaint();
}

void WaveformMemoryParametersComponent::mouseDrag(const MouseEvent& e)
{
	auto* t = getTrail(e.source);

	if (t == nullptr)
	{
		t = new Trail(e.source);
		t->path.startNewSubPath(e.position);
		trails.add(t);
	}

	t->pushPoint(e.position, e.mods, e.pressure);

	repaint();
}
