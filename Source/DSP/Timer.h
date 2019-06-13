/*
  ==============================================================================

	SimpleVoice.cpp
	Created: 11 June 2019
	Author:  MasakiMori

  ==============================================================================
*/

class Timer
{
public:
	Timer();
	~Timer() = default;
	double getTime();
	void cycle(float sampleRate);
	void cycle(double sampleRate);
	void reset();

private:
	double _value;
	double _sampleRate;
};

