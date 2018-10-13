/*
  ==============================================================================

    Waveforms.h
    Created: 22 Aug 2018 10:13:35pm
	Modified: 11 September 2018
	Author:  MasakiMori, COx2

  ==============================================================================
*/

#pragma once

// ①JUCEライブラリのヘッダをインクルードする。
#include "../JuceLibraryCode/JuceHeader.h"
#include "SimpleSynthParameters.h"

// ②クラス宣言
class Waveforms
{
public:
	int reg = 1 << 14;
	float noiseVal = 1.0f;
	int counter = 0;

	// ③各種波形のサンプルデータを返す関数を宣言する。
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
};