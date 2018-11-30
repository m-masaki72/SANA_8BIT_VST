/*
  ==============================================================================

    Waveforms.h
    Created: 22 Aug 2018 10:13:35pm
	Modified: 11 September 2018
	Author:  MasakiMori, COx2

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "SimpleSynthParameters.h"

class Waveforms
{
public:
	Waveforms();
	void init();
	float nesTriangle(float angle);
	float nesSquare(float angle);
	float nesSquare25(float angle);
	float nesSquare125(float angle);
    float sine(float angle);
    float saw(float angle);
	float square(float angle);
	float square25(float angle);
	float square125(float angle);
    float triangle(float angle);
    float longNoise(float angleDelta);
	float shortNoise(float angleDelta);
	float lobitNoise(float angleDelta);
	float waveformMemory(float angleDelta, WaveformMemoryParameters* _waveformMemoryParamsPtr);

private:
	float quantize(float sample);
	float checkAngleRanage(float angle);

	int noizeReg = 1 << 14;
	float noiseVal = 1.0f;
	int freqCounter = 0;
	Random rand;
};