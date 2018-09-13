/*
  ==============================================================================

    Waveforms.cpp
    Created: 22 Aug 2018 10:13:35pm
	Modified: 11 September 2018
	Author:  MasakiMori, COx2

  ==============================================================================
*/

// ヘッダファイルをインクルードする。
#include "Waveforms.h"

// ①無名名前空間に定数宣言を記述する。
namespace {
	const float HALF_PI = MathConstants<float>::halfPi;
	const float ONE_PI = MathConstants<float>::pi;
	const float TWO_PI = MathConstants<float>::twoPi;
}

float quantize(float sample)
{
	int qNum = 8;
	return round(sample * qNum) / qNum;
}

// ②正弦波のサンプルデータを生成して返す関数
float Waveforms::sine(float angle)
{
	// 角度（ラジアン）の値が2πを超えている場合は、変数angleの値が0～2πの範囲内に収まるよう剰余を求める。
    if(angle > TWO_PI)
    {
        angle = fmodf(angle, TWO_PI);
    }

	// 角度（ラジアン）に対応する波形のサンプルデータを返す。
    return sinf(angle);
}

// ③鋸波のサンプルデータを生成して返す関数
float Waveforms::saw(float angle)
{
    if(angle > TWO_PI)
    {
        angle = fmodf(angle, TWO_PI);
    }

    if (angle <= ONE_PI)
    {
        return (angle / ONE_PI);
    }
    else
    {
        return -2.0f + (angle / ONE_PI) ;
    }
}

// ④方形波のサンプルデータを生成して返す関数
float Waveforms::square(float angle)
{

	if (angle > TWO_PI)
	{
		angle = fmodf(angle, TWO_PI);
	}

	if (angle <= ONE_PI)
	{
		return 1.0f;
	}
	else
	{
		return -1.0f;
	}
}

float Waveforms::square25(float angle)
{

	if (angle > TWO_PI)
	{
		angle = fmodf(angle, TWO_PI);
	}

	if (angle <= HALF_PI)
	{
		return 1.0f;
	}
	else
	{
		return -1.0f;
	}
}

float Waveforms::square125(float angle)
{

	if (angle > TWO_PI)
	{
		angle = fmodf(angle, TWO_PI);
	}

	if (angle <= HALF_PI / 2)
	{
		return 1.0f;
	}
	else
	{
		return -1.0f;
	}
}

// ⑤三角波のサンプルデータを生成して返す関数
float Waveforms::triangle(float angle)
{
    if(angle > TWO_PI)
    {
        angle = fmodf(angle, TWO_PI);
    }

    if (angle <= HALF_PI)
    {
        return (angle / HALF_PI);
    }
    else if(angle > HALF_PI && angle <= (ONE_PI + HALF_PI))
    {
        return 2.0f - (2.0f * angle / ONE_PI);
    }
    else
    {
        return -4.0f + (angle / HALF_PI);
    }
}

// ⑥ホワイトノイズのサンプルデータを生成して返す関数
float Waveforms::longNoise(float angleDelta)
{
	noiseVal *= 0.99;

	if (counter++ > TWO_PI / angleDelta / (2 << 4)) {
		counter = 0;
		int result = (reg ^ (reg >> 1)) & 1;
		reg = reg >> 1;
		reg |= result << 14;
		noiseVal = (reg & 1) * 2.0 - 1;
	}
	return noiseVal;
}
float Waveforms::shortNoise(float angleDelta)
{
	noiseVal *= 0.99;

	if (counter++ > TWO_PI / angleDelta / (2 << 4)) {
		counter = 0;
		int result = (reg ^ (reg >> 6)) & 1;
		reg = reg >> 1;
		reg |= result << 14;
		noiseVal = (reg & 1) * 2.0 - 1;
	}
	return noiseVal;
}

float Waveforms::nesSquare(float angle)
{
	
	if (angle > TWO_PI)
	{
		angle = fmodf(angle, TWO_PI);
	}

	if (angle <= ONE_PI)
	{
		return 1.0f / pow(1.57f, angle);
	}
	else
	{
		return -1.0f / pow(1.57f, angle-ONE_PI);
	}
}

float Waveforms::nesSquare25(float angle)
{
	if (angle > TWO_PI)
	{
		angle = fmodf(angle, TWO_PI);
	}

	if (angle <= HALF_PI)
	{
		return 1.0f / pow(1.57f, angle);
	}
	else
	{
		return -1.0f / pow(1.57f, angle - HALF_PI);
	}
}

float Waveforms::nesSquare125(float angle)
{
	if (angle > TWO_PI)
	{
		angle = fmodf(angle, TWO_PI);
	}

	if (angle <= HALF_PI/2)
	{
		return 1.0f / pow(1.57f, angle);
	}
	else
	{
		return -1.0f / pow(1.57f, angle - HALF_PI/2);
	}
}

float Waveforms::nesTriangle(float angle)
{
	if (angle > TWO_PI)
	{
		angle = fmodf(angle, TWO_PI);
	}

	if (angle <= HALF_PI)
	{
		return quantize(angle / HALF_PI);
	}
	else if (angle > HALF_PI && angle <= (ONE_PI + HALF_PI))
	{
		return quantize(2.0f - (2.0f * angle / ONE_PI));
	}
	else
	{
		return quantize(-4.0f + (angle / HALF_PI));
	}
}

float Waveforms::waveformMemory(float angle, WaveformMemoryParameters* _waveformMemoryParamsPtr)
{
	if (angle > TWO_PI)
	{
		angle = fmodf(angle, TWO_PI);
	}

	float DELTA_PI = TWO_PI / 32.0f;

	if (angle <= DELTA_PI * 6)
	{
		int val, ans;
		int i = 0;
		val = _waveformMemoryParamsPtr->WaveSamples0_5->get();
		val >>= 8;
		do
		{
			ans = val & 0xF;
			val >>= 4;
			i++;
		} while (angle > DELTA_PI * i);

		return ans / 8.0f - 1.0f;
	}
	else if (DELTA_PI * 6  < angle && angle <= DELTA_PI * 12)
	{
		int val, ans;
		int i = 6;
		val = _waveformMemoryParamsPtr->WaveSamples6_11->get();
		val >>= 8;
		do
		{
			ans = val & 0xF;
			val >>= 4;
			i++;
		} while (angle > DELTA_PI * i);

		return ans / 8.0f - 1.0f;
	}
	else if (DELTA_PI * 12  < angle && angle <= DELTA_PI * 18)
	{
		int val, ans;
		int i = 12;
		val = _waveformMemoryParamsPtr->WaveSamples12_17->get();
		val >>= 8;
		do
		{
			ans = val & 0xF;
			val >>= 4;
			i++;
		} while (angle > DELTA_PI * i);

		return ans / 8.0f - 1.0f;
	}
	else if (DELTA_PI * 18  < angle && angle <= DELTA_PI * 24)
	{
		int val, ans;
		int i = 18;
		val = _waveformMemoryParamsPtr->WaveSamples18_23->get();
		val >>= 8;
		do
		{
			ans = val & 0xF;
			val >>= 4;
			i++;
		} while (angle > DELTA_PI * i);

		return ans / 8.0f - 1.0f;
	}
	else if (DELTA_PI * 24  < angle && angle <= DELTA_PI * 30)
	{
		int val, ans;
		int i = 24;
		val = _waveformMemoryParamsPtr->WaveSamples24_29->get();
		val >>= 8;
		do
		{
			ans = val & 0xF;
			val >>= 4;
			i++;
		} while (angle > DELTA_PI * i);

		return ans / 8.0f - 1.0f;
	}
	else
	{
		int val, ans;
		int i = 30;
		val = _waveformMemoryParamsPtr->WaveSamples30_31->get();
		val >>= 24;
		do
		{
			ans = val & 0xF;
			val >>= 4;
			i++;
		} while (angle > DELTA_PI * i && DELTA_PI * i < TWO_PI);

		return ans / 8.0f - 1.0f;
	}
}
