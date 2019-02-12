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

void paintHeader(Graphics& g, Rectangle<int> bounds, std::string text)
{
	{
		float x = 0.0f, y = 0.0f, width = (float)bounds.getWidth(), height = (float)bounds.getHeight();
		g.setColour(PANEL_COLOUR());
		g.fillRoundedRectangle(x, y, width, height, 10.0f);
	}
	{
		float x = 0.0f, y = 0.0f, width = (float)bounds.getWidth(), height = PANEL_NAME_HEIGHT;
		g.setColour(HEADER_COLOUR());
		g.fillRoundedRectangle(x, y, width, height, 10.0f);
	}
	{
		Rectangle<int> textArea = bounds.removeFromTop(PANEL_NAME_HEIGHT).reduced(LOCAL_MARGIN);

		g.setColour(TEXT_COLOUR());
		g.setFont(panelNameFont());
		g.drawText(text, textArea, Justification::centred, false);
	}
}

ChipOscillatorComponent::ChipOscillatorComponent(ChipOscillatorParameters* oscParams)
	: _oscParamsPtr(oscParams)
	, waveTypeSelector("waveForm")
	, volumeLevelSlider("Volume", "dB", _oscParamsPtr->VolumeLevel, 0.01f)
	, attackSlider("Attack", "sec", _oscParamsPtr->Attack, 0.0001f,1.0f)
	, decaySlider("Decay", "sec", _oscParamsPtr->Decay, 0.0001f, 1.0f)
	, sustainSlider("Sustain", "", _oscParamsPtr->Sustain, 0.0001f)
	, releaseSlider("Release", "sec", _oscParamsPtr->Release, 0.0001f, 1.0f)
{
	waveTypeSelector.addItemList(_oscParamsPtr->OscWaveType->getAllValueStrings(), 1);
	waveTypeSelector.setSelectedItemIndex(_oscParamsPtr->OscWaveType->getIndex(), dontSendNotification);
	waveTypeSelector.setJustificationType(Justification::centred);
	waveTypeSelector.addListener(this);
	addAndMakeVisible(waveTypeSelector);

	waveTypeSelectorLabel.setFont(paramLabelFont());
	waveTypeSelectorLabel.setText("Type", dontSendNotification);
	waveTypeSelectorLabel.setJustificationType(Justification::centred);
	waveTypeSelectorLabel.setEditable(false, false, false);
	addAndMakeVisible(waveTypeSelectorLabel);

	volumeLevelSlider.slider.addListener(this);
	attackSlider.slider.addListener(this);
	decaySlider.slider.addListener(this);
	sustainSlider.slider.addListener(this);
	releaseSlider.slider.addListener(this);

	addAndMakeVisible(volumeLevelSlider);
	addAndMakeVisible(attackSlider);
	addAndMakeVisible(decaySlider);
	addAndMakeVisible(sustainSlider);
	addAndMakeVisible(releaseSlider);

	startTimerHz(30);
}

ChipOscillatorComponent::~ChipOscillatorComponent()
{}

void ChipOscillatorComponent::paint(Graphics& g)
{
	paintHeader(g, getLocalBounds(), "OSCILLATOR");
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
		volumeLevelSlider.setBounds(area.reduced(LOCAL_MARGIN));
	}
	{
		Rectangle<int> area = bounds.removeFromTop(compHeight);
		attackSlider.setBounds(area.reduced(LOCAL_MARGIN));
	}
	{
		Rectangle<int> area = bounds.removeFromTop(compHeight);
		decaySlider.setBounds(area.reduced(LOCAL_MARGIN));
	}
	{
		Rectangle<int> area = bounds.removeFromTop(compHeight);
		sustainSlider.setBounds(area.reduced(LOCAL_MARGIN));
	}
	{
		Rectangle<int> area = bounds.removeFromTop(compHeight);
		releaseSlider.setBounds(area.reduced(LOCAL_MARGIN));
	}
}

void ChipOscillatorComponent::timerCallback()
{
	waveTypeSelector.setSelectedItemIndex(_oscParamsPtr->OscWaveType->getIndex(), dontSendNotification);
	volumeLevelSlider.setValue(_oscParamsPtr->VolumeLevel->get());
	attackSlider.setValue(_oscParamsPtr->Attack->get());
	decaySlider.setValue(_oscParamsPtr->Decay->get());
	sustainSlider.setValue(_oscParamsPtr->Sustain->get());
	releaseSlider.setValue(_oscParamsPtr->Release->get());
}

void ChipOscillatorComponent::sliderValueChanged(Slider* slider)
{
	if (slider == &volumeLevelSlider.slider)
	{
		*_oscParamsPtr->VolumeLevel = (float)volumeLevelSlider.getValue();
	}
	else if (slider == &attackSlider.slider)
	{
		*_oscParamsPtr->Attack = (float)attackSlider.getValue();
	}
	else if (slider == &decaySlider.slider)
	{
		*_oscParamsPtr->Decay = (float)decaySlider.getValue();
	}
	else if (slider == &sustainSlider.slider)
	{
		*_oscParamsPtr->Sustain = (float)sustainSlider.getValue();
	}
	else if (slider == &releaseSlider.slider)
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
	, timeSlider("Speed", "sec", _sweepParamsPtr->SweepTime, 0.01f, 1.0f)
{
	sweepSwitchSelector.addItemList(_sweepParamsPtr->SweepSwitch ->getAllValueStrings(), 1);
	sweepSwitchSelector.setSelectedItemIndex(_sweepParamsPtr->SweepSwitch->getIndex(), dontSendNotification);
	sweepSwitchSelector.setJustificationType(Justification::centred);
	sweepSwitchSelector.addListener(this);
	addAndMakeVisible(sweepSwitchSelector);

	switchLabel.setFont(paramLabelFont());
	switchLabel.setText("Switch", dontSendNotification);
	switchLabel.setJustificationType(Justification::centred);
	switchLabel.setEditable(false, false, false);
	addAndMakeVisible(switchLabel);
	
	timeSlider.slider.addListener(this);
	addAndMakeVisible(timeSlider);

	startTimerHz(30);
}

SweepParametersComponent::~SweepParametersComponent()
{}

void SweepParametersComponent::paint(Graphics& g)
{
	paintHeader(g, getLocalBounds(), "PITCH SWEEP");
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
		timeSlider.setAlpha(alpha);
	}
	{
		Rectangle<int> area = bounds.removeFromTop(compHeight);
		switchLabel.setBounds(area.removeFromLeft(LABEL_WIDTH).reduced(LOCAL_MARGIN));
		sweepSwitchSelector.setBounds(area.reduced(LOCAL_MARGIN * 2));
	}
	{
		Rectangle<int> area = bounds.removeFromTop(compHeight);
		timeSlider.setBounds(area.reduced(LOCAL_MARGIN));
	}
}

void SweepParametersComponent::timerCallback()
{
	sweepSwitchSelector.setSelectedItemIndex(_sweepParamsPtr->SweepSwitch->getIndex(), dontSendNotification);
	timeSlider.setValue(_sweepParamsPtr->SweepTime->get());
}

void SweepParametersComponent::sliderValueChanged(Slider* slider)
{
	if (slider == &timeSlider.slider)
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
	, amountSlider("Depth", "HarfTone", _vibratoParamsPtr->VibratoAmount, 0.01f, 2.0f)
	, speedSlider("Speed", "hz", _vibratoParamsPtr->VibratoSpeed, 0.001f, 2.0f)
	, attackTimeSlider("Attack", "sec", _vibratoParamsPtr->VibratoAttackTime, 0.001f, 2.0f)
{
	enableButton.setButtonText("ON / OFF");
	enableButton.setToggleState(_vibratoParamsPtr->VibratoEnable->get(), dontSendNotification);
	enableButton.addListener(this);
	addAndMakeVisible(enableButton);

	amountSlider.slider.addListener(this);
	addAndMakeVisible(amountSlider);

	speedSlider.slider.addListener(this);
	addAndMakeVisible(speedSlider);

	attackTimeSlider.slider.addListener(this);
	addAndMakeVisible(attackTimeSlider);

	startTimerHz(30);
}

VibratoParametersComponent::~VibratoParametersComponent()
{}

void VibratoParametersComponent::paint(Graphics& g)
{
	paintHeader(g, getLocalBounds(), "VIBRATO");
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
		amountSlider.setAlpha(alpha);
		speedSlider.setAlpha(alpha);
		attackTimeSlider.setAlpha(alpha);
	}
	{
		Rectangle<int> area = bounds.removeFromTop(compHeight);
		area.removeFromLeft(LABEL_WIDTH / 2);
		enableButton.setBounds(area.reduced(LOCAL_MARGIN));
	}
	{
		Rectangle<int> area = bounds.removeFromTop(compHeight);
		amountSlider.setBounds(area.reduced(LOCAL_MARGIN));

	}
	{
		Rectangle<int> area = bounds.removeFromTop(compHeight);
		speedSlider.setBounds(area.reduced(LOCAL_MARGIN));
	}
	{
		Rectangle<int> area = bounds.removeFromTop(compHeight);
		attackTimeSlider.setBounds(area.reduced(LOCAL_MARGIN));
	}
}

void VibratoParametersComponent::timerCallback()
{
	enableButton.setToggleState(_vibratoParamsPtr->VibratoEnable->get(), dontSendNotification);
	amountSlider.setValue(_vibratoParamsPtr->VibratoAmount->get());
	speedSlider.setValue(_vibratoParamsPtr->VibratoSpeed->get());
	attackTimeSlider.setValue(_vibratoParamsPtr->VibratoAttackTime->get());
}

void VibratoParametersComponent::sliderValueChanged(Slider* slider)
{
	if (slider == &amountSlider.slider)
	{
		*_vibratoParamsPtr->VibratoAmount = (float)amountSlider.getValue();
	}
	else if (slider == &speedSlider.slider)
	{
		*_vibratoParamsPtr->VibratoSpeed = (float)speedSlider.getValue();
	}
	else if (slider == &attackTimeSlider.slider)
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
	, portaTimeSlider("PortaTime", "sec", _voicingParamsPtr->PortaTime, 0.01f, 0.5f)
{
	voicingTypeSelector.addItemList(_voicingParamsPtr->VoicingSwitch->getAllValueStrings(), 1);
	voicingTypeSelector.setSelectedItemIndex(_voicingParamsPtr->VoicingSwitch->getIndex(), dontSendNotification);
	voicingTypeSelector.setJustificationType(Justification::centred);
	voicingTypeSelector.addListener(this);
	addAndMakeVisible(voicingTypeSelector);

	portaTimeSlider.slider.addListener(this);
	addAndMakeVisible(portaTimeSlider);

	voicingTypeSelectorLabel.setFont(paramLabelFont());
	voicingTypeSelectorLabel.setText("Type", dontSendNotification);
	voicingTypeSelectorLabel.setJustificationType(Justification::centred);
	voicingTypeSelectorLabel.setEditable(false, false, false);
	addAndMakeVisible(voicingTypeSelectorLabel);

	startTimerHz(30);
}

VoicingParametersComponent::~VoicingParametersComponent()
{}

void VoicingParametersComponent::paint(Graphics& g)
{
	paintHeader(g, getLocalBounds(), "VOICING");
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
		portaTimeSlider.setAlpha(alpha);
	}
	{
		Rectangle<int> area = bounds.removeFromTop(compHeight);
		voicingTypeSelectorLabel.setBounds(area.removeFromLeft(LABEL_WIDTH).reduced(LOCAL_MARGIN));
		voicingTypeSelector.setBounds(area.reduced(LOCAL_MARGIN * 2));
	}
	{
		Rectangle<int> area = bounds.removeFromTop(compHeight);
		portaTimeSlider.setBounds(area.reduced(LOCAL_MARGIN));
	}
}

void VoicingParametersComponent::timerCallback()
{
	voicingTypeSelector.setSelectedItemIndex(_voicingParamsPtr->VoicingSwitch->getIndex(), dontSendNotification);
	portaTimeSlider.setValue(_voicingParamsPtr->PortaTime->get());
}

void VoicingParametersComponent::sliderValueChanged(Slider* slider)
{
	if (slider == &portaTimeSlider.slider)
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
	, pitchStandardSlider("Tunes", "", _optionsParamsPtr->PitchStandard)
	, pitchBendRangeSlider("PB Range", "", _optionsParamsPtr->PitchBendRange)
{	
	pitchStandardSlider.slider.addListener(this);
	addAndMakeVisible(pitchStandardSlider);
	pitchBendRangeSlider.slider.addListener(this);
	addAndMakeVisible(pitchBendRangeSlider);

	startTimerHz(30);
}

OptionsParametersComponent::~OptionsParametersComponent()
{}

void OptionsParametersComponent::paint(Graphics& g)
{
	paintHeader(g, getLocalBounds(), "OPTIONS");
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
		pitchStandardSlider.setBounds(area.reduced(LOCAL_MARGIN));
	}
	{
		Rectangle<int> area = bounds.removeFromTop(compHeight);
		pitchBendRangeSlider.setBounds(area.reduced(LOCAL_MARGIN));
	}
}

void OptionsParametersComponent::timerCallback()
{
	velocitySenseButton.setToggleState(_optionsParamsPtr->IsVelocitySense->get(), dontSendNotification);
	pitchStandardSlider.setValue(_optionsParamsPtr->PitchStandard->get());
	pitchBendRangeSlider.setValue(_optionsParamsPtr->PitchBendRange->get());
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
	if (slider == &pitchStandardSlider.slider)
	{
		*_optionsParamsPtr->PitchStandard = (int)pitchStandardSlider.getValue();
	}
	else if (slider == &pitchBendRangeSlider.slider)
	{
		*_optionsParamsPtr->PitchBendRange = (int)pitchBendRangeSlider.getValue();
	}
}

//----------------------------------------------------------------------------------------------------

MidiEchoParametersComponent::MidiEchoParametersComponent(MidiEchoParameters* midiEchoParams)
	:_midiEchoParamsPtr(midiEchoParams)
	, enableButton("MidiEcho-Enable")
	, durationSlider("Duration", "sec", _midiEchoParamsPtr->EchoDuration, 0.01f, 0.5f)
	, repeatSlider("Repeat", "", _midiEchoParamsPtr->EchoRepeat->get(), _midiEchoParamsPtr->EchoRepeat->getRange().getStart(), _midiEchoParamsPtr->EchoRepeat->getRange().getEnd() , 1.0f)
	, volumeOffsetSlider("Vol Offset", "%", _midiEchoParamsPtr->VolumeOffset, 0.1f, 100.0f)
{
	enableButton.setButtonText("ON / OFF");
	enableButton.setToggleState(_midiEchoParamsPtr->IsEchoEnable->get(), dontSendNotification);
	enableButton.addListener(this);
	addAndMakeVisible(enableButton);

	durationSlider.slider.addListener(this);
	addAndMakeVisible(durationSlider);

	repeatSlider.slider.addListener(this);
	addAndMakeVisible(repeatSlider);

	volumeOffsetSlider.slider.addListener(this);
	addAndMakeVisible(volumeOffsetSlider);

	startTimerHz(30);
}

MidiEchoParametersComponent::~MidiEchoParametersComponent()
{}

void MidiEchoParametersComponent::paint(Graphics& g)
{
	paintHeader(g, getLocalBounds(), "MIDI ECHO");
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
		durationSlider.setAlpha(alpha);
		repeatSlider.setAlpha(alpha);
		volumeOffsetSlider.setAlpha(alpha);
	}
	{
		Rectangle<int> area = bounds.removeFromTop(compHeight);
		area.removeFromLeft(LABEL_WIDTH / 2);
		enableButton.setBounds(area.reduced(LOCAL_MARGIN));
	}
	{
		Rectangle<int> area = bounds.removeFromTop(compHeight);
		durationSlider.setBounds(area.reduced(LOCAL_MARGIN));

	}
	{
		Rectangle<int> area = bounds.removeFromTop(compHeight);
		repeatSlider.setBounds(area.reduced(LOCAL_MARGIN));
	}
	{
		Rectangle<int> area = bounds.removeFromTop(compHeight);
		volumeOffsetSlider.setBounds(area.reduced(LOCAL_MARGIN));
	}
}

void MidiEchoParametersComponent::timerCallback()
{
	enableButton.setToggleState(_midiEchoParamsPtr->IsEchoEnable->get(), dontSendNotification);
	durationSlider.setValue(_midiEchoParamsPtr->EchoDuration->get());
	repeatSlider.setValue(_midiEchoParamsPtr->EchoRepeat->get());
	volumeOffsetSlider.setValue(_midiEchoParamsPtr->VolumeOffset->get());
}

void MidiEchoParametersComponent::sliderValueChanged(Slider* slider)
{
	if (slider == &durationSlider.slider)
	{
		*_midiEchoParamsPtr->EchoDuration = (float)durationSlider.getValue();
	}
	else if (slider == &repeatSlider.slider)
	{
		*_midiEchoParamsPtr->EchoRepeat = (int)repeatSlider.getValue();
	}
	else if (slider == &volumeOffsetSlider.slider)
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

WaveformMemoryParametersComponent::WaveformMemoryParametersComponent(WaveformMemoryParameters* waveformMemoryParams)
	: _waveformMemoryParamsPtr(waveformMemoryParams)
	, waveSampleSlider{}
	, saveButton()
	, loadButton()
{
	for (int i = 0; i < WAVESAMPLE_LENGTH; ++i)
	{
		waveSampleSlider[i].setRange(0, 15, 1.0);
		waveSampleSlider[i].setValue(7, dontSendNotification);
	}

	saveButton.setButtonText("Save to File");
	saveButton.addListener(this);
	addAndMakeVisible(saveButton);

	loadButton.setButtonText("Load from File");
	loadButton.addListener(this);
	addAndMakeVisible(loadButton);

	timerCallback();
	startTimerHz(30);
}

WaveformMemoryParametersComponent::~WaveformMemoryParametersComponent()
{}

void WaveformMemoryParametersComponent::paint(Graphics& g)
{
	paintHeader(g, getLocalBounds(), "WAVEFORM MEMORY");

	//update slider Params
	for (auto* trail : trails)
	{	
		float compWidth = (float)(getWidth()) -12.0f; // 微調整値
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
			Line<float> line(p_x, (float)PANEL_NAME_HEIGHT , p_x, (float)(bounds.getHeight() + PANEL_NAME_HEIGHT));
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

void WaveformMemoryParametersComponent::resized()
{
	Rectangle<int> bounds = getLocalBounds();
	bounds.removeFromTop(PANEL_NAME_HEIGHT);

	{
		Rectangle<int> area = bounds.removeFromBottom(BUTTON_HEIGHT);
		saveButton.setBounds(area.removeFromLeft(area.getWidth() / 2).reduced(LOCAL_MARGIN));
		loadButton.setBounds(area.reduced(LOCAL_MARGIN));
	}
}

void WaveformMemoryParametersComponent::timerCallback()
{
	for (int i = 0; i < WAVESAMPLE_LENGTH; ++i)
	{
		waveSampleSlider[i].setValue(_waveformMemoryParamsPtr->WaveSamplesArray[i]->get(), dontSendNotification);
	}
}

void WaveformMemoryParametersComponent::updateValue()
{
	for (int i = 0; i < WAVESAMPLE_LENGTH; ++i)
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
	auto* t = getTrail(e.source);

	if (t != nullptr)
	{
		trails.removeObject(t);
	}

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
	else
	{
		t->pushPoint(e.position, e.mods, e.pressure);
	}
	repaint();
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
	timerCallback();
	repaint();
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
	timerCallback();
	repaint();
}

//----------------------------------------------------------------------------------------------------

FilterParametersComponent::FilterParametersComponent(FilterParameters* filterParams)
	: _filterParamsPtr(filterParams)
	, hiCutSwitch()
	, lowCutSwitch()
	, hicutFreqSlider("hicut", "Hz", _filterParamsPtr->HicutFreq, 0.1f, 2000.0f)
	, lowcutFreqSlider("lowcut", "Hz", _filterParamsPtr->LowcutFreq, 0.1f, 2000.0f)
{
	hiCutSwitch.setButtonText("HiCut: ON / OFF");
	hiCutSwitch.setToggleState(_filterParamsPtr->HicutEnable->get(), dontSendNotification);
	hiCutSwitch.addListener(this);
	addAndMakeVisible(hiCutSwitch);

	lowCutSwitch.setButtonText("LowCut: ON / OFF");
	lowCutSwitch.setToggleState(_filterParamsPtr->LowcutEnable->get(), dontSendNotification);
	lowCutSwitch.addListener(this);
	addAndMakeVisible(lowCutSwitch);

	hicutFreqSlider.slider.addListener(this);
	lowcutFreqSlider.slider.addListener(this);
	addAndMakeVisible(hicutFreqSlider);
	addAndMakeVisible(lowcutFreqSlider);

	startTimerHz(30);
}

FilterParametersComponent::~FilterParametersComponent()
{}

void FilterParametersComponent::paint(Graphics& g)
{
	paintHeader(g, getLocalBounds(), "FILTER");
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
		hicutFreqSlider.setAlpha(alpha);
	}
	{
		float alpha = _filterParamsPtr->LowcutEnable->get() ? 1.0f : 0.4f;
		lowcutFreqSlider.setAlpha(alpha);
	}

	{
		Rectangle<int> area = bounds.removeFromTop(compHeight);
		area.removeFromLeft(LABEL_WIDTH / 2);
		hiCutSwitch.setBounds(area.reduced(LOCAL_MARGIN));
	}
	{
		Rectangle<int> area = bounds.removeFromTop(compHeight);
		hicutFreqSlider.setBounds(area);
	}
	{
		Rectangle<int> area = bounds.removeFromTop(compHeight);
		area.removeFromLeft(LABEL_WIDTH / 2);
		lowCutSwitch.setBounds(area.reduced(LOCAL_MARGIN));
	}
	{
		Rectangle<int> area = bounds.removeFromTop(compHeight);
		lowcutFreqSlider.setBounds(area);
	}
}

void FilterParametersComponent::timerCallback()
{
	hicutFreqSlider.setValue(_filterParamsPtr->HicutFreq->get());
	lowcutFreqSlider.setValue(_filterParamsPtr->LowcutFreq->get());
}

void FilterParametersComponent::sliderValueChanged(Slider* slider)
{
	if (slider == &hicutFreqSlider.slider)
	{
		*_filterParamsPtr->HicutFreq = (float)hicutFreqSlider.getValue();
	}
	else if (slider == &lowcutFreqSlider.slider)
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