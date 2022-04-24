#include "EditorGUI.h"

#include "PluginProcessor.h"
#include "GUI/ScopeComponent.hpp"

namespace {
const std::int32_t KEY_HEIGHT = 80;
const float KEY_WIDTH = 32.0f;
const float KEY_SCROLL_WIDTH = 32.0f;
const std::int32_t PANEL_MARGIN = 3;

}  // namespace

EditorGUI::EditorGUI(PluginProcessor &p)
    : AudioProcessorEditor(&p),
      processor(p),
      keyboardComponent(p.getKeyboardState(),
                        MidiKeyboardComponent::Orientation::horizontalKeyboard),
      OscButton("Wave", this),
      EffectButton("Effects", this),
      chipOscComponent(&p.chipOscParameters),
      sweepParamsComponent(&p.sweepParameters),
      vibratoParamsComponent(&p.vibratoParameters),
      voicingParamsComponent(&p.voicingParameters),
      optionsParamsComponent(&p.optionsParameters),
      waveformMemoryParamsComponent(&p.waveformMemoryParameters),
      midiEchoParamsComponent(&p.midiEchoParameters),
      filterParamsComponent(&p.filterParameters),
      wavePatternsComponent(&p.wavePatternParameters),
      scopeComponent(p.getAudioBufferQueue()) {
  /*
          TabComponentを使いたかったが，Tabだとメモリリークが収まらないので現在の形に．
          デストラクタ時にメモリリーク，CustomLookAndFeelまわりでエラーが取れない．
          安定化のためにアルファ切り替えによるGUI管理方式に変更
  */

  {
    addAndMakeVisible(keyboardComponent);
    keyboardComponent.setKeyWidth(KEY_WIDTH);
    keyboardComponent.setScrollButtonWidth(KEY_SCROLL_WIDTH);

    addAndMakeVisible(OscButton);
    addAndMakeVisible(EffectButton);
    OscButton.setToggleState(true);
    EffectButton.setToggleState(false);
  }
  {
    addAndMakeVisible(chipOscComponent);
    addAndMakeVisible(sweepParamsComponent);
    addAndMakeVisible(vibratoParamsComponent);
    addAndMakeVisible(voicingParamsComponent);
    addAndMakeVisible(optionsParamsComponent);
    addAndMakeVisible(waveformMemoryParamsComponent);
    addAndMakeVisible(scopeComponent);
    addAndMakeVisible(wavePatternsComponent);
  }
  {
    addAndMakeVisible(midiEchoParamsComponent);
    addAndMakeVisible(filterParamsComponent);
    addAndMakeVisible(scopeComponent);
  }

  setSize(960, 540 + KEY_HEIGHT);

  {
    customLookAndFeel = new LookAndFeel_V4(LookAndFeel_V4::getLightColourScheme());

    customLookAndFeel->setColour(TextButton::ColourIds::buttonColourId, Colours::black);
    customLookAndFeel->setColour(TextButton::ColourIds::buttonOnColourId, Colours::black);
    customLookAndFeel->setColour(TextButton::ColourIds::textColourOffId, Colours::white);
    customLookAndFeel->setColour(TextButton::ColourIds::textColourOnId, Colours::white);

    customLookAndFeel->setColour(Slider::ColourIds::trackColourId, Colours::lightgreen);
    customLookAndFeel->setColour(Slider::ColourIds::thumbColourId, Colours::greenyellow);
    customLookAndFeel->setColour(Slider::ColourIds::backgroundColourId, Colours::white);
    customLookAndFeel->setColour(Slider::ColourIds::textBoxBackgroundColourId,Colours::black);
    customLookAndFeel->setColour(Slider::ColourIds::textBoxTextColourId, Colours::white);

    customLookAndFeel->setColour(ToggleButton::ColourIds::textColourId, Colours::white);
    customLookAndFeel->setColour(ToggleButton::ColourIds::tickColourId, Colours::white);
    customLookAndFeel->setColour(ToggleButton::ColourIds::tickDisabledColourId, Colours::grey);

    customLookAndFeel->setColour(ComboBox::ColourIds::arrowColourId, Colours::white);
    customLookAndFeel->setColour(ComboBox::ColourIds::backgroundColourId, Colours::black);
    customLookAndFeel->setColour(ComboBox::ColourIds::buttonColourId, Colours::white);
    customLookAndFeel->setColour(ComboBox::ColourIds::focusedOutlineColourId, Colours::white);
    customLookAndFeel->setColour(ComboBox::ColourIds::outlineColourId, Colours::lightgrey);
    customLookAndFeel->setColour(ComboBox::ColourIds::textColourId, Colours::white);
    
    customLookAndFeel->setColour(Label::textColourId, Colours::white);

    for (Component* child : AudioProcessorEditor::getChildren()) {
      child->setLookAndFeel(customLookAndFeel);
    }

  }

  // init each GUI Alpha
  {
    scopeComponent.setVisible(true);

    chipOscComponent.setVisible(true);
    sweepParamsComponent.setVisible(true);
    vibratoParamsComponent.setVisible(true);
    voicingParamsComponent.setVisible(true);
    optionsParamsComponent.setVisible(true);
    waveformMemoryParamsComponent.setVisible(true);
    midiEchoParamsComponent.setVisible(false);
    filterParamsComponent.setVisible(false);
  }
}

EditorGUI::~EditorGUI() {
  for (Component* child : getChildren()) {
    child->setLookAndFeel(nullptr);
  }

  delete customLookAndFeel;
}

void EditorGUI::paint(Graphics& g) {
  g.fillAll(BACKGROUND_COLOUR());
  g.setColour(Colours::white);
  g.setFont(Font(32, Font::bold));
  std::string VersionName = "Version ";
  VersionName += JucePlugin_VersionString;
  g.drawFittedText(VersionName, AudioProcessorEditor::getLocalBounds(),
                   Justification::topRight, 1);
}

void EditorGUI::resized() {
  Rectangle<int> bounds = getLocalBounds();

  keyboardComponent.setBounds(bounds.removeFromBottom(KEY_HEIGHT));

  {
    Rectangle<int> area = bounds.removeFromTop(40);
    OscButton.setBounds(area.removeFromLeft(80));
    EffectButton.setBounds(area.removeFromLeft(80));
  }

  // Oscillator Page
  if (OscButton.button.getToggleState() == true) {
    Rectangle<int> mainbounds = bounds;
    {
      Rectangle<int> leftArea =
          mainbounds.removeFromLeft(bounds.getWidth() * 0.45f);
      chipOscComponent.setBounds(
          leftArea.removeFromTop(leftArea.getHeight() * 0.6f).reduced(PANEL_MARGIN));
      scopeComponent.setBounds(leftArea.reduced(PANEL_MARGIN));
    }
    {
      Rectangle<int> rightArea = mainbounds;
      auto HEIGHT = rightArea.getHeight();
      waveformMemoryParamsComponent.setBounds(
        rightArea.removeFromTop(HEIGHT * 0.6f).reduced(PANEL_MARGIN)
      );
      
      wavePatternsComponent.setBounds(rightArea);
    }
  }

  if (EffectButton.button.getToggleState() == true) {
    Rectangle<int> mainbounds = bounds;
    {
      Rectangle<int> leftArea = mainbounds.removeFromLeft(bounds.getWidth() * 0.45f);
      chipOscComponent.setBounds(leftArea.removeFromTop(leftArea.getHeight() * 0.6f).reduced(PANEL_MARGIN));
      scopeComponent.setBounds(leftArea.reduced(PANEL_MARGIN));
    }
    {
      Rectangle<int> rightArea = mainbounds;
      auto boundsHeight = rightArea.getHeight() / 3.f;
      auto boundsWidth = rightArea.getWidth() / 2.f;
      {
        auto bounds = rightArea.removeFromTop(boundsHeight);
        vibratoParamsComponent.setBounds(bounds.removeFromLeft(boundsWidth));
        sweepParamsComponent.setBounds(bounds);
      }
      {
        auto bounds = rightArea.removeFromTop(boundsHeight);
        midiEchoParamsComponent.setBounds(bounds.removeFromLeft(boundsWidth));
        filterParamsComponent.setBounds(bounds);
      }
      {
        auto bounds = rightArea.removeFromTop(boundsHeight);
        voicingParamsComponent.setBounds(bounds.removeFromLeft(boundsWidth));
        optionsParamsComponent.setBounds(bounds);
      }
    }
  }
}

void EditorGUI::buttonClicked(Button* button) {
  {
    chipOscComponent.setVisible(false);
    sweepParamsComponent.setVisible(false);
    vibratoParamsComponent.setVisible(false);
    voicingParamsComponent.setVisible(false);
    optionsParamsComponent.setVisible(false);
    waveformMemoryParamsComponent.setVisible(false);
    midiEchoParamsComponent.setVisible(false);
    filterParamsComponent.setVisible(false);
    scopeComponent.setVisible(false);
    wavePatternsComponent.setVisible(false);

    OscButton.setToggleState(false);
    EffectButton.setToggleState(false);
  }

  if (button == &OscButton.button) {
    chipOscComponent.setVisible(true);
    waveformMemoryParamsComponent.setVisible(true);
    scopeComponent.setVisible(true);
    wavePatternsComponent.setVisible(true);

    OscButton.setToggleState(true);
  } else if (button == &EffectButton.button) {
    chipOscComponent.setVisible(true);
    scopeComponent.setVisible(true);

    sweepParamsComponent.setVisible(true);
    vibratoParamsComponent.setVisible(true);
    midiEchoParamsComponent.setVisible(true);
    filterParamsComponent.setVisible(true);
    voicingParamsComponent.setVisible(true);
    optionsParamsComponent.setVisible(true);

    EffectButton.setToggleState(true);
  }
  resized();
}