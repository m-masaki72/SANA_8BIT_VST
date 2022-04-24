#pragma once
#include "../DSP/SynthParameters.h"
#include "ComponentUtil.hpp"

class BaseComponent : public Component, private juce::Timer {
 public:
  BaseComponent() { startTimerHz(5); };
  virtual void paint(Graphics& g) = 0;
  virtual void resized() = 0;

 private:
  virtual void timerCallback() = 0;
};

class ChipOscillatorComponent : public BaseComponent,
                                ComboBox::Listener,
                                Slider::Listener {
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
  TextSelector colorTypeSelector;
  TextSlider colorDurationSlider;
};

class SweepParametersComponent : public BaseComponent,
                                 Slider::Listener,
                                 ComboBox::Listener {
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

class VibratoParametersComponent : public BaseComponent,
                                   Button::Listener,
                                   Slider::Listener {
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

  SwitchButton enableSwitch;
  SwitchButton attackDeleySwitch;
  TextSlider amountSlider;
  TextSlider speedSlider;
  TextSlider attackDeleyTimeSlider;
};

class VoicingParametersComponent : public BaseComponent,
                                   ComboBox::Listener,
                                   Slider::Listener {
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
  TextSlider stepTimeSlider;
};

class OptionsParametersComponent : public BaseComponent, Slider::Listener {
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

class MidiEchoParametersComponent : public BaseComponent,
                                    Button::Listener,
                                    Slider::Listener {
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

class WaveformMemoryParametersComponent : public Component,
                                          Button::Listener,
                                          public FileDragAndDropTarget, 
                                          public FileBrowserListener {
 public:
  WaveformMemoryParametersComponent(WaveformMemoryParameters* waveformMemoryParams);
  ~WaveformMemoryParametersComponent();

  virtual void paint(Graphics& g) override;
  virtual void resized() override;

 private:
  WaveformMemoryParametersComponent();

  // Button::Listener
  virtual void buttonClicked(Button* button) override;

  // FIleDragAndDropTarget
  virtual bool isInterestedInFileDrag(const StringArray& files) override;
  virtual void filesDropped(const StringArray& files, int x, int y) override;
  virtual void fileDragEnter(const StringArray& files, int x, int y) override {};
  virtual void fileDragMove(const StringArray& files, int x, int y) override {};
  virtual void fileDragExit(const StringArray& files) override {};

  // FileBrowserListener
  virtual void fileClicked(const File& file, const MouseEvent& event) override;	
  virtual void selectionChanged() override {};
  virtual void fileDoubleClicked(const File &) override {};	
  virtual void browserRootChanged(const File &file) override;	

  WaveformMemoryParameters* _waveformMemoryParamsPtr;

  WaveSampleSliders waveRangeSlider;
  FileBrowserComponent* _fileBrowser = nullptr;
  TextButton saveButton;
  TextButton loadButton;
  TextButton fileBrowserButton;
  bool isFileBrowserEnabled = false;
  const int BUTTON_HEIGHT = 32;
  const int FILE_BROWSER_WIDTH = 240;
};

class FilterParametersComponent : public BaseComponent,
                                  Button::Listener,
                                  Slider::Listener {
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

class WavePatternsComponent : public BaseComponent,
                              public Button::Listener,
                              public Slider::Listener,
                              public ComboBox::Listener {
 public:
  WavePatternsComponent(WavePatternParameters* wavePatternParameters);

  virtual void paint(Graphics& g) override;
  virtual void resized() override;

 private:
  virtual void timerCallback() override;
  virtual void comboBoxChanged(ComboBox* comboBoxThatHasChanged) override;
  virtual void buttonClicked(Button* button) override;
  virtual void sliderValueChanged(Slider* slider) override;

  WavePatternParameters* _wavePatternParameters;
  SwitchButton _enableSwitch;
  SwitchButton _loopSwitch;
  TextSlider _stepTimeSlider;
  TextSelector _waveTypeSelectors[4];
  PatternSliders _rangeSliders;

  const StringArray OSC_WAVE_TYPES {
  "NES_Square50%",    "NES_Square25%",   "NES_Square12.5%",
  "NES_Triangle",     "Pure_Square50%",  "Pure_Square25%",
  "Pure_Square12.5%", "Pure_Triangle",   "Pure_Sine",
  "Pure_Saw",         "NES_LongNoise",   "NES_ShortNoise",
  "Pure_Lo-bitNoise", "Waveform Memory",
};
};