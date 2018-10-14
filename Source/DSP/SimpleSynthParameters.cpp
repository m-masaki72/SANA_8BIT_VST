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

VibratoParameters::VibratoParameters(AudioParameterBool* vibratoEnable, AudioParameterFloat* vibratoAmount, AudioParameterFloat* vibratoSpeed, AudioParameterFloat* vibratoAttackTime)
	: VibratoEnable(vibratoEnable)
	, VibratoAmount(vibratoAmount)
	, VibratoSpeed(vibratoSpeed)
	, VibratoAttackTime(vibratoAttackTime)
{
}

void VibratoParameters::addAllParameters(AudioProcessor& processor)
{
	processor.addParameter(VibratoEnable);
	processor.addParameter(VibratoAmount);
	processor.addParameter(VibratoSpeed);
	processor.addParameter(VibratoAttackTime);
}

void VibratoParameters::saveParameters(XmlElement & xml)
{
	xml.setAttribute(VibratoEnable->paramID, VibratoEnable->get());
	xml.setAttribute(VibratoAmount->paramID, (double)VibratoAmount->get());
	xml.setAttribute(VibratoSpeed->paramID, (double)VibratoSpeed->get());
	xml.setAttribute(VibratoAttackTime->paramID, (double)VibratoAttackTime->get());
}

void VibratoParameters::loadParameters(XmlElement & xml)
{
	*VibratoEnable = (bool)xml.getBoolAttribute(VibratoEnable->paramID, true);
	*VibratoAmount = (float)xml.getDoubleAttribute(VibratoAmount->paramID, 0.3);
	*VibratoSpeed = (float)xml.getDoubleAttribute(VibratoSpeed->paramID, 2.0);
	*VibratoAttackTime = (float)xml.getDoubleAttribute(VibratoAttackTime->paramID, 0.0);
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

WaveformMemoryParameters::WaveformMemoryParameters()
	: WaveSamplesArray{
	new AudioParameterInt("w0", "w0", 0, 31, 0),
	new AudioParameterInt("w1", "w1", 0, 31, 0),
	new AudioParameterInt("w2", "w2", 0, 31, 0),
	new AudioParameterInt("w3", "w3", 0, 31, 0),
	new AudioParameterInt("w4", "w4", 0, 31, 0),
	new AudioParameterInt("w5", "w5", 0, 31, 0),
	new AudioParameterInt("w6", "w6", 0, 31, 0),
	new AudioParameterInt("w7", "w7", 0, 31, 0),
	new AudioParameterInt("w8", "w8", 0, 31, 0),
	new AudioParameterInt("w9", "w9", 0, 31, 0),
	new AudioParameterInt("w10", "w10", 0, 31, 0),
	new AudioParameterInt("w11", "w11", 0, 31, 0),
	new AudioParameterInt("w12", "w12", 0, 31, 0),
	new AudioParameterInt("w13", "w13", 0, 31, 0),
	new AudioParameterInt("w14", "w14", 0, 31, 0),
	new AudioParameterInt("w15", "w15", 0, 31, 0),
	new AudioParameterInt("w16", "w16", 0, 31, 0),
	new AudioParameterInt("w17", "w17", 0, 31, 0),
	new AudioParameterInt("w18", "w18", 0, 31, 0),
	new AudioParameterInt("w19", "w19", 0, 31, 0),
	new AudioParameterInt("w20", "w20", 0, 31, 0),
	new AudioParameterInt("w21", "w21", 0, 31, 0),
	new AudioParameterInt("w22", "w22", 0, 31, 0),
	new AudioParameterInt("w23", "w23", 0, 31, 0),
	new AudioParameterInt("w24", "w24", 0, 31, 0),
	new AudioParameterInt("w25", "w25", 0, 31, 0),
	new AudioParameterInt("w26", "w26", 0, 31, 0),
	new AudioParameterInt("w27", "w27", 0, 31, 0),
	new AudioParameterInt("w28", "w28", 0, 31, 0),
	new AudioParameterInt("w29", "w29", 0, 31, 0),
	new AudioParameterInt("w30", "w30", 0, 31, 0),
	new AudioParameterInt("w31", "w31", 0, 31, 0)
	}
{
}

void WaveformMemoryParameters::addAllParameters(AudioProcessor& processor)
{
	for (int i = 0; i < 32; i++)
	{
		processor.addParameter(WaveSamplesArray[i]);
	}
}

void WaveformMemoryParameters::saveParameters(XmlElement & xml)
{
	for (int i = 0; i < 32; i++)
	{
		xml.setAttribute(WaveSamplesArray[i]->paramID, (int)WaveSamplesArray[i]->get());
	}
}

void WaveformMemoryParameters::loadParameters(XmlElement & xml)
{
	for (int i = 0; i < 32; i++)
	{
		*WaveSamplesArray[i] = xml.getIntAttribute(WaveSamplesArray[i]->paramID, 0);
	}
}