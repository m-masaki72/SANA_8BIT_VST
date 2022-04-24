#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "GUI/ParametersComponent.h"
#include "GUI/ScopeComponent.hpp"

class PluginProcessor;

class EditorGUI : public AudioProcessorEditor,
                                        public Button::Listener {
 public:
  EditorGUI(PluginProcessor & p);
  ~EditorGUI();
  void paint(Graphics& g) override;
  void resized() override;
  void buttonClicked(Button* button) override;

 private:
  PluginProcessor & processor;

  MidiKeyboardComponent keyboardComponent;

  PageButton OscButton;
  PageButton EffectButton;

  // Oscillator Page Component
  ScopeComponent<float> scopeComponent;
  ChipOscillatorComponent chipOscComponent;
  SweepParametersComponent sweepParamsComponent;
  VibratoParametersComponent vibratoParamsComponent;
  VoicingParametersComponent voicingParamsComponent;
  OptionsParametersComponent optionsParamsComponent;
  WaveformMemoryParametersComponent waveformMemoryParamsComponent;
  // Effects Page Component
  MidiEchoParametersComponent midiEchoParamsComponent;
  FilterParametersComponent filterParamsComponent;

  LookAndFeel* customLookAndFeel;

  WavePatternsComponent wavePatternsComponent;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EditorGUI)
};
