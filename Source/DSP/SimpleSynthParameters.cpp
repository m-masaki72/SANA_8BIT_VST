/*
  ==============================================================================

    SimpleSynthParameters.cpp
    Created: 10 May 2018 12:28:18am
	Modified: 11 September 2018
	Author:  MasakiMori, COx2

  ==============================================================================
*/

#include "SimpleSynthParameters.h"

ChipOscillatorParameters::ChipOscillatorParameters(AudioParameterChoice* oscWaveType, AudioParameterFloat* volumeLevel,	AudioParameterFloat* attack, AudioParameterFloat* decay, AudioParameterFloat* sustain, AudioParameterFloat* release)
	:OscWaveType(oscWaveType),VolumeLevel(volumeLevel), Attack(attack), Decay(decay), Sustain(sustain), Release(release)
{
}

void ChipOscillatorParameters::addAllParameters(AudioProcessor& processor)
{
	// 引数で受け取ったAudioProcessorクラスの参照を介してaddParameter関数を実行してパラメータを追加する。
	processor.addParameter(OscWaveType);
	processor.addParameter(VolumeLevel);
	processor.addParameter(Attack);
	processor.addParameter(Decay);
	processor.addParameter(Sustain);
	processor.addParameter(Release);
}

void ChipOscillatorParameters::saveParameters(XmlElement& xml)
{
	// 引数で受け取ったXmlElementクラスの参照を介して各パラメータのIDと値のペアをXMLフォーマットで書き込む。
	xml.setAttribute(OscWaveType->paramID, OscWaveType->getIndex());
	xml.setAttribute(VolumeLevel->paramID, (double)VolumeLevel->get());
	xml.setAttribute(Attack->paramID, (double)Attack->get());
	xml.setAttribute(Decay->paramID, (double)Decay->get());
	xml.setAttribute(Sustain->paramID, (double)Sustain->get());
	xml.setAttribute(Release->paramID, (double)Release->get());
}

void ChipOscillatorParameters::loadParameters(XmlElement& xml)
{
	// 引数で受け取ったXmlElementクラスの参照を介してXMLフォーマットのデータから各パラメータに対応する値を読み込んで代入する。
	*OscWaveType = xml.getIntAttribute(OscWaveType->paramID, 0);
	*VolumeLevel = (float)xml.getDoubleAttribute(VolumeLevel->paramID, 1.0);
	*Attack = (float)xml.getDoubleAttribute(Attack->paramID, 0.01);
	*Decay = (float)xml.getDoubleAttribute(Decay->paramID, 0.01);
	*Sustain = (float)xml.getDoubleAttribute(Sustain->paramID, 1.0);
	*Release = (float)xml.getDoubleAttribute(Release->paramID, 0.01);
}

//-----------------------------------------------------------------------------------------

SweepParameters::SweepParameters(AudioParameterChoice* sweepSwitch, AudioParameterFloat* sweepTime)
	: SweepSwitch(sweepSwitch), SweepTime(sweepTime)
{
}

void SweepParameters::addAllParameters(AudioProcessor& processor)
{
	processor.addParameter(SweepSwitch);
	processor.addParameter(SweepTime);
}

void SweepParameters::saveParameters(XmlElement & xml)
{
	xml.setAttribute(SweepSwitch->paramID, SweepSwitch->getIndex());
	xml.setAttribute(SweepTime->paramID, (double)SweepTime->get());
}

void SweepParameters::loadParameters(XmlElement & xml)
{
	*SweepSwitch = xml.getIntAttribute(SweepSwitch->paramID, 0);
	*SweepTime = (float)xml.getDoubleAttribute(SweepTime->paramID, 1.0);
}

//-----------------------------------------------------------------------------------------

VibratoParameters::VibratoParameters(AudioParameterBool* vibratoEnable, AudioParameterFloat* vibratoAmount, AudioParameterFloat* vibratoSpeed)
	: VibratoEnable(vibratoEnable)
	, VibratoAmount(vibratoAmount)
	, VibratoSpeed(vibratoSpeed)
{
}

void VibratoParameters::addAllParameters(AudioProcessor& processor)
{
	processor.addParameter(VibratoEnable);
	processor.addParameter(VibratoAmount);
	processor.addParameter(VibratoSpeed);
}

void VibratoParameters::saveParameters(XmlElement & xml)
{
	xml.setAttribute(VibratoEnable->paramID, VibratoEnable->get());
	xml.setAttribute(VibratoAmount->paramID, (double)VibratoAmount->get());
	xml.setAttribute(VibratoSpeed->paramID, (double)VibratoSpeed->get());
}

void VibratoParameters::loadParameters(XmlElement & xml)
{
	*VibratoEnable = (bool)xml.getBoolAttribute(VibratoEnable->paramID, true);
	*VibratoAmount = (float)xml.getDoubleAttribute(VibratoAmount->paramID, 0.3);
	*VibratoSpeed = (float)xml.getDoubleAttribute(VibratoSpeed->paramID, 2.0);
}

//-----------------------------------------------------------------------------------------

OptionsParameters::OptionsParameters(AudioParameterBool* isPolyMode, AudioParameterBool* isVelocitySense, AudioParameterInt* pitchBendRange, AudioParameterInt* pitchStandard)
	: IsPolyMode(isPolyMode)
	, IsVelocitySense(isVelocitySense)
	, PitchBendRange(pitchBendRange)
	, PitchStandard(pitchStandard)
{
}

void OptionsParameters::addAllParameters(AudioProcessor& processor)
{
	processor.addParameter(IsPolyMode);
	processor.addParameter(IsVelocitySense);
	processor.addParameter(PitchBendRange);
	processor.addParameter(PitchStandard);
}

void OptionsParameters::saveParameters(XmlElement & xml)
{
	xml.setAttribute(IsPolyMode->paramID, IsPolyMode->get());
	xml.setAttribute(IsVelocitySense->paramID, IsVelocitySense->get());
	xml.setAttribute(PitchBendRange->paramID, (double)PitchBendRange->get());
	xml.setAttribute(PitchStandard->paramID, (double)PitchStandard->get());
}

void OptionsParameters::loadParameters(XmlElement & xml)
{
	*IsPolyMode = xml.getBoolAttribute(IsPolyMode->paramID, true);
	*IsVelocitySense = xml.getBoolAttribute(IsVelocitySense->paramID, true);
	*PitchBendRange = xml.getIntAttribute(PitchBendRange->paramID, 2);
	*PitchStandard = xml.getIntAttribute(PitchStandard->paramID, 440);
}

//-----------------------------------------------------------------------------------------

WaveformMemoryParameters::WaveformMemoryParameters(AudioParameterInt* waveSamples0_5, AudioParameterInt* waveSamples6_11, AudioParameterInt* waveSamples12_17, AudioParameterInt* waveSamples18_23,	AudioParameterInt* waveSamples24_29, AudioParameterInt* waveSamples30_31)
	: WaveSamples0_5(waveSamples0_5)
	, WaveSamples6_11(waveSamples6_11)
	, WaveSamples12_17(waveSamples12_17)
	, WaveSamples18_23(waveSamples18_23)
	, WaveSamples24_29(waveSamples24_29)
	, WaveSamples30_31(waveSamples30_31)
{
}

void WaveformMemoryParameters::addAllParameters(AudioProcessor& processor)
{
	processor.addParameter(WaveSamples0_5);
	processor.addParameter(WaveSamples6_11);
	processor.addParameter(WaveSamples12_17);
	processor.addParameter(WaveSamples18_23);
	processor.addParameter(WaveSamples24_29);
	processor.addParameter(WaveSamples30_31);
}

void WaveformMemoryParameters::saveParameters(XmlElement & xml)
{
	xml.setAttribute(WaveSamples0_5->paramID, (int)WaveSamples0_5->get());
	xml.setAttribute(WaveSamples6_11->paramID, (int)WaveSamples6_11->get());
	xml.setAttribute(WaveSamples12_17->paramID, (int)WaveSamples12_17->get());
	xml.setAttribute(WaveSamples18_23->paramID, (int)WaveSamples18_23->get());
	xml.setAttribute(WaveSamples24_29->paramID, (int)WaveSamples24_29->get());
	xml.setAttribute(WaveSamples30_31->paramID, (int)WaveSamples30_31->get());
}

void WaveformMemoryParameters::loadParameters(XmlElement & xml)
{
	*WaveSamples0_5 = xml.getIntAttribute(WaveSamples0_5->paramID, 0);
	*WaveSamples6_11 = xml.getIntAttribute(WaveSamples6_11->paramID, 0);
	*WaveSamples12_17 = xml.getIntAttribute(WaveSamples12_17->paramID, 0);
	*WaveSamples18_23 = xml.getIntAttribute(WaveSamples18_23->paramID, 0);
	*WaveSamples24_29 = xml.getIntAttribute(WaveSamples24_29->paramID, 0);
	*WaveSamples30_31 = xml.getIntAttribute(WaveSamples30_31->paramID, 0);
}