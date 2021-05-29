#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

class BaseAudioProcessor : public AudioProcessor {
 public:
  BaseAudioProcessor();
  ~BaseAudioProcessor();

#ifndef JucePlugin_PreferredChannelConfigurations
  bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

  bool hasEditor() const override;
  const String getName() const override;
  bool acceptsMidi() const override;
  bool producesMidi() const override;
  bool isMidiEffect() const override;
  double getTailLengthSeconds() const override;
  void changeProgramName(int index, const String& newName) override;
  void releaseResources() override;
};

