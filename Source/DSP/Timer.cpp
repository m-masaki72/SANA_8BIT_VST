/*
  ==============================================================================

	SimpleVoice.cpp
	Created: 11 June 2019
	Author:  MasakiMori

  ==============================================================================
*/

#include "Timer.h"

Timer::Timer()
{
	reset();
}

double Timer::getTime()
{
	return _value;
}

void Timer::reset()
{
	_value = 0.0;
}

void Timer::cycle(float sampleRate)
{
	cycle((double)sampleRate);
}

void Timer::cycle(double sampleRate)
{
	_sampleRate = sampleRate;
	_value += 1.0 / _sampleRate;
}