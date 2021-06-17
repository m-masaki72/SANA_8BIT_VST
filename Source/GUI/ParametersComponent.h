#pragma once
#include "../DSP/SynthParameters.h"
#include "ComponentUtil.hpp"

class BaseComponent : public Component, private juce::Timer {
 public:
  BaseComponent() { startTimerHz(10); };
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
  virtual void browserRootChanged(const File &) override {};	

  WaveformMemoryParameters* _waveformMemoryParamsPtr;

  RangeSliders waveRangeSlider;
  FileBrowserComponent* _fileBrowser = nullptr;
  TextButton saveButton;
  TextButton loadButton;
  // File preFilePath = File::getSpecialLocation(File::userDesktopDirectory);
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
