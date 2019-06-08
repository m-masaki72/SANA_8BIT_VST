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
	const std::int32_t PANEL_NAME_HEIGHT = 32;
	const std::int32_t LOCAL_MARGIN = 2;
	const std::int32_t LABEL_WIDTH = 80;

	const Font panelNameFont() { return Font(PANEL_NAME_FONT_SIZE, Font::plain).withTypefaceStyle("Italic"); };
	const Font paramLabelFont() { return Font(PARAM_LABEL_FONT_SIZE, Font::plain).withTypefaceStyle("Regular"); };

	static File preFilePath = File::getSpecialLocation(File::userDesktopDirectory);
}

std::vector<std::string> split(std::string str, char del)
{
	auto first = 0;
	auto last = str.find_first_of(del);

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

void saveWaveFile(WaveformMemoryParameters *_waveformMemoryParamsPtr)
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
		for (auto i = 0; i < WAVESAMPLE_LENGTH; ++i)
		{
			newFile.appendText(std::to_string(*_waveformMemoryParamsPtr->WaveSamplesArray[i]));
			newFile.appendText(" ");
		}
		preFilePath = fileSaver.getResult();
	}
}
void loadWaveFile(WaveformMemoryParameters *_waveformMemoryParamsPtr)
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
		auto count = 0;
		for (const auto subStr : split(data, ' '))
		{
			*_waveformMemoryParamsPtr->WaveSamplesArray[count] = atoi(subStr.c_str());
			++count;
		}
		preFilePath = fileLoader.getResult();
	}
}

ChipOscillatorComponent::ChipOscillatorComponent(ChipOscillatorParameters* oscParams)
	: _oscParamsPtr(oscParams)
	, waveTypeSelector("waveForm", _oscParamsPtr->OscWaveType, this)
	, volumeLevelSlider("Volume", "dB", _oscParamsPtr->VolumeLevel, this, 0.01f)
	, attackSlider("Attack", "sec", _oscParamsPtr->Attack, this, 0.0001f, 1.0f)
	, decaySlider("Decay", "sec", _oscParamsPtr->Decay, this, 0.0001f, 1.0f)
	, sustainSlider("Sustain", "", _oscParamsPtr->Sustain, this, 0.0001f)
	, releaseSlider("Release", "sec", _oscParamsPtr->Release, this, 0.0001f, 1.0f)
{
	addAndMakeVisible(waveTypeSelector);
	addAndMakeVisible(volumeLevelSlider);
	addAndMakeVisible(attackSlider);
	addAndMakeVisible(decaySlider);
	addAndMakeVisible(sustainSlider);
	addAndMakeVisible(releaseSlider);

}

void ChipOscillatorComponent::paint(Graphics& g)
{
	paintHeader(g, getLocalBounds(), "OSCILLATOR");
}

void ChipOscillatorComponent::resized()
{
	float rowSize = 6.0f;
	float divide = 1.0f / rowSize;
	std::int32_t compHeight = std::int32_t((getHeight() - PANEL_NAME_HEIGHT) * divide);

	Rectangle<int> bounds = getLocalBounds(); // コンポーネント基準の値
	bounds.removeFromTop(PANEL_NAME_HEIGHT);
	
	waveTypeSelector.setBounds(bounds.removeFromTop(compHeight));
	volumeLevelSlider.setBounds(bounds.removeFromTop(compHeight));
	attackSlider.setBounds(bounds.removeFromTop(compHeight));
	decaySlider.setBounds(bounds.removeFromTop(compHeight));
	sustainSlider.setBounds(bounds.removeFromTop(compHeight));
	releaseSlider.setBounds(bounds.removeFromTop(compHeight));
}

void ChipOscillatorComponent::timerCallback()
{
	waveTypeSelector.setSelectedItemIndex(_oscParamsPtr->OscWaveType->getIndex());
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
	if (comboBoxThatHasChanged == &waveTypeSelector.selector)
	{
		*_oscParamsPtr->OscWaveType = waveTypeSelector.getSelectedItemIndex();
	}
}

//----------------------------------------------------------------------------------------------------

SweepParametersComponent::SweepParametersComponent(SweepParameters* sweepParams)
	:_sweepParamsPtr(sweepParams)
	, sweepSwitchSelector("Sweep-Swetch", _sweepParamsPtr->SweepSwitch, this)
	, timeSlider("Speed", "sec", _sweepParamsPtr->SweepTime, this, 0.01f, 1.0f)
{
	addAndMakeVisible(sweepSwitchSelector);	
	addAndMakeVisible(timeSlider);
}

void SweepParametersComponent::paint(Graphics& g)
{
	paintHeader(g, getLocalBounds(), "PITCH SWEEP");
}

void SweepParametersComponent::resized()
{
	float rowSize = 2.0f;
	float divide = 1.0f / rowSize;
	std::int32_t compHeight = std::int32_t((getHeight() - PANEL_NAME_HEIGHT) * divide);

	Rectangle<int> bounds = getLocalBounds(); // コンポーネント基準の値
	bounds.removeFromTop(PANEL_NAME_HEIGHT);

	{
		float alpha = isEditable() ? 1.0f : 0.4f;
		timeSlider.setAlpha(alpha);
	}
	sweepSwitchSelector.setBounds(bounds.removeFromTop(compHeight));
	timeSlider.setBounds(bounds.removeFromTop(compHeight));
}

void SweepParametersComponent::timerCallback()
{
	sweepSwitchSelector.setSelectedItemIndex(_sweepParamsPtr->SweepSwitch->getIndex());
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
	if (comboBoxThatHasChanged == &sweepSwitchSelector.selector)
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
	, enableSwitch("Vibrato-Switch", _vibratoParamsPtr->VibratoEnable, this)
	, attackDeleySwitch("Attack-Deley-Switch", _vibratoParamsPtr->VibratoAttackDeleySwitch, this)
	, amountSlider("Depth", "HarfTone", _vibratoParamsPtr->VibratoAmount, this, 0.01f, 2.0f)
	, speedSlider("Speed", "hz", _vibratoParamsPtr->VibratoSpeed, this, 0.001f, 10.0f)
	, attackDeleyTimeSlider("Attack", "sec", _vibratoParamsPtr->VibratoAttackTime, this, 0.001f, 2.0f)
{
	addAndMakeVisible(enableSwitch);
	addAndMakeVisible(attackDeleySwitch);
	addAndMakeVisible(amountSlider);
	addAndMakeVisible(speedSlider);
	addAndMakeVisible(attackDeleyTimeSlider);

	// 切り替えSwitchのカスタマイズ
	{
		attackDeleySwitch.button.setButtonText("Attack / Deley");
	}
}

void VibratoParametersComponent::paint(Graphics& g)
{
	paintHeader(g, getLocalBounds(), "VIBRATO");
}

void VibratoParametersComponent::resized()
{
	float rowSize = 4.0f;
	float divide = 1.0f / rowSize;
	std::int32_t compHeight = std::int32_t((getHeight() - PANEL_NAME_HEIGHT) * divide);

	Rectangle<int> bounds = getLocalBounds(); // コンポーネント基準の値
	bounds.removeFromTop(PANEL_NAME_HEIGHT);

	{
		float alpha = isEditable() ? 1.0f : 0.4f;
		amountSlider.setAlpha(alpha);
		speedSlider.setAlpha(alpha);
		attackDeleyTimeSlider.setAlpha(alpha);
	}
	{
		auto b = bounds.removeFromTop(compHeight);
		enableSwitch.setBounds(b.removeFromLeft(b.getWidth() * 0.3f));
		attackDeleySwitch.setBounds(b);
	}
	amountSlider.setBounds(bounds.removeFromTop(compHeight));
	speedSlider.setBounds(bounds.removeFromTop(compHeight));
	attackDeleyTimeSlider.setBounds(bounds.removeFromTop(compHeight));

	//切り替えスイッチ処理
	if (_vibratoParamsPtr->VibratoAttackDeleySwitch->get() == true)
	{
		attackDeleyTimeSlider.label.setText("Attack", dontSendNotification);
	}
	else
	{
		attackDeleyTimeSlider.label.setText("Deley", dontSendNotification);
	}
}

void VibratoParametersComponent::timerCallback()
{
	enableSwitch.setToggleState(_vibratoParamsPtr->VibratoEnable->get());
	attackDeleySwitch.setToggleState(_vibratoParamsPtr->VibratoAttackDeleySwitch->get());
	amountSlider.setValue(_vibratoParamsPtr->VibratoAmount->get());
	speedSlider.setValue(_vibratoParamsPtr->VibratoSpeed->get());
	attackDeleyTimeSlider.setValue(_vibratoParamsPtr->VibratoAttackTime->get());
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
	else if (slider == &attackDeleyTimeSlider.slider)
	{
		*_vibratoParamsPtr->VibratoAttackTime = (float)attackDeleyTimeSlider.getValue();
	}
}

void VibratoParametersComponent::buttonClicked(Button* button)
{
	if (button == &enableSwitch.button)
	{
		*_vibratoParamsPtr->VibratoEnable = enableSwitch.getToggleState();
	}
	else if (button == &attackDeleySwitch.button)
	{
		*_vibratoParamsPtr->VibratoAttackDeleySwitch = attackDeleySwitch.getToggleState();
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
	, voicingTypeSelector("Type", _voicingParamsPtr->VoicingSwitch, this)
	, portaTimeSlider("PortaTime", "sec", _voicingParamsPtr->PortaTime, this, 0.01f, 0.5f)
{
	addAndMakeVisible(voicingTypeSelector);
	addAndMakeVisible(portaTimeSlider);
}

void VoicingParametersComponent::paint(Graphics& g)
{
	paintHeader(g, getLocalBounds(), "VOICING");
}

void VoicingParametersComponent::resized()
{
	float rowSize = 2.0f;
	float divide = 1.0f / rowSize;
	std::int32_t compHeight = std::int32_t((getHeight() - PANEL_NAME_HEIGHT) * divide);

	Rectangle<int> bounds = getLocalBounds(); // コンポーネント基準の値
	bounds.removeFromTop(PANEL_NAME_HEIGHT);

	{
		float alpha = _voicingParamsPtr->VoicingSwitch->getCurrentChoiceName() == "PORTAMENTO" ? 1.0f : 0.4f;
		portaTimeSlider.setAlpha(alpha);
	}
	voicingTypeSelector.setBounds(bounds.removeFromTop(compHeight));
	portaTimeSlider.setBounds(bounds.removeFromTop(compHeight));
}

void VoicingParametersComponent::timerCallback()
{
	voicingTypeSelector.setSelectedItemIndex(_voicingParamsPtr->VoicingSwitch->getIndex());
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
	if (comboBoxThatHasChanged == &voicingTypeSelector.selector)
	{
		*_voicingParamsPtr->VoicingSwitch = voicingTypeSelector.getSelectedItemIndex();
	}
	resized();
}

//----------------------------------------------------------------------------------------------------

OptionsParametersComponent::OptionsParametersComponent(OptionsParameters* optionsParams)
	: _optionsParamsPtr(optionsParams)
	, pitchStandardSlider("Tunes", "", _optionsParamsPtr->PitchStandard, this)
	, pitchBendRangeSlider("PB Range", "", _optionsParamsPtr->PitchBendRange, this)
{	
	addAndMakeVisible(pitchStandardSlider);
	addAndMakeVisible(pitchBendRangeSlider);
}

void OptionsParametersComponent::paint(Graphics& g)
{
	paintHeader(g, getLocalBounds(), "OPTIONS");
}

void OptionsParametersComponent::resized()
{
	float columnSize = 2.0f;
	float divide = 1.0f / columnSize;
	std::int32_t compHeight = std::int32_t((getHeight() - PANEL_NAME_HEIGHT) * divide);

	Rectangle<int> bounds = getLocalBounds(); // コンポーネント基準の値
	bounds.removeFromTop(PANEL_NAME_HEIGHT);

	pitchStandardSlider.setBounds(bounds.removeFromTop(compHeight));
	pitchBendRangeSlider.setBounds(bounds.removeFromTop(compHeight));
}

void OptionsParametersComponent::timerCallback()
{
	pitchStandardSlider.setValue(_optionsParamsPtr->PitchStandard->get());
	pitchBendRangeSlider.setValue(_optionsParamsPtr->PitchBendRange->get());
}


void OptionsParametersComponent::sliderValueChanged(Slider* slider)
{
	if (slider == &pitchStandardSlider.slider)
	{
		*_optionsParamsPtr->PitchStandard = (std::int32_t)pitchStandardSlider.getValue();
	}
	else if (slider == &pitchBendRangeSlider.slider)
	{
		*_optionsParamsPtr->PitchBendRange = (std::int32_t)pitchBendRangeSlider.getValue();
	}
}

//----------------------------------------------------------------------------------------------------

MidiEchoParametersComponent::MidiEchoParametersComponent(MidiEchoParameters* midiEchoParams)
	:_midiEchoParamsPtr(midiEchoParams)
	, enableButton("ON / OFF", _midiEchoParamsPtr->IsEchoEnable, this)
	, durationSlider("Duration", "sec", _midiEchoParamsPtr->EchoDuration, this, 0.01f, 0.5f)
	, repeatSlider("Repeat", "", _midiEchoParamsPtr->EchoRepeat , this)
	, volumeOffsetSlider("Vol Offset", "%", _midiEchoParamsPtr->VolumeOffset, this, 0.1f, 100.0f)
{
	addAndMakeVisible(repeatSlider);
	addAndMakeVisible(durationSlider);
	addAndMakeVisible(enableButton);
	addAndMakeVisible(volumeOffsetSlider);
}

void MidiEchoParametersComponent::paint(Graphics& g)
{
	paintHeader(g, getLocalBounds(), "MIDI ECHO");
}

void MidiEchoParametersComponent::resized()
{
	float rowSize = 4.0f;
	float divide = 1.0f / rowSize;
	std::int32_t compHeight = std::int32_t((getHeight() - PANEL_NAME_HEIGHT) * divide);

	Rectangle<int> bounds = getLocalBounds(); // コンポーネント基準の値
	bounds.removeFromTop(PANEL_NAME_HEIGHT);
	
	{
		float alpha = isEditable() ? 1.0f : 0.4f;
		durationSlider.setAlpha(alpha);
		repeatSlider.setAlpha(alpha);
		volumeOffsetSlider.setAlpha(alpha);
	}

	enableButton.setBounds(bounds.removeFromTop(compHeight));
	durationSlider.setBounds(bounds.removeFromTop(compHeight));
	repeatSlider.setBounds(bounds.removeFromTop(compHeight));
	volumeOffsetSlider.setBounds(bounds.removeFromTop(compHeight));

}

void MidiEchoParametersComponent::timerCallback()
{
	enableButton.setToggleState(_midiEchoParamsPtr->IsEchoEnable->get());
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
		*_midiEchoParamsPtr->EchoRepeat = (std::int32_t)repeatSlider.getValue();
	}
	else if (slider == &volumeOffsetSlider.slider)
	{
		*_midiEchoParamsPtr->VolumeOffset = (float)volumeOffsetSlider.getValue();
	}
}

void MidiEchoParametersComponent::buttonClicked(Button* button)
{
	if (button == &enableButton.button)
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
	for (auto i = 0; i < WAVESAMPLE_LENGTH; ++i)
	{
		waveSampleSlider[i].setRange(0, 15, 1.0);
		waveSampleSlider[i].setValue(7, dontSendNotification);
	}

	timerCallback();
}

void RangeSlider::paint(Graphics& g)
{
	//update slider Params
	for (auto* trail : trails)
	{
		float compWidth = (float)(getWidth()) - 12.0f; // 微調整値
		auto compHeight = getHeight();

		std::int32_t index = (std::int32_t)(trail->currentPosition.x * (float)WAVESAMPLE_LENGTH / compWidth);
		if (index < 0)
		{
			index = 0;
		}
		else if (index >= 31)
		{
			index = 31;
		}
		float point = trail->currentPosition.y;// -PANEL_NAME_HEIGHT;
		std::int32_t value = 15 - (std::int32_t)(point * 16.0 / compHeight);
		waveSampleSlider[index].setValue(value, dontSendNotification);
		updateValue();
	}
	//repaint Sliders
	{
		float columnSize = (float)WAVESAMPLE_LENGTH;
		float rowSize = (float)16;
		float divide = 1.0f / columnSize;
		std::int32_t compWidth = std::int32_t((getWidth()) * divide);

		Rectangle<int> bounds = getLocalBounds();

		//Draw Scale Line
		for (auto i = 1; i < 4; ++i)
		{
			float p_y = bounds.getHeight() * 0.25f * i;
			Line<float> line(0.0f, p_y, (float)(bounds.getWidth()), p_y);
			g.setColour(Colours::darkslateblue);
			g.drawLine(line, 1.0f);
		}
		for (auto i = 1; i < 8; ++i)
		{
			float p_x = compWidth * i * 4.0f;
			Line<float> line(p_x, 0.0f, p_x, (float)(bounds.getHeight()));
			g.setColour(Colours::darkslateblue);
			g.drawLine(line, 1.4f);
		}

		//Draw Slider
		for (auto i = 0; i < WAVESAMPLE_LENGTH; ++i)
		{
			Rectangle<int> area = bounds.removeFromLeft(compWidth);
			std::int32_t barHeight = std::int32_t(bounds.getHeight() / rowSize * (waveSampleSlider[i].getMaximum() - waveSampleSlider[i].getValue()));
			area.removeFromTop(barHeight);
			g.setColour(Colours::lime);
			g.fillRect(area.reduced(1));
		}
	}
}

void RangeSlider::timerCallback()
{
	for (auto i = 0; i < WAVESAMPLE_LENGTH; ++i)
	{
		waveSampleSlider[i].setValue(_waveformMemoryParamsPtr->WaveSamplesArray[i]->get(), dontSendNotification);
	}
}

void RangeSlider::updateValue()
{
	for (auto i = 0; i < WAVESAMPLE_LENGTH; ++i)
	{
		*_waveformMemoryParamsPtr->WaveSamplesArray[i] = (std::int32_t)waveSampleSlider[i].getValue();
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

void WaveformMemoryParametersComponent::paint(Graphics& g)
{
	paintHeader(g, getLocalBounds(), "WAVEFORM MEMORY");
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
	waveRangeSlider.setBounds(bounds);
}		

void WaveformMemoryParametersComponent::buttonClicked(Button* button)
{
	if (button == &saveButton)
	{
		saveWaveFile(_waveformMemoryParamsPtr);
	}
	else if (button == &loadButton)
	{
		loadWaveFile(_waveformMemoryParamsPtr);
	}
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
		std::int32_t count = 0;
		for (const auto subStr : split(data, ' ')) {
			*_waveformMemoryParamsPtr->WaveSamplesArray[count] = atoi(subStr.c_str());
			++count;
		}
	}
	repaint();
}

//----------------------------------------------------------------------------------------------------

FilterParametersComponent::FilterParametersComponent(FilterParameters* filterParams)
	: BaseComponent()
	,_filterParamsPtr(filterParams)
	, hiCutSwitch("HiCut: ON / OFF", _filterParamsPtr->HicutEnable, this)
	, lowCutSwitch("LowCut: ON / OFF", _filterParamsPtr->LowcutEnable, this)
	, hicutFreqSlider("hicut", "Hz", _filterParamsPtr->HicutFreq, this, 0.1f, 2000.0f)
	, lowcutFreqSlider("lowcut", "Hz", _filterParamsPtr->LowcutFreq, this, 0.1f, 2000.0f)
{
	addAndMakeVisible(hiCutSwitch);
	addAndMakeVisible(lowCutSwitch);
	addAndMakeVisible(hicutFreqSlider);
	addAndMakeVisible(lowcutFreqSlider);
}

void FilterParametersComponent::paint(Graphics& g)
{
	paintHeader(g, getLocalBounds(), "FILTER");
}

void FilterParametersComponent::resized()
{
	float rowSize = 4.0f;
	float divide = 1.0f / rowSize;
	std::int32_t compHeight = std::int32_t((getHeight() - PANEL_NAME_HEIGHT) * divide);

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

	hiCutSwitch.setBounds(bounds.removeFromTop(compHeight));
	hicutFreqSlider.setBounds(bounds.removeFromTop(compHeight));
	lowCutSwitch.setBounds(bounds.removeFromTop(compHeight));
	lowcutFreqSlider.setBounds(bounds.removeFromTop(compHeight));
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
	if (button == &hiCutSwitch.button)
	{
		*_filterParamsPtr->HicutEnable = hiCutSwitch.getToggleState();
	}
	else if (button == &lowCutSwitch.button)
	{
		*_filterParamsPtr->LowcutEnable = lowCutSwitch.getToggleState();
	}
	resized();
}

//----------------------------------------------------------------------------------------------------

