#include "ParametersComponent.h"

namespace {
const Colour PANEL_COLOUR() { return Colours::cornsilk; }
const Colour HEADER_COLOUR() { return Colours::darkorange; }
const Colour FONT_COLOUR() { return Colours::black; }
const Colour TEXT_COLOUR() { return Colours::white; }
const Colour BACKGROUND_COLOUR() { return Colour(60,14,60); }

const float HEADER_HEIGHT = 24.0f;
const std::int32_t LOCAL_MARGIN = 2;

// static File preFilePath = File::getSpecialLocation(File::userDesktopDirectory);
static File preFilePath = File("C:\\Users\\memen\\Desktop\\SANA_8BIT_VST\\PreBuilds\\WAVE_FORM_SAMPLES");
}  // namespace

static std::vector<std::string> split(std::string str, char del) {
  auto first = 0;
  auto last = str.find_first_of(del);

  std::vector<std::string> result;

  while (first < str.size()) {
    std::string subStr(str, first, last - first);

    result.push_back(subStr);

    first = last + 1;
    last = str.find_first_of(del, first);

    if (last == std::string::npos) {
      last = str.size();
    }
  }
  return result;
}

static void paintHeader(Graphics& g, Rectangle<int> bounds, std::string text) {
  { // 枠の描画
    auto x = 0.0f, y = HEADER_HEIGHT / 2.0f;
    auto width = (float)bounds.getWidth(), height = (float)bounds.getHeight() - y;
    auto cornerSize = 8.0f, thickness = 2.0f;
    g.setColour(Colours::orange);
    g.drawRoundedRectangle(x, y, width, height, cornerSize, thickness);
  }

  { // ヘッダー描画
    g.setFont(Font(24, Font::bold));
    auto x = 20.f, y = 0.0f;
    auto width = (float)g.getCurrentFont().getStringWidth(text), height = HEADER_HEIGHT;

    g.setColour(BACKGROUND_COLOUR());
    g.fillRect(x,y,width,height);
    g.setColour(Colours::orange);
    g.drawText(text, x, y, width, height, Justification::centred, false);
  }
}

static void saveWaveFile(WaveformMemoryParameters* _waveformMemoryParamsPtr) {
  FileChooser fileSaver("Save File As", preFilePath, "*.wfm");

  if (fileSaver.browseForFileToSave(true)) {
    File newFile(fileSaver.getResult());
    newFile.create();
    newFile.replaceWithText("");
    for (auto i = 0; i < WAVESAMPLE_LENGTH; ++i) {
      newFile.appendText(
          std::to_string(*_waveformMemoryParamsPtr->WaveSamplesArray[i]));
      newFile.appendText(" ");
    }
    preFilePath = fileSaver.getResult();
  }
}

static void loadWaveFile(WaveformMemoryParameters* _waveformMemoryParamsPtr) {
  FileChooser fileLoader("Load File From", preFilePath, "*.wfm");

  if (fileLoader.browseForFileToOpen()) {
    File waveformFile(fileLoader.getResult());
    std::string data = waveformFile.loadFileAsString().toStdString();
    auto count = 0;
    for (const auto subStr : split(data, ' ')) {
      *_waveformMemoryParamsPtr->WaveSamplesArray[count] = atoi(subStr.c_str());
      ++count;
    }
    preFilePath = fileLoader.getResult();
  }
}

ChipOscillatorComponent::ChipOscillatorComponent(
    ChipOscillatorParameters* oscParams)
    : _oscParamsPtr(oscParams),
      waveTypeSelector("waveForm", _oscParamsPtr->OscWaveType, this),
      volumeLevelSlider("Volume", "dB", _oscParamsPtr->VolumeLevel, this,
                        0.01f),
      attackSlider("Attack", "sec", _oscParamsPtr->Attack, this, 0.0001f, 1.0f),
      decaySlider("Decay", "sec", _oscParamsPtr->Decay, this, 0.0001f, 1.0f),
      sustainSlider("Sustain", "", _oscParamsPtr->Sustain, this, 0.0001f),
      releaseSlider("Release", "sec", _oscParamsPtr->Release, this, 0.0001f,
                    1.0f) {

  addAndMakeVisible(waveTypeSelector);
  addAndMakeVisible(volumeLevelSlider);
  addAndMakeVisible(attackSlider);
  addAndMakeVisible(decaySlider);
  addAndMakeVisible(sustainSlider);
  addAndMakeVisible(releaseSlider);
}

void ChipOscillatorComponent::paint(Graphics& g) {
  paintHeader(g, getLocalBounds(), "OSCILLATOR");
}

void ChipOscillatorComponent::resized() {
  float rowSize = 6.0f;
  float divide = 1.0f / rowSize;
  std::int32_t compHeight =
      std::int32_t((getHeight() - HEADER_HEIGHT) * divide);

  Rectangle<int> bounds = getLocalBounds();  // コンポーネント基準の値
  bounds.removeFromTop(HEADER_HEIGHT);

  waveTypeSelector.setBounds(bounds.removeFromTop(compHeight));
  volumeLevelSlider.setBounds(bounds.removeFromTop(compHeight));
  attackSlider.setBounds(bounds.removeFromTop(compHeight));
  decaySlider.setBounds(bounds.removeFromTop(compHeight));
  sustainSlider.setBounds(bounds.removeFromTop(compHeight));
  releaseSlider.setBounds(bounds.removeFromTop(compHeight));
}

void ChipOscillatorComponent::timerCallback() {
  waveTypeSelector.setSelectedItemIndex(_oscParamsPtr->OscWaveType->getIndex());
  volumeLevelSlider.setValue(_oscParamsPtr->VolumeLevel->get());
  attackSlider.setValue(_oscParamsPtr->Attack->get());
  decaySlider.setValue(_oscParamsPtr->Decay->get());
  sustainSlider.setValue(_oscParamsPtr->Sustain->get());
  releaseSlider.setValue(_oscParamsPtr->Release->get());
}

void ChipOscillatorComponent::sliderValueChanged(Slider* slider) {
  if (slider == &volumeLevelSlider.slider) {
    *_oscParamsPtr->VolumeLevel = (float)volumeLevelSlider.getValue();
  } else if (slider == &attackSlider.slider) {
    *_oscParamsPtr->Attack = (float)attackSlider.getValue();
  } else if (slider == &decaySlider.slider) {
    *_oscParamsPtr->Decay = (float)decaySlider.getValue();
  } else if (slider == &sustainSlider.slider) {
    *_oscParamsPtr->Sustain = (float)sustainSlider.getValue();
  } else if (slider == &releaseSlider.slider) {
    *_oscParamsPtr->Release = (float)releaseSlider.getValue();
  }
}

void ChipOscillatorComponent::comboBoxChanged(
    ComboBox* comboBoxThatHasChanged) {
  if (comboBoxThatHasChanged == &waveTypeSelector.selector) {
    *_oscParamsPtr->OscWaveType = waveTypeSelector.getSelectedItemIndex();
  }
}

SweepParametersComponent::SweepParametersComponent(SweepParameters* sweepParams)
    : _sweepParamsPtr(sweepParams),
      sweepSwitchSelector("Sweep-Swetch", _sweepParamsPtr->SweepSwitch, this),
      timeSlider("Speed", "sec", _sweepParamsPtr->SweepTime, this, 0.01f,
                 1.0f) {
  addAndMakeVisible(sweepSwitchSelector);
  addAndMakeVisible(timeSlider);
}

void SweepParametersComponent::paint(Graphics& g) {
  paintHeader(g, getLocalBounds(), "PITCH SWEEP");
}

void SweepParametersComponent::resized() {
  float rowSize = 2.0f;
  float divide = 1.0f / rowSize;
  std::int32_t compHeight =
      std::int32_t((getHeight() - HEADER_HEIGHT) * divide);

  Rectangle<int> bounds = getLocalBounds();  // コンポーネント基準の値
  bounds.removeFromTop(HEADER_HEIGHT);

  {
    float alpha = isEditable() ? 1.0f : 0.4f;
    timeSlider.setAlpha(alpha);
  }
  sweepSwitchSelector.setBounds(bounds.removeFromTop(compHeight));
  timeSlider.setBounds(bounds.removeFromTop(compHeight));
}

void SweepParametersComponent::timerCallback() {
  sweepSwitchSelector.setSelectedItemIndex(
      _sweepParamsPtr->SweepSwitch->getIndex());
  timeSlider.setValue(_sweepParamsPtr->SweepTime->get());
}

void SweepParametersComponent::sliderValueChanged(Slider* slider) {
  if (slider == &timeSlider.slider) {
    *_sweepParamsPtr->SweepTime = (float)timeSlider.getValue();
  }
}

void SweepParametersComponent::comboBoxChanged(
    ComboBox* comboBoxThatHasChanged) {
  if (comboBoxThatHasChanged == &sweepSwitchSelector.selector) {
    *_sweepParamsPtr->SweepSwitch = sweepSwitchSelector.getSelectedItemIndex();
  }
  resized();
}

bool SweepParametersComponent::isEditable() {
  return (_sweepParamsPtr->SweepSwitch->getIndex() >= 1) ? true : false;
}

VibratoParametersComponent::VibratoParametersComponent(
    VibratoParameters* vibratoParams)
    : _vibratoParamsPtr(vibratoParams),
      enableSwitch("Vibrato-Switch", _vibratoParamsPtr->VibratoEnable, this),
      attackDeleySwitch("Attack-Deley-Switch",
                        _vibratoParamsPtr->VibratoAttackDeleySwitch, this),
      amountSlider("Depth", "HarfTone", _vibratoParamsPtr->VibratoAmount, this,
                   0.01f, 2.0f),
      speedSlider("Speed", "hz", _vibratoParamsPtr->VibratoSpeed, this, 0.001f,
                  10.0f),
      attackDeleyTimeSlider("Attack", "sec",
                            _vibratoParamsPtr->VibratoAttackTime, this, 0.001f,
                            2.0f) {
  addAndMakeVisible(enableSwitch);
  addAndMakeVisible(attackDeleySwitch);
  addAndMakeVisible(amountSlider);
  addAndMakeVisible(speedSlider);
  addAndMakeVisible(attackDeleyTimeSlider);

  // 切り替えSwitchのカスタマイズ
  { attackDeleySwitch.button.setButtonText("Attack / Deley"); }
}

void VibratoParametersComponent::paint(Graphics& g) {
  paintHeader(g, getLocalBounds(), "VIBRATO");
}

void VibratoParametersComponent::resized() {
  float rowSize = 4.0f;
  float divide = 1.0f / rowSize;
  std::int32_t compHeight =
      std::int32_t((getHeight() - HEADER_HEIGHT) * divide);

  Rectangle<int> bounds = getLocalBounds();  // コンポーネント基準の値
  bounds.removeFromTop(HEADER_HEIGHT);

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
  if (_vibratoParamsPtr->VibratoAttackDeleySwitch->get() == true) {
    attackDeleyTimeSlider.label.setText("Attack", dontSendNotification);
  } else {
    attackDeleyTimeSlider.label.setText("Deley", dontSendNotification);
  }
}

void VibratoParametersComponent::timerCallback() {
  enableSwitch.setToggleState(_vibratoParamsPtr->VibratoEnable->get());
  attackDeleySwitch.setToggleState(
      _vibratoParamsPtr->VibratoAttackDeleySwitch->get());
  amountSlider.setValue(_vibratoParamsPtr->VibratoAmount->get());
  speedSlider.setValue(_vibratoParamsPtr->VibratoSpeed->get());
  attackDeleyTimeSlider.setValue(_vibratoParamsPtr->VibratoAttackTime->get());
}

void VibratoParametersComponent::sliderValueChanged(Slider* slider) {
  if (slider == &amountSlider.slider) {
    *_vibratoParamsPtr->VibratoAmount = (float)amountSlider.getValue();
  } else if (slider == &speedSlider.slider) {
    *_vibratoParamsPtr->VibratoSpeed = (float)speedSlider.getValue();
  } else if (slider == &attackDeleyTimeSlider.slider) {
    *_vibratoParamsPtr->VibratoAttackTime =
        (float)attackDeleyTimeSlider.getValue();
  }
}

void VibratoParametersComponent::buttonClicked(Button* button) {
  if (button == &enableSwitch.button) {
    *_vibratoParamsPtr->VibratoEnable = enableSwitch.getToggleState();
  } else if (button == &attackDeleySwitch.button) {
    *_vibratoParamsPtr->VibratoAttackDeleySwitch =
        attackDeleySwitch.getToggleState();
  }
  resized();
}

bool VibratoParametersComponent::isEditable() {
  return _vibratoParamsPtr->VibratoEnable->get();
}

VoicingParametersComponent::VoicingParametersComponent(
    VoicingParameters* voicingParams)
    : _voicingParamsPtr(voicingParams),
      voicingTypeSelector("Type", _voicingParamsPtr->VoicingSwitch, this),
      stepTimeSlider("StepTime", "sec", _voicingParamsPtr->StepTime, this,
                     0.001f, 0.5f) {
  addAndMakeVisible(voicingTypeSelector);
  addAndMakeVisible(stepTimeSlider);
}

void VoicingParametersComponent::paint(Graphics& g) {
  paintHeader(g, getLocalBounds(), "VOICING");
}

void VoicingParametersComponent::resized() {
  float rowSize = 2.0f;
  float divide = 1.0f / rowSize;
  std::int32_t compHeight =
      std::int32_t((getHeight() - HEADER_HEIGHT) * divide);

  Rectangle<int> bounds = getLocalBounds();  // コンポーネント基準の値
  bounds.removeFromTop(HEADER_HEIGHT);

  {
    auto alpha = 0.0f;
    if (_voicingParamsPtr->VoicingSwitch->getCurrentChoiceName() ==
            "PORTAMENTO" ||
        _voicingParamsPtr->VoicingSwitch->getCurrentChoiceName() ==
            "ARPEGGIO") {
      alpha = 1.0f;
    } else {
      alpha = 0.4f;
    }
    stepTimeSlider.setAlpha(alpha);
  }
  voicingTypeSelector.setBounds(bounds.removeFromTop(compHeight));
  stepTimeSlider.setBounds(bounds.removeFromTop(compHeight));
}

void VoicingParametersComponent::timerCallback() {
  voicingTypeSelector.setSelectedItemIndex(
      _voicingParamsPtr->VoicingSwitch->getIndex());
  stepTimeSlider.setValue(_voicingParamsPtr->StepTime->get());
}

void VoicingParametersComponent::sliderValueChanged(Slider* slider) {
  if (slider == &stepTimeSlider.slider) {
    *_voicingParamsPtr->StepTime = (float)stepTimeSlider.getValue();
  }
}

void VoicingParametersComponent::comboBoxChanged(
    ComboBox* comboBoxThatHasChanged) {
  if (comboBoxThatHasChanged == &voicingTypeSelector.selector) {
    *_voicingParamsPtr->VoicingSwitch =
        voicingTypeSelector.getSelectedItemIndex();
  }
  resized();
}

OptionsParametersComponent::OptionsParametersComponent(
    OptionsParameters* optionsParams)
    : _optionsParamsPtr(optionsParams),
      pitchStandardSlider("Tunes", "", _optionsParamsPtr->PitchStandard, this),
      pitchBendRangeSlider("PB Range", "", _optionsParamsPtr->PitchBendRange,
                           this) {
  addAndMakeVisible(pitchStandardSlider);
  addAndMakeVisible(pitchBendRangeSlider);
}

void OptionsParametersComponent::paint(Graphics& g) {
  paintHeader(g, getLocalBounds(), "OPTIONS");
}

void OptionsParametersComponent::resized() {
  float columnSize = 2.0f;
  float divide = 1.0f / columnSize;
  std::int32_t compHeight =
      std::int32_t((getHeight() - HEADER_HEIGHT) * divide);

  Rectangle<int> bounds = getLocalBounds();  // コンポーネント基準の値
  bounds.removeFromTop(HEADER_HEIGHT);

  pitchStandardSlider.setBounds(bounds.removeFromTop(compHeight));
  pitchBendRangeSlider.setBounds(bounds.removeFromTop(compHeight));
}

void OptionsParametersComponent::timerCallback() {
  pitchStandardSlider.setValue(_optionsParamsPtr->PitchStandard->get());
  pitchBendRangeSlider.setValue(_optionsParamsPtr->PitchBendRange->get());
}

void OptionsParametersComponent::sliderValueChanged(Slider* slider) {
  if (slider == &pitchStandardSlider.slider) {
    *_optionsParamsPtr->PitchStandard =
        (std::int32_t)pitchStandardSlider.getValue();
  } else if (slider == &pitchBendRangeSlider.slider) {
    *_optionsParamsPtr->PitchBendRange =
        (std::int32_t)pitchBendRangeSlider.getValue();
  }
}

MidiEchoParametersComponent::MidiEchoParametersComponent(
    MidiEchoParameters* midiEchoParams)
    : _midiEchoParamsPtr(midiEchoParams),
      enableButton("ON / OFF", _midiEchoParamsPtr->IsEchoEnable, this),
      durationSlider("Duration", "sec", _midiEchoParamsPtr->EchoDuration, this,
                     0.01f, 0.5f),
      repeatSlider("Repeat", "", _midiEchoParamsPtr->EchoRepeat, this),
      volumeOffsetSlider("Vol Offset", "%", _midiEchoParamsPtr->VolumeOffset,
                         this, 0.1f, 100.0f) {
  addAndMakeVisible(repeatSlider);
  addAndMakeVisible(durationSlider);
  addAndMakeVisible(enableButton);
  addAndMakeVisible(volumeOffsetSlider);
}

void MidiEchoParametersComponent::paint(Graphics& g) {
  paintHeader(g, getLocalBounds(), "MIDI ECHO");
}

void MidiEchoParametersComponent::resized() {
  float rowSize = 4.0f;
  float divide = 1.0f / rowSize;
  std::int32_t compHeight =
      std::int32_t((getHeight() - HEADER_HEIGHT) * divide);

  Rectangle<int> bounds = getLocalBounds();  // コンポーネント基準の値
  bounds.removeFromTop(HEADER_HEIGHT);

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

void MidiEchoParametersComponent::timerCallback() {
  enableButton.setToggleState(_midiEchoParamsPtr->IsEchoEnable->get());
  durationSlider.setValue(_midiEchoParamsPtr->EchoDuration->get());
  repeatSlider.setValue(_midiEchoParamsPtr->EchoRepeat->get());
  volumeOffsetSlider.setValue(_midiEchoParamsPtr->VolumeOffset->get());
}

void MidiEchoParametersComponent::sliderValueChanged(Slider* slider) {
  if (slider == &durationSlider.slider) {
    *_midiEchoParamsPtr->EchoDuration = (float)durationSlider.getValue();
  } else if (slider == &repeatSlider.slider) {
    *_midiEchoParamsPtr->EchoRepeat = (std::int32_t)repeatSlider.getValue();
  } else if (slider == &volumeOffsetSlider.slider) {
    *_midiEchoParamsPtr->VolumeOffset = (float)volumeOffsetSlider.getValue();
  }
}

void MidiEchoParametersComponent::buttonClicked(Button* button) {
  if (button == &enableButton.button) {
    *_midiEchoParamsPtr->IsEchoEnable = enableButton.getToggleState();
  }
  resized();
}

bool MidiEchoParametersComponent::isEditable() {
  return _midiEchoParamsPtr->IsEchoEnable->get();
}

WaveformMemoryParametersComponent::WaveformMemoryParametersComponent(
    WaveformMemoryParameters* waveformMemoryParams)
    : _waveformMemoryParamsPtr(waveformMemoryParams),
      waveRangeSlider(waveformMemoryParams),
      saveButton(),
      loadButton() {
  auto fileMode = 
    FileBrowserComponent::FileChooserFlags::openMode + 
    FileBrowserComponent::FileChooserFlags::canSelectFiles + 
    FileBrowserComponent::FileChooserFlags::filenameBoxIsReadOnly + 
    FileBrowserComponent::FileChooserFlags::doNotClearFileNameOnRootChange;

  _fileBrowser = new FileBrowserComponent(fileMode , preFilePath, nullptr, nullptr);
  _fileBrowser->addListener(this);
  addAndMakeVisible(_fileBrowser);

  saveButton.setButtonText("Save to File");
  saveButton.addListener(this);
  addAndMakeVisible(saveButton);

  loadButton.setButtonText("Load from File");
  loadButton.addListener(this);
  addAndMakeVisible(loadButton);

  waveRangeSlider.addMouseListener(this, true);
  addAndMakeVisible(waveRangeSlider);
}

WaveformMemoryParametersComponent::~WaveformMemoryParametersComponent() {
  delete _fileBrowser;
}

void WaveformMemoryParametersComponent::paint(Graphics& g) {
  paintHeader(g, getLocalBounds(), "WAVEFORM MEMORY");
}

void WaveformMemoryParametersComponent::resized() {
  Rectangle<int> bounds = getLocalBounds();
  bounds.removeFromTop(HEADER_HEIGHT);

  {
    Rectangle<int> area = bounds.removeFromBottom(BUTTON_HEIGHT);
    saveButton.setBounds(
        area.removeFromLeft(area.getWidth() / 2).reduced(LOCAL_MARGIN));
    loadButton.setBounds(area.reduced(LOCAL_MARGIN));
  }
  _fileBrowser->setBounds(bounds.removeFromLeft(FILE_BROWSER_WIDTH));

  waveRangeSlider.setBounds(bounds);
}

void WaveformMemoryParametersComponent::buttonClicked(Button* button) {
  if (button == &saveButton) {
    saveWaveFile(_waveformMemoryParamsPtr);
  } else if (button == &loadButton) {
    loadWaveFile(_waveformMemoryParamsPtr);
  }
}

bool WaveformMemoryParametersComponent::isInterestedInFileDrag(
    const StringArray& files) {
  return true;
}

void WaveformMemoryParametersComponent::filesDropped(const StringArray& files,
                                                     int x, int y) {
  std::string filePath = files.begin()->toStdString();

  if (filePath.find(".wfm")) {
    File waveformFile(filePath);
    std::string data = waveformFile.loadFileAsString().toStdString();
    std::int32_t count = 0;
    for (const auto subStr : split(data, ' ')) {
      *_waveformMemoryParamsPtr->WaveSamplesArray[count] = atoi(subStr.c_str());
      ++count;
    }
  }
}

void WaveformMemoryParametersComponent::fileClicked(const File& file, const MouseEvent& event) {
  std::string filePath = file.getFullPathName().toStdString();
  
  if (filePath.find(".wfm")) {
    File waveformFile(filePath);
    std::string data = waveformFile.loadFileAsString().toStdString();
    std::int32_t count = 0;
    for (const auto subStr : split(data, ' ')) {
      *_waveformMemoryParamsPtr->WaveSamplesArray[count] = atoi(subStr.c_str());
      ++count;
    }
  }
}	

FilterParametersComponent::FilterParametersComponent(
    FilterParameters* filterParams)
    : BaseComponent(),
      _filterParamsPtr(filterParams),
      hiCutSwitch("HiCut: ON / OFF", _filterParamsPtr->HicutEnable, this),
      lowCutSwitch("LowCut: ON / OFF", _filterParamsPtr->LowcutEnable, this),
      hicutFreqSlider("hicut", "Hz", _filterParamsPtr->HicutFreq, this, 0.1f,
                      2000.0f),
      lowcutFreqSlider("lowcut", "Hz", _filterParamsPtr->LowcutFreq, this, 0.1f,
                       2000.0f) {
  addAndMakeVisible(hiCutSwitch);
  addAndMakeVisible(lowCutSwitch);
  addAndMakeVisible(hicutFreqSlider);
  addAndMakeVisible(lowcutFreqSlider);
}

void FilterParametersComponent::paint(Graphics& g) {
  paintHeader(g, getLocalBounds(), "FILTER");
}

void FilterParametersComponent::resized() {
  float rowSize = 4.0f;
  float divide = 1.0f / rowSize;
  std::int32_t compHeight =
      std::int32_t((getHeight() - HEADER_HEIGHT) * divide);

  Rectangle<int> bounds = getLocalBounds();  // コンポーネント基準の値
  bounds.removeFromTop(HEADER_HEIGHT);

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

void FilterParametersComponent::timerCallback() {
  hicutFreqSlider.setValue(_filterParamsPtr->HicutFreq->get());
  lowcutFreqSlider.setValue(_filterParamsPtr->LowcutFreq->get());
}

void FilterParametersComponent::sliderValueChanged(Slider* slider) {
  if (slider == &hicutFreqSlider.slider) {
    *_filterParamsPtr->HicutFreq = (float)hicutFreqSlider.getValue();
  } else if (slider == &lowcutFreqSlider.slider) {
    *_filterParamsPtr->LowcutFreq = (float)lowcutFreqSlider.getValue();
  }
}

void FilterParametersComponent::buttonClicked(Button* button) {
  if (button == &hiCutSwitch.button) {
    *_filterParamsPtr->HicutEnable = hiCutSwitch.getToggleState();
  } else if (button == &lowCutSwitch.button) {
    *_filterParamsPtr->LowcutEnable = lowCutSwitch.getToggleState();
  }
  resized();
}