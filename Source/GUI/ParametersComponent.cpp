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
	const Colour TEXT_COLOUR() { return Colours::white; }

	const float PANEL_NAME_FONT_SIZE = 24.0f;
	const float PARAM_LABEL_FONT_SIZE = 18.0f;
	const int PANEL_NAME_HEIGHT = 32;
	const int LOCAL_MARGIN = 2;
	const int LABEL_WIDTH = 80;

	const Font panelNameFont() { return Font(PANEL_NAME_FONT_SIZE, Font::plain).withTypefaceStyle("Italic"); };
	const Font paramLabelFont() { return Font(PARAM_LABEL_FONT_SIZE, Font::plain).withTypefaceStyle("Regular"); };

	static File preFilePath = File::getSpecialLocation(File::userDesktopDirectory);
}

std::vector<std::string> split(std::string str, char del) 
{
	int first = 0;
	int last = str.find_first_of(del);

	std::vector<std::string> result;

	while (first < str.size()) 
	{
		std::string subStr(str, first, last - first);

		result.push_back(subStr);

		first = last + 1;
		last = str.find_first_of(del, first);

		if (last == std::string::npos)
		{
			last = str.size();
		}
	}
	return result;
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

	waveTypeSelectorLabel.setFont(paramLabelFont());
	waveTypeSelectorLabel.setText("Type", dontSendNotification);
	waveTypeSelectorLabel.setJustificationType(Justification::centred);
	waveTypeSelectorLabel.setEditable(false, false, false);
	addAndMakeVisible(waveTypeSelectorLabel);

	volumeLevelLabel.setFont(paramLabelFont());
	volumeLevelLabel.setText("Volume", dontSendNotification);
	volumeLevelLabel.setJustificationType(Justification::centred);
	volumeLevelLabel.setEditable(false, false, false);
	addAndMakeVisible(volumeLevelLabel);

	attackLabel.setFont(paramLabelFont());
	attackLabel.setText("Attack", dontSendNotification);
	attackLabel.setJustificationType(Justification::centred);
	attackLabel.setEditable(false, false, false);
	addAndMakeVisible(attackLabel);

	decayLabel.setFont(paramLabelFont());
	decayLabel.setText("Decay", dontSendNotification);
	decayLabel.setJustificationType(Justification::centred);
	decayLabel.setEditable(false, false, false);
	addAndMakeVisible(decayLabel);

	sustainLabel.setFont(paramLabelFont());
	sustainLabel.setText("Sustain", dontSendNotification);
	sustainLabel.setJustificationType(Justification::centred);
	sustainLabel.setEditable(false, false, false);
	addAndMakeVisible(sustainLabel);

	releaseLabel.setFont(paramLabelFont());
	releaseLabel.setText("Release", dontSendNotification);
	releaseLabel.setJustificationType(Justification::centred);
	releaseLabel.setEditable(false, false, false);
	addAndMakeVisible(releaseLabel);

	startTimerHz(30);
}

ChipOscillatorComponent::~ChipOscillatorComponent()
{}

void ChipOscillatorComponent::paint(Graphics& g)
{
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
		g.setColour(TEXT_COLOUR());
		g.setFont(panelNameFont());
		g.drawText(text, textArea, Justification::centred, false);
	}
}

void ChipOscillatorComponent::resized()
{
	float rowSize = 6.0f;
	float divide = 1.0f / rowSize;
	int compHeight = int((getHeight() - PANEL_NAME_HEIGHT) * divide);

	Rectangle<int> bounds = getLocalBounds(); // コンポーネント基準の値
	bounds.removeFromTop(PANEL_NAME_HEIGHT);
	
	{
		Rectangle<int> area = bounds.removeFromTop(compHeight);
		waveTypeSelectorLabel.setBounds(area.removeFromLeft(LABEL_WIDTH).reduced(LOCAL_MARGIN));
		waveTypeSelector.setBounds(area.reduced(LOCAL_MARGIN * 2));
	}
	{
		Rectangle<int> area = bounds.removeFromTop(compHeight);
		volumeLevelLabel.setBounds(area.removeFromLeft(LABEL_WIDTH).reduced(LOCAL_MARGIN));
		volumeLevelSlider.setBounds(area.reduced(LOCAL_MARGIN));
	}
	{
		Rectangle<int> area = bounds.removeFromTop(compHeight);
		attackLabel.setBounds(area.removeFromLeft(LABEL_WIDTH).reduced(LOCAL_MARGIN));
		attackSlider.setBounds(area.reduced(LOCAL_MARGIN));
	}
	{
		Rectangle<int> area = bounds.removeFromTop(compHeight);
		decayLabel.setBounds(area.removeFromLeft(LABEL_WIDTH).reduced(LOCAL_MARGIN));
		decaySlider.setBounds(area.reduced(LOCAL_MARGIN));
	}
	{
		Rectangle<int> area = bounds.removeFromTop(compHeight);
		sustainLabel.setBounds(area.removeFromLeft(LABEL_WIDTH).reduced(LOCAL_MARGIN));
		sustainSlider.setBounds(area.reduced(LOCAL_MARGIN));
	}
	{
		Rectangle<int> area = bounds.removeFromTop(compHeight);
		releaseLabel.setBounds(area.removeFromLeft(LABEL_WIDTH).reduced(LOCAL_MARGIN));
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

SweepParametersComponent::SweepParametersComponent(SweepParameters* sweepParams)
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

	switchLabel.setFont(paramLabelFont());
	switchLabel.setText("Switch", dontSendNotification);
	switchLabel.setJustificationType(Justification::centred);
	switchLabel.setEditable(false, false, false);
	addAndMakeVisible(switchLabel);
	
	timeLabel.setFont(paramLabelFont());
	timeLabel.setText("Speed", dontSendNotification);
	timeLabel.setJustificationType(Justification::centred);
	timeLabel.setEditable(false, false, false);
	addAndMakeVisible(timeLabel);

	startTimerHz(30);
}

SweepParametersComponent::~SweepParametersComponent()
{}

void SweepParametersComponent::paint(Graphics& g)
{
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
		g.setColour(TEXT_COLOUR());
		g.setFont(panelNameFont());
		g.drawText(text, textArea, Justification::centred, false);
	}
}

void SweepParametersComponent::resized()
{
	float rowSize = 2.0f;
	float divide = 1.0f / rowSize;
	int compHeight = int((getHeight() - PANEL_NAME_HEIGHT) * divide);

	Rectangle<int> bounds = getLocalBounds(); // コンポーネント基準の値
	bounds.removeFromTop(PANEL_NAME_HEIGHT);

	{
		float alpha = isEditable() ? 1.0f : 0.4f;
		timeLabel.setAlpha(alpha);
		timeSlider.setAlpha(alpha);
	}
	{
		Rectangle<int> area = bounds.removeFromTop(compHeight);
		switchLabel.setBounds(area.removeFromLeft(LABEL_WIDTH).reduced(LOCAL_MARGIN));
		sweepSwitchSelector.setBounds(area.reduced(LOCAL_MARGIN * 2));
	}
	{
		Rectangle<int> area = bounds.removeFromTop(compHeight);
		timeLabel.setBounds(area.removeFromLeft(LABEL_WIDTH).reduced(LOCAL_MARGIN));
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
	resized();
}

bool SweepParametersComponent::isEditable()
{
	return (_sweepParamsPtr->SweepSwitch->getIndex() >= 1) ? true : false;
}

//----------------------------------------------------------------------------------------------------

VibratoParametersComponent::VibratoParametersComponent(VibratoParameters* vibratoParams)
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

	amountLabel.setFont(paramLabelFont());
	amountLabel.setText("Depth", dontSendNotification);
	amountLabel.setJustificationType(Justification::centred);
	amountLabel.setEditable(false, false, false);
	addAndMakeVisible(amountLabel);

	speedLabel.setFont(paramLabelFont());
	speedLabel.setText("Speed", dontSendNotification);
	speedLabel.setJustificationType(Justification::centred);
	speedLabel.setEditable(false, false, false);
	addAndMakeVisible(speedLabel);
	
	attackTimeLabel.setFont(paramLabelFont());
	attackTimeLabel.setText("Attack", dontSendNotification);
	attackTimeLabel.setJustificationType(Justification::centred);
	attackTimeLabel.setEditable(false, false, false);
	addAndMakeVisible(attackTimeLabel);

	startTimerHz(30);
}

VibratoParametersComponent::~VibratoParametersComponent()
{}

void VibratoParametersComponent::paint(Graphics& g)
{
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
		g.setColour(TEXT_COLOUR());
		g.setFont(panelNameFont());
		g.drawText(text, textArea, Justification::centred, false);
	}
}

void VibratoParametersComponent::resized()
{
	float rowSize = 4.0f;
	float divide = 1.0f / rowSize;
	int compHeight = int((getHeight() - PANEL_NAME_HEIGHT) * divide);

	Rectangle<int> bounds = getLocalBounds(); // コンポーネント基準の値
	bounds.removeFromTop(PANEL_NAME_HEIGHT);

	{
		float alpha = isEditable() ? 1.0f : 0.4f;
		amountLabel.setAlpha(alpha);
		amountSlider.setAlpha(alpha);
		speedLabel.setAlpha(alpha);
		speedSlider.setAlpha(alpha);
		attackTimeLabel.setAlpha(alpha);
		attackTimeSlider.setAlpha(alpha);
	}
	{
		Rectangle<int> area = bounds.removeFromTop(compHeight);
		area.removeFromLeft(LABEL_WIDTH / 2);
		enableButton.setBounds(area.reduced(LOCAL_MARGIN));
	}
	{
		Rectangle<int> area = bounds.removeFromTop(compHeight);
		amountLabel.setBounds(area.removeFromLeft(LABEL_WIDTH).reduced(LOCAL_MARGIN));
		amountSlider.setBounds(area.reduced(LOCAL_MARGIN));

	}
	{
		Rectangle<int> area = bounds.removeFromTop(compHeight);
		speedLabel.setBounds(area.removeFromLeft(LABEL_WIDTH).reduced(LOCAL_MARGIN));
		speedSlider.setBounds(area.reduced(LOCAL_MARGIN));
	}
	{
		Rectangle<int> area = bounds.removeFromTop(compHeight);
		attackTimeLabel.setBounds(area.removeFromLeft(LABEL_WIDTH).reduced(LOCAL_MARGIN));
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
	resized();
}

bool VibratoParametersComponent::isEditable()
{
	return _vibratoParamsPtr->VibratoEnable->get();
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

	voicingTypeSelectorLabel.setFont(paramLabelFont());
	voicingTypeSelectorLabel.setText("Type", dontSendNotification);
	voicingTypeSelectorLabel.setJustificationType(Justification::centred);
	voicingTypeSelectorLabel.setEditable(false, false, false);
	addAndMakeVisible(voicingTypeSelectorLabel);

	portaTimeLabel.setFont(paramLabelFont());
	portaTimeLabel.setText("PortaTime", dontSendNotification);
	portaTimeLabel.setJustificationType(Justification::centred);
	portaTimeLabel.setEditable(false, false, false);
	addAndMakeVisible(portaTimeLabel);

	startTimerHz(30);
}

VoicingParametersComponent::~VoicingParametersComponent()
{}

void VoicingParametersComponent::paint(Graphics& g)
{
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
		g.setColour(TEXT_COLOUR());
		g.setFont(panelNameFont());
		g.drawText(text, textArea, Justification::centred, false);
	}

}

void VoicingParametersComponent::resized()
{
	float rowSize = 2.0f;
	float divide = 1.0f / rowSize;
	int compHeight = int((getHeight() - PANEL_NAME_HEIGHT) * divide);

	Rectangle<int> bounds = getLocalBounds(); // コンポーネント基準の値
	bounds.removeFromTop(PANEL_NAME_HEIGHT);

	{
		float alpha = _voicingParamsPtr->VoicingSwitch->getCurrentChoiceName() == "PORTAMENTO" ? 1.0f : 0.4f;
		portaTimeLabel.setAlpha(alpha);
		portaTimeSlider.setAlpha(alpha);
	}
	{
		Rectangle<int> area = bounds.removeFromTop(compHeight);
		voicingTypeSelectorLabel.setBounds(area.removeFromLeft(LABEL_WIDTH).reduced(LOCAL_MARGIN));
		voicingTypeSelector.setBounds(area.reduced(LOCAL_MARGIN * 2));
	}
	{
		Rectangle<int> area = bounds.removeFromTop(compHeight);
		portaTimeLabel.setBounds(area.removeFromLeft(LABEL_WIDTH).reduced(LOCAL_MARGIN));
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
	resized();
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

	pitchStandardLabel.setFont(paramLabelFont());
	pitchStandardLabel.setText("Tunes", dontSendNotification);
	pitchStandardLabel.setJustificationType(Justification::centred);
	pitchStandardLabel.setEditable(false, false, false);
	addAndMakeVisible(pitchStandardLabel);

	pitchBendRangeLabel.setFont(paramLabelFont());
	pitchBendRangeLabel.setText("PB Range", dontSendNotification);
	pitchBendRangeLabel.setJustificationType(Justification::centred);
	pitchBendRangeLabel.setEditable(false, false, false);
	addAndMakeVisible(pitchBendRangeLabel);

	startTimerHz(30);
}

OptionsParametersComponent::~OptionsParametersComponent()
{}

void OptionsParametersComponent::paint(Graphics& g)
{
	{
		float x = 0.0f, y = 0.0f, width = (float)getWidth(), height = (float)getHeight();
		g.setColour(PANEL_COLOUR());
		g.fillRoundedRectangle(x, y, width, height, 10.0f);
	}
	{
		float x = 0.0f, y = 0.0f, width = (float)getWidth(), height = (float)PANEL_NAME_HEIGHT;
		g.setColour(HEADER_COLOUR());
		g.fillRoundedRectangle(x, y, width, height, 10.0f);
	}
	{
		Rectangle<int> bounds = getLocalBounds();
		Rectangle<int> textArea = bounds.removeFromTop(PANEL_NAME_HEIGHT).reduced(LOCAL_MARGIN);

		String text("OPTIONS");
		g.setColour(TEXT_COLOUR());
		g.setFont(panelNameFont());
		g.drawText(text, textArea, Justification::centred, false);
	}
}

void OptionsParametersComponent::resized()
{
	float columnSize = 2.0f;
	float divide = 1.0f / columnSize;
	int compHeight = int((getHeight() - PANEL_NAME_HEIGHT) * divide);

	Rectangle<int> bounds = getLocalBounds(); // コンポーネント基準の値
	bounds.removeFromTop(PANEL_NAME_HEIGHT);

	{
		Rectangle<int> area = bounds.removeFromTop(compHeight);
		pitchStandardLabel.setBounds(area.removeFromLeft(LABEL_WIDTH).reduced(LOCAL_MARGIN));
		pitchStandardSlider.setBounds(area.reduced(LOCAL_MARGIN));
	}
	{
		Rectangle<int> area = bounds.removeFromTop(compHeight);
		pitchBendRangeLabel.setBounds(area.removeFromLeft(LABEL_WIDTH).reduced(LOCAL_MARGIN));
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

void OptionsParametersComponent::sliderValueChanged(Slider* slider)
{
	if (slider == &pitchStandardSlider)
	{
		*_optionsParamsPtr->PitchStandard = (int)pitchStandardSlider.getValue();
	}
	else if (slider == &pitchBendRangeSlider)
	{
		*_optionsParamsPtr->PitchBendRange = (int)pitchBendRangeSlider.getValue();
	}
}

//----------------------------------------------------------------------------------------------------

MidiEchoParametersComponent::MidiEchoParametersComponent(MidiEchoParameters* midiEchoParams)
	:_midiEchoParamsPtr(midiEchoParams)
	, enableButton("MidiEcho-Enable")
	, durationSlider(Slider::SliderStyle::LinearHorizontal, Slider::TextEntryBoxPosition::TextBoxLeft)
	, repeatSlider(Slider::SliderStyle::LinearHorizontal, Slider::TextEntryBoxPosition::TextBoxLeft)
	, volumeOffsetSlider(Slider::SliderStyle::LinearHorizontal, Slider::TextEntryBoxPosition::TextBoxLeft)
{
	enableButton.setButtonText("ON / OFF");
	enableButton.setToggleState(_midiEchoParamsPtr->IsEchoEnable->get(), dontSendNotification);
	enableButton.addListener(this);
	addAndMakeVisible(enableButton);

	durationSlider.setRange(_midiEchoParamsPtr->EchoDuration->range.start, _midiEchoParamsPtr->EchoDuration->range.end, 0.01);
	durationSlider.setValue(_midiEchoParamsPtr->EchoDuration->get(), dontSendNotification);
	durationSlider.setTextValueSuffix(" sec");
	durationSlider.setSkewFactorFromMidPoint(0.5f);
	durationSlider.addListener(this);
	addAndMakeVisible(durationSlider);

	repeatSlider.setRange(_midiEchoParamsPtr->EchoRepeat->getRange().getStart(), _midiEchoParamsPtr->EchoRepeat->getRange().getEnd(), 1);
	repeatSlider.setValue(_midiEchoParamsPtr->EchoRepeat->get(), dontSendNotification);
	repeatSlider.addListener(this);
	addAndMakeVisible(repeatSlider);

	volumeOffsetSlider.setRange(_midiEchoParamsPtr->VolumeOffset->range.start, _midiEchoParamsPtr->VolumeOffset->range.end, 0.1);
	volumeOffsetSlider.setSkewFactorFromMidPoint(100.0f);
	volumeOffsetSlider.setValue(_midiEchoParamsPtr->VolumeOffset->get(), dontSendNotification);
	volumeOffsetSlider.setTextValueSuffix(" %");
	volumeOffsetSlider.addListener(this);
	addAndMakeVisible(volumeOffsetSlider);

	durationLabel.setFont(paramLabelFont());
	durationLabel.setText("Duration", dontSendNotification);
	durationLabel.setJustificationType(Justification::centred);
	durationLabel.setEditable(false, false, false);
	addAndMakeVisible(durationLabel);

	repeatLabel.setFont(paramLabelFont());
	repeatLabel.setText("Repeat", dontSendNotification);
	repeatLabel.setJustificationType(Justification::centred);
	repeatLabel.setEditable(false, false, false);
	addAndMakeVisible(repeatLabel);

	volumeOffsetLabel.setFont(paramLabelFont());
	volumeOffsetLabel.setText("Vol Offset", dontSendNotification);
	volumeOffsetLabel.setJustificationType(Justification::centred);
	volumeOffsetLabel.setEditable(false, false, false);
	addAndMakeVisible(volumeOffsetLabel);

	startTimerHz(30);
}

MidiEchoParametersComponent::~MidiEchoParametersComponent()
{}

void MidiEchoParametersComponent::paint(Graphics& g)
{
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

		String text("MIDI ECHO");
		g.setColour(TEXT_COLOUR());
		g.setFont(panelNameFont());
		g.drawText(text, textArea, Justification::centred, false);
	}
}

void MidiEchoParametersComponent::resized()
{
	float rowSize = 4.0f;
	float divide = 1.0f / rowSize;
	int compHeight = int((getHeight() - PANEL_NAME_HEIGHT) * divide);

	Rectangle<int> bounds = getLocalBounds(); // コンポーネント基準の値
	bounds.removeFromTop(PANEL_NAME_HEIGHT);
	
	{
		float alpha = isEditable() ? 1.0f : 0.4f;
		durationLabel.setAlpha(alpha);
		durationSlider.setAlpha(alpha);
		repeatLabel.setAlpha(alpha);
		repeatSlider.setAlpha(alpha);
		volumeOffsetLabel.setAlpha(alpha);
		volumeOffsetSlider.setAlpha(alpha);
	}
	{
		Rectangle<int> area = bounds.removeFromTop(compHeight);
		area.removeFromLeft(LABEL_WIDTH / 2);
		enableButton.setBounds(area.reduced(LOCAL_MARGIN));
	}
	{
		Rectangle<int> area = bounds.removeFromTop(compHeight);
		durationLabel.setBounds(area.removeFromLeft(LABEL_WIDTH).reduced(LOCAL_MARGIN));
		durationSlider.setBounds(area.reduced(LOCAL_MARGIN));

	}
	{
		Rectangle<int> area = bounds.removeFromTop(compHeight);
		repeatLabel.setBounds(area.removeFromLeft(LABEL_WIDTH).reduced(LOCAL_MARGIN));
		repeatSlider.setBounds(area.reduced(LOCAL_MARGIN));
	}
	{
		Rectangle<int> area = bounds.removeFromTop(compHeight);
		volumeOffsetLabel.setBounds(area.removeFromLeft(LABEL_WIDTH).reduced(LOCAL_MARGIN));
		volumeOffsetSlider.setBounds(area.reduced(LOCAL_MARGIN));
	}
}

void MidiEchoParametersComponent::timerCallback()
{
	enableButton.setToggleState(_midiEchoParamsPtr->IsEchoEnable->get(), dontSendNotification);
	durationSlider.setValue(_midiEchoParamsPtr->EchoDuration->get(), dontSendNotification);
	repeatSlider.setValue(_midiEchoParamsPtr->EchoRepeat->get(), dontSendNotification);
	volumeOffsetSlider.setValue(_midiEchoParamsPtr->VolumeOffset->get(), dontSendNotification);
}

void MidiEchoParametersComponent::sliderValueChanged(Slider* slider)
{
	if (slider == &durationSlider)
	{
		*_midiEchoParamsPtr->EchoDuration = (float)durationSlider.getValue();
	}
	else if (slider == &repeatSlider)
	{
		*_midiEchoParamsPtr->EchoRepeat = (int)repeatSlider.getValue();
	}
	else if (slider == &volumeOffsetSlider)
	{
		*_midiEchoParamsPtr->VolumeOffset = (float)volumeOffsetSlider.getValue();
	}
}

void MidiEchoParametersComponent::buttonClicked(Button* button)
{
	if (button == &enableButton)
	{
		*_midiEchoParamsPtr->IsEchoEnable = enableButton.getToggleState();
	}
	resized();
}

bool MidiEchoParametersComponent::isEditable()
{
	return _midiEchoParamsPtr->IsEchoEnable->get();
}

//----------------------------------------------------------------------------------------------------

RangeSlider::RangeSlider(WaveformMemoryParameters* waveformMemoryParams)
	: _waveformMemoryParamsPtr(waveformMemoryParams)
	, waveSampleSlider{}
{
	for (int i = 0; i < WAVESAMPLE_LENGTH; ++i)
	{
		waveSampleSlider[i].setRange(0, 15, 1.0);
		waveSampleSlider[i].setValue(7, dontSendNotification);
	}

	timerCallback();
	startTimerHz(30);
}
void RangeSlider::paint(Graphics& g)
{
	{
		float x = 0.0f, y = 0.0f, width = (float)getWidth(), height = (float)getHeight();
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
		g.setColour(TEXT_COLOUR());
		g.setFont(panelNameFont());
		g.drawText(text, textArea, Justification::centred, false);
	}

	//update slider Params
	for (auto* trail : trails)
	{
		float compWidth = (float)(getWidth()) - 12.0f; // 微調整値
		int compHeight = getHeight() - PANEL_NAME_HEIGHT - BUTTON_HEIGHT;

		int index = (int)(trail->currentPosition.x * (float)WAVESAMPLE_LENGTH / compWidth);
		if (index < 0)
		{
			index = 0;
		}
		else if (index >= 31)
		{
			index = 31;
		}
		float point = trail->currentPosition.y - PANEL_NAME_HEIGHT;
		int value = 15 - (int)(point * 16.0 / compHeight);
		waveSampleSlider[index].setValue(value, dontSendNotification);
		updateValue();
	}
	//repaint Sliders
	{
		float columnSize = (float)WAVESAMPLE_LENGTH;
		float rowSize = (float)16;
		float divide = 1.0f / columnSize;
		int compWidth = int((getWidth()) * divide);

		Rectangle<int> bounds = getLocalBounds();
		bounds.removeFromTop(PANEL_NAME_HEIGHT);
		bounds.removeFromBottom(BUTTON_HEIGHT);

		//Draw Scale Line
		for (int i = 1; i < 4; ++i)
		{
			float p_y = PANEL_NAME_HEIGHT + bounds.getHeight() * 0.25f * i;
			Line<float> line(0.0f, p_y, (float)(bounds.getWidth()), p_y);
			g.setColour(Colours::darkslateblue);
			g.drawLine(line, 1.0f);
		}
		for (int i = 1; i < 8; ++i)
		{
			float p_x = compWidth * i * 4.0f;
			Line<float> line(p_x, (float)PANEL_NAME_HEIGHT, p_x, (float)(bounds.getHeight() + PANEL_NAME_HEIGHT));
			g.setColour(Colours::darkslateblue);
			g.drawLine(line, 1.4f);
		}

		//Draw Slider
		for (int i = 0; i < WAVESAMPLE_LENGTH; ++i)
		{
			Rectangle<int> area = bounds.removeFromLeft(compWidth);
			int barHeight = int(bounds.getHeight() / rowSize * (waveSampleSlider[i].getMaximum() - waveSampleSlider[i].getValue()));
			area.removeFromTop(barHeight);
			//area.removeFromBottom(BUTTON_HEIGHT);
			g.setColour(Colours::lime);
			g.fillRect(area.reduced(1));
		}
	}
}

void RangeSlider::timerCallback()
{
	for (int i = 0; i < WAVESAMPLE_LENGTH; ++i)
	{
		waveSampleSlider[i].setValue(_waveformMemoryParamsPtr->WaveSamplesArray[i]->get(), dontSendNotification);
	}
}

void RangeSlider::updateValue()
{
	for (int i = 0; i < WAVESAMPLE_LENGTH; ++i)
	{
		*_waveformMemoryParamsPtr->WaveSamplesArray[i] = (int)waveSampleSlider[i].getValue();
	}
}

void RangeSlider::mouseDown(const MouseEvent& e)
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

void RangeSlider::mouseUp(const MouseEvent& e)
{
	auto* t = getTrail(e.source);

	if (t != nullptr)
	{
		trails.removeObject(t);
	}

	repaint();
}

void RangeSlider::mouseDrag(const MouseEvent& e)
{
	auto* t = getTrail(e.source);

	if (t == nullptr)
	{
		t = new Trail(e.source);
		t->path.startNewSubPath(e.position);
		trails.add(t);
	}
	else
	{
		t->pushPoint(e.position, e.mods, e.pressure);
	}
	repaint();
}

//----------------------------------------------------------------------------------------------------
WaveformMemoryParametersComponent::WaveformMemoryParametersComponent(WaveformMemoryParameters* waveformMemoryParams)
	: _waveformMemoryParamsPtr(waveformMemoryParams)
	//, waveSampleSlider{}
	, waveRangeSlider(waveformMemoryParams)
	, saveButton()
	, loadButton()
{
	saveButton.setButtonText("Save to File");
	saveButton.addListener(this);
	addAndMakeVisible(saveButton);

	loadButton.setButtonText("Load from File");
	loadButton.addListener(this);
	addAndMakeVisible(loadButton);

	waveRangeSlider.addMouseListener(this, true);
	addAndMakeVisible(waveRangeSlider);
}

WaveformMemoryParametersComponent::~WaveformMemoryParametersComponent()
{}

void WaveformMemoryParametersComponent::paint(Graphics& g)
{
	waveRangeSlider.paint(g);
}

void WaveformMemoryParametersComponent::resized()
{
	Rectangle<int> bounds = getLocalBounds();
	bounds.removeFromTop(PANEL_NAME_HEIGHT);
	
	waveRangeSlider.setBounds(getLocalBounds());

	{
		Rectangle<int> area = bounds.removeFromBottom(BUTTON_HEIGHT);
		saveButton.setBounds(area.removeFromLeft(area.getWidth() / 2).reduced(LOCAL_MARGIN));
		loadButton.setBounds(area.reduced(LOCAL_MARGIN));
	}
}	

void WaveformMemoryParametersComponent::buttonClicked(Button* button)
{
	if (button == &saveButton)
	{
		FileChooser fileSaver(
			"Save File As",
			preFilePath,
			"*.wfm"
		);

		if (fileSaver.browseForFileToSave(true))
		{
			File newFile(fileSaver.getResult());
			newFile.create();
			newFile.replaceWithText("");
			for (int i = 0; i < WAVESAMPLE_LENGTH; ++i)
			{
				newFile.appendText(std::to_string(*_waveformMemoryParamsPtr->WaveSamplesArray[i]));
				newFile.appendText(" ");
			}
			preFilePath = fileSaver.getResult();
		}
	}
	else if (button = &loadButton)
	{
		FileChooser fileLoader(
			"Load File From",
			preFilePath,
			"*.wfm"
		);

		if (fileLoader.browseForFileToOpen())
		{
			File waveformFile(fileLoader.getResult());
			std::string data = waveformFile.loadFileAsString().toStdString();
			int count = 0;
			for (const auto subStr : split(data, ' '))
			{
				*_waveformMemoryParamsPtr->WaveSamplesArray[count] = atoi(subStr.c_str());
				++count;
			}
			preFilePath = fileLoader.getResult();
		}
	}
	//timerCallback();
	//repaint();
}

bool WaveformMemoryParametersComponent::isInterestedInFileDrag(const StringArray & files)
{
	return true;
}

void WaveformMemoryParametersComponent::fileDragEnter(const StringArray &files, int x, int y)
{}

void WaveformMemoryParametersComponent::fileDragMove(const StringArray &files, int x, int y)
{}

void WaveformMemoryParametersComponent::fileDragExit(const StringArray &files)
{}

void WaveformMemoryParametersComponent::filesDropped(const StringArray &files, int x, int y)
{
	std::string filePath = files.begin()->toStdString();

	if (filePath.find(".wfm"))
	{
		File waveformFile(filePath);
		std::string data = waveformFile.loadFileAsString().toStdString();
		int count = 0;
		for (const auto subStr : split(data, ' ')) {
			*_waveformMemoryParamsPtr->WaveSamplesArray[count] = atoi(subStr.c_str());
			++count;
		}
	}
	//timerCallback();
	//repaint();
}

//----------------------------------------------------------------------------------------------------

FilterParametersComponent::FilterParametersComponent(FilterParameters* filterParams)
	: _filterParamsPtr(filterParams)
	, hiCutSwitch()
	, lowCutSwitch()
	, hicutFreqSlider(Slider::SliderStyle::LinearHorizontal, Slider::TextEntryBoxPosition::TextBoxLeft)
	, lowcutFreqSlider(Slider::SliderStyle::LinearHorizontal, Slider::TextEntryBoxPosition::TextBoxLeft)
{
	hiCutSwitch.setButtonText("HiCut: ON / OFF");
	hiCutSwitch.setToggleState(_filterParamsPtr->HicutEnable->get(), dontSendNotification);
	hiCutSwitch.addListener(this);
	addAndMakeVisible(hiCutSwitch);

	lowCutSwitch.setButtonText("LowCut: ON / OFF");
	lowCutSwitch.setToggleState(_filterParamsPtr->LowcutEnable->get(), dontSendNotification);
	lowCutSwitch.addListener(this);
	addAndMakeVisible(lowCutSwitch);

	hicutFreqSlider.setRange(_filterParamsPtr->HicutFreq->range.start, _filterParamsPtr->HicutFreq->range.end, 0.1f);
	hicutFreqSlider.setValue(_filterParamsPtr->HicutFreq->get(), dontSendNotification);
	hicutFreqSlider.setTextValueSuffix(" Hz");
	hicutFreqSlider.setSkewFactorFromMidPoint(2000.0f);
	hicutFreqSlider.addListener(this);
	addAndMakeVisible(hicutFreqSlider);

	lowcutFreqSlider.setRange(_filterParamsPtr->LowcutFreq->range.start, _filterParamsPtr->LowcutFreq->range.end, 0.1f);
	lowcutFreqSlider.setValue(_filterParamsPtr->LowcutFreq->get(), dontSendNotification);
	lowcutFreqSlider.setTextValueSuffix(" Hz");
	lowcutFreqSlider.setSkewFactorFromMidPoint(2000.0f);
	lowcutFreqSlider.addListener(this);
	addAndMakeVisible(lowcutFreqSlider);

	hicutFreqLabel.setFont(paramLabelFont());
	hicutFreqLabel.setText("HiCut", dontSendNotification);
	hicutFreqLabel.setJustificationType(Justification::centred);
	hicutFreqLabel.setEditable(false, false, false);
	addAndMakeVisible(hicutFreqLabel);

	lowcutFreqLabel.setFont(paramLabelFont());
	lowcutFreqLabel.setText("LowCut", dontSendNotification);
	lowcutFreqLabel.setJustificationType(Justification::centred);
	lowcutFreqLabel.setEditable(false, false, false);
	addAndMakeVisible(lowcutFreqLabel);

	startTimerHz(30);
}

FilterParametersComponent::~FilterParametersComponent()
{}

void FilterParametersComponent::paint(Graphics& g)
{
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

		String text("FILTER");
		g.setColour(TEXT_COLOUR());
		g.setFont(panelNameFont());
		g.drawText(text, textArea, Justification::centred, false);
	}
}

void FilterParametersComponent::resized()
{
	float rowSize = 4.0f;
	float divide = 1.0f / rowSize;
	int compHeight = int((getHeight() - PANEL_NAME_HEIGHT) * divide);

	Rectangle<int> bounds = getLocalBounds(); // コンポーネント基準の値
	bounds.removeFromTop(PANEL_NAME_HEIGHT);

	{
		float alpha = _filterParamsPtr->HicutEnable->get() ? 1.0f : 0.4f;
		hicutFreqLabel.setAlpha(alpha);
		hicutFreqSlider.setAlpha(alpha);
	}
	{
		float alpha = _filterParamsPtr->LowcutEnable->get() ? 1.0f : 0.4f;
		lowcutFreqLabel.setAlpha(alpha);
		lowcutFreqSlider.setAlpha(alpha);
	}
	{
		Rectangle<int> area = bounds.removeFromTop(compHeight);
		area.removeFromLeft(LABEL_WIDTH / 2);
		hiCutSwitch.setBounds(area.reduced(LOCAL_MARGIN));
	}
	{
		Rectangle<int> area = bounds.removeFromTop(compHeight);
		hicutFreqLabel.setBounds(area.removeFromLeft(LABEL_WIDTH).reduced(LOCAL_MARGIN));
		hicutFreqSlider.setBounds(area.reduced(LOCAL_MARGIN));
	}
	{
		Rectangle<int> area = bounds.removeFromTop(compHeight);
		area.removeFromLeft(LABEL_WIDTH / 2);
		lowCutSwitch.setBounds(area.reduced(LOCAL_MARGIN));
	}
	{
		Rectangle<int> area = bounds.removeFromTop(compHeight);
		lowcutFreqLabel.setBounds(area.removeFromLeft(LABEL_WIDTH).reduced(LOCAL_MARGIN));
		lowcutFreqSlider.setBounds(area.reduced(LOCAL_MARGIN));
	}
}

void FilterParametersComponent::timerCallback()
{
	hicutFreqSlider.setValue(_filterParamsPtr->HicutFreq->get(), dontSendNotification);
	lowcutFreqSlider.setValue(_filterParamsPtr->LowcutFreq->get(), dontSendNotification);
}

void FilterParametersComponent::sliderValueChanged(Slider* slider)
{
	if (slider == &hicutFreqSlider)
	{
		*_filterParamsPtr->HicutFreq = (float)hicutFreqSlider.getValue();
	}
	else if (slider == &lowcutFreqSlider)
	{
		*_filterParamsPtr->LowcutFreq = (float)lowcutFreqSlider.getValue();
	}
}

void FilterParametersComponent::buttonClicked(Button* button)
{
	if (button == &hiCutSwitch)
	{
		*_filterParamsPtr->HicutEnable = hiCutSwitch.getToggleState();
	}
	else if (button == &lowCutSwitch)
	{
		*_filterParamsPtr->LowcutEnable = lowCutSwitch.getToggleState();
	}
	resized();
}

//----------------------------------------------------------------------------------------------------

