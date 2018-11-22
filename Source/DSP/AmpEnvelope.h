/*
  ==============================================================================

    AmpEnvelope.h
    Created: 20 Apr 2018 2:08:26am
	Modified: 14 September 2018
	Author:  MasakiMori, COx2

  ==============================================================================
*/

#pragma once

class AmpEnvelope 
{
public:
	enum class AMPENV_STATE
	{
		ATTACK = 0,
		DECAY,
		SUSTAIN,
		RELEASE,
		WAIT,
		ECHO,
	};

	AmpEnvelope(float attackTime, float decayTime, float sustain, float releaseTime, float echoTime);
	~AmpEnvelope();

	AMPENV_STATE getState();
	float getValue();
	void setParameters(float attackTime, float decayTime, float sustain, float releaseTime, float echoTime);
	void checkVarRange();
	void attackStart();
	void releaseStart();
	void releaseEnd();
	bool isHolding();
	bool isReleasing();
	bool isEchoEnded();
	void cycle(float sampleRate);

private:
	AmpEnvelope();

	AMPENV_STATE _ampState;						
	float _attackTime, _decayTime, _sustainValue, _releaseTime, _echoTime;
	float _value, _valueOnReleaseStart, _timer;
	float _sampleRate;
};