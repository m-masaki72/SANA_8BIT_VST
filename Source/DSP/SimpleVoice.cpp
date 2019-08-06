/*
  ==============================================================================

    SimpleVoice.cpp
    Created: 20 Apr 2018 2:08:09am
	Modified: 12 September 2018
	Author:  MasakiMori, COx2

  ==============================================================================
*/
#include "SimpleVoice.h"

namespace {
	const float HALF_PI = MathConstants<float>::halfPi;
	const float ONE_PI = MathConstants<float>::pi;
	const float TWO_PI = MathConstants<float>::twoPi;
}

SimpleVoice::SimpleVoice(
	ChipOscillatorParameters* chipOscParams, 
	SweepParameters* sweepParams, 
	VibratoParameters* vibratoParams, 
	VoicingParameters* voicingParams, 
	OptionsParameters* optionsParams, 
	MidiEchoParameters* midiEchoParams, 
	WaveformMemoryParameters* waveformMemoryParams,
	std::set<int>* midiList
)
	: _chipOscParamsPtr(chipOscParams)
	, _sweepParamsPtr(sweepParams)
	, _vibratoParamsPtr(vibratoParams)
	, _voicingParamsPtr(voicingParams)
	, _optionsParamsPtr(optionsParams)
	, _midiEchoParamsPtr(midiEchoParams)
	, _waveformMemoryParamsPtr(waveformMemoryParams)
	, ampEnv(chipOscParams->Attack->get(), chipOscParams->Decay->get(), chipOscParams->Sustain->get(), chipOscParams->Release->get(), midiEchoParams->EchoDuration->get() * midiEchoParams->EchoRepeat->get())
	, vibratoEnv(vibratoParams->VibratoAttackTime->get(), 0.1f, 1.0f, 0.1f, 0.0f)
	, portaEnv(voicingParams->StepTime->get(), 0.0f, 1.0f, 0.0f, 0.0f)
	, currentAngle(0.0f), vibratoAngle(0.0f), angleDelta(0.0f), portaAngleDelta(0.0f)
	, level(0.0f)
	, pitchBend(0.0f), pitchSweep(0.0f)
	, eb((std::int32_t)getSampleRate(), (float)midiEchoParams->EchoDuration->get(), midiEchoParams->EchoRepeat->get())
{
	_midiList = midiList;
}

bool SimpleVoice::canPlaySound(SynthesiserSound* sound)
{
	return dynamic_cast<const SimpleSound*> (sound) != nullptr;
}

void SimpleVoice::startNote(int midiNoteNumber, float velocity, SynthesiserSound* sound, int currentPitchWheelPosition)
{
	DBG("[StartNote] NoteNumber: " + juce::String(midiNoteNumber) + ", Velocity: " + juce::String(velocity));

	if (canStartNote())
	{

		if (SimpleSound* soundForPlay = dynamic_cast<SimpleSound*> (sound))
		{
			//パラメータ初期化
			{
				pitchSweep = 0.0f;
				waveForms.init();
				eb.updateParam(_midiEchoParamsPtr->EchoDuration->get(), _midiEchoParamsPtr->EchoRepeat->get());
			}

			if (velocity <= 0.01f) {
				velocity = 0.01f;
			}
			level = velocity * 0.8f;

			pitchBend = ((float)currentPitchWheelPosition - 8192.0f) / 8192.0f;

			// 生成する波形のピッチを再現するサンプルデータ間の角度差⊿θ[rad]の値を決定する。
			float cyclesPerSecond = (float)MidiMessage::getMidiNoteInHertz(midiNoteNumber, _optionsParamsPtr->PitchStandard->get());
			float cyclesPerSample = (float)cyclesPerSecond / (float)getSampleRate();
			angleDelta = cyclesPerSample * TWO_PI;

			ampEnv.attackStart();
			vibratoEnv.attackStart();
			portaEnv.attackStart();
		}
	}
}

//   キーリリースだとallowTailOff == true、キーリリース直後のボイススチールではallowTailOff == false
void SimpleVoice::stopNote(float /*velocity*/, bool allowTailOff)
{
	DBG("stopNote : " + juce::String((std::int32_t)allowTailOff));
	if (isArpMode() && _midiList->size() >= 1)
	{
		// Nothing to do
	}
	else
	{
		if (allowTailOff)
		{
			ampEnv.releaseStart();
		}
		else
		{
			// キーホールド中(ADSのいずれか)であればangleDeltaをリリース状態に移行
			if (ampEnv.isHolding()) {
				//ポルタメント処理，一つ前のピッチを記憶しておく
				portaAngleDelta = angleDelta;

				// ボイススチールを受けて直ぐに音量を0にしてしまうと、急峻な変化となりノイズの発生を引き起こすため、それを予防する処理。
				ampEnv.releaseStart();
			}
			else
			{
				portaAngleDelta = 0.0f;
			}
			clearCurrentNote(); // ここの処理がいつ通るのか要検証
			_midiList->clear();
		}
	}
}

void SimpleVoice::pitchWheelMoved(int newPitchWheelValue)
{
	pitchBend = ((float)newPitchWheelValue - 8192.0f) / 8192.0f;
}

void SimpleVoice::controllerMoved(int /*controllerNumber*/, int /*newControllerValue*/)
{

}

int timer = 0;

void SimpleVoice::renderNextBlock(AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
	timer++;
	if (timer > 10) timer = 0;

	if (isArpMode())
	{
		// angleDeltaの特殊制御	
		if (_midiList->size() >= 2)
		{
			if (timer > 5) {
				float cyclesPerSecond = (float)MidiMessage::getMidiNoteInHertz(*_midiList->begin(), _optionsParamsPtr->PitchStandard->get());
				float cyclesPerSample = (float)cyclesPerSecond / (float)getSampleRate();
				angleDelta = cyclesPerSample * TWO_PI;
			}
			else
			{
				auto itr = _midiList->begin();
				++itr;
				float cyclesPerSecond = (float)MidiMessage::getMidiNoteInHertz(*itr, _optionsParamsPtr->PitchStandard->get());
				float cyclesPerSample = (float)cyclesPerSecond / (float)getSampleRate();
				angleDelta = cyclesPerSample * TWO_PI;
			}
		}
	}

	if (SimpleSound* playingSound = static_cast<SimpleSound*>(getCurrentlyPlayingSound().get()))
	{
		if (angleDelta != 0.0f)
		{
			while (--numSamples >= 0)
			{
				// Vibrato:モジュレーション波形のサンプルデータを生成する。
				float modulationFactor = calcModulationFactor(vibratoAngle);
				modulationFactor *= vibratoEnv.getValue() * vibratoEnv.getValue();

				// OSC MIX: 
				float currentSample = 0.0f;

				currentSample += angle2wave(currentAngle);

				// AMP EG: エンベロープの値をサンプルデータに反映する。
				currentSample *= ampEnv.getValue() * level;

				//エコー処理とエコーレンダリング
				if (_midiEchoParamsPtr->IsEchoEnable->get())
				{
					eb.addSample(currentSample, _midiEchoParamsPtr->VolumeOffset->get() / 100.0f);
					eb.cycle();

					for (auto channelNum = outputBuffer.getNumChannels(); --channelNum >= 0;)
					{
						for (auto i = 0, repeatNum = _midiEchoParamsPtr->EchoRepeat->get(); i < repeatNum; ++i)
						{
							outputBuffer.addSample(channelNum, startSample, eb.getSample(i));
						}
					}
				}

				//	リアルタイム音処理  バッファに対して加算処理を行う。ポリフォニックでは、各ボイスの音を加算処理する必要がある。
				for (auto channelNum = outputBuffer.getNumChannels(); --channelNum >= 0;) 
				{
					outputBuffer.addSample(channelNum, startSample, currentSample);
				}

				// Voiceの初期化処理
				// エンベロープにおいて，エフェクトエコーが終わっている or リリース状態のとき				
				if (ampEnv.isReleaseEnded() || (_midiEchoParamsPtr->IsEchoEnable->get() && ampEnv.isEchoEnded()))
				{					
					clearCurrentNote();			 // このボイスが生成するノート情報をクリアする。
					angleDelta = 0.0f;			 // 変数を初期値に戻す。
					portaAngleDelta = 0.0f;
					currentAngle = 0.0f;
					pitchSweep = 0.0f;
					break;	
				}
				
				//============ 次のサンプルデータを生成するための準備 ============

				//ピッチ処理
				//モジュレーションとピッチベンド，ピッチスイープにより変化
				if (_vibratoParamsPtr->VibratoAttackDeleySwitch->get() == false && vibratoEnv.getState() == AmpEnvelope::AMPENV_STATE::ATTACK)
				{
					modulationFactor = 0.0f;
				}
				else if (_vibratoParamsPtr->VibratoEnable->get() == false)
				{
					modulationFactor = 0.0f;
				}
				currentAngle += angleDelta
					* pow(2.0f, pitchBend / 13.0f * _optionsParamsPtr->PitchBendRange->get())
					* pow(2.0f, pitchSweep)
					* pow(2.0f, modulationFactor / 13.0f);

				// ポルタメント処理
				//カレントアングルにエンベロープ処理を施す
				if (_voicingParamsPtr->VoicingSwitch->getCurrentChoiceName() == "PORTAMENTO") {
					if (portaAngleDelta > 0.0f) {
						currentAngle -= (angleDelta - portaAngleDelta) * (1 - portaEnv.getValue());
					}
				}

				// ビブラートエフェクト・スイープエフェクト
				// currentのピッチに対して処理を行う．
				vibratoAngle += (_vibratoParamsPtr->VibratoSpeed->get() / (float)getSampleRate()) * TWO_PI;

				if (_sweepParamsPtr->SweepSwitch->getCurrentChoiceName() == "Positive")
				{
					pitchSweep += 1 / (float)getSampleRate() / (float)_sweepParamsPtr->SweepTime->get();
				}
				else if (_sweepParamsPtr->SweepSwitch->getCurrentChoiceName() == "Negative")
				{
					pitchSweep -= 1 / (float)getSampleRate() / (float)_sweepParamsPtr->SweepTime->get();
				}

				if (pitchSweep >= 10.0f)
				{
					pitchSweep = 10.0f;
				}

				if (currentAngle > TWO_PI) 
				{
					currentAngle = fmod(currentAngle, TWO_PI);
				}
				if (vibratoAngle > TWO_PI) 
				{
					vibratoAngle = fmod(vibratoAngle, TWO_PI);
				}

				// エンベロープパラメータを更新して時間分進める
				ampEnv.setParameters(_chipOscParamsPtr->Attack->get(), _chipOscParamsPtr->Decay->get(), _chipOscParamsPtr->Sustain->get(), _chipOscParamsPtr->Release->get(), _midiEchoParamsPtr->EchoDuration->get() * _midiEchoParamsPtr->EchoRepeat->get());
				ampEnv.cycle((float)getSampleRate());
				vibratoEnv.setParameters(_vibratoParamsPtr->VibratoAttackTime->get(), 0.1f, 1.0f, 0.1f, 0.0f);
				vibratoEnv.cycle((float)getSampleRate());
				portaEnv.setParameters(_voicingParamsPtr->StepTime->get(), 0.0f, 1.0f, 0.0f, 0.0f);
				portaEnv.cycle((float)getSampleRate());
				eb.updateParam(_midiEchoParamsPtr->EchoDuration->get(), _midiEchoParamsPtr->EchoRepeat->get());
				// 書き込み先のオーディオバッファのサンプルインデックス値をインクリメントする。
				++startSample;

				if (isArpMode() &&  _midiList->size() <= 0) // Arpモード用の緊急処理，普段の処理では通らない
				{
					ampEnv.releaseStart();
					_midiList->clear();
				}
			}
		}
	}
}

float SimpleVoice::calcModulationFactor(float angle)
{
	float factor = waveForms.sine(angle);

	// factorの値が0.5を中心とした0.0～1.0の値となるように調整する。
	factor *= _vibratoParamsPtr->VibratoAmount->get();
	return factor;
}

float SimpleVoice::angle2wave(float angle) 
{
	auto value = 0.0f;

	if (_chipOscParamsPtr->OscWaveType->getCurrentChoiceName() == "NES_Square50%") {
		value += waveForms.nesSquare(angle);
	}
	else if (_chipOscParamsPtr->OscWaveType->getCurrentChoiceName() == "NES_Square25%") {
		value += waveForms.nesSquare25(angle);
	}
	else if (_chipOscParamsPtr->OscWaveType->getCurrentChoiceName() == "NES_Square12.5%") {
		value += waveForms.nesSquare125(angle);
	}
	else if (_chipOscParamsPtr->OscWaveType->getCurrentChoiceName() == "NES_Triangle") {
		value += waveForms.nesTriangle(angle);
	}
	else if (_chipOscParamsPtr->OscWaveType->getCurrentChoiceName() == "NES_LongNoise") {
		value += waveForms.longNoise(angle);
	}
	else if (_chipOscParamsPtr->OscWaveType->getCurrentChoiceName() == "NES_ShortNoise") {
		value += waveForms.shortNoise(angle);
	}
	else if (_chipOscParamsPtr->OscWaveType->getCurrentChoiceName() == "Pure_Sine") {
		value += waveForms.sine(angle);
	}
	else if (_chipOscParamsPtr->OscWaveType->getCurrentChoiceName() == "Pure_Saw") {
		value += waveForms.saw(angle);
	}
	else if (_chipOscParamsPtr->OscWaveType->getCurrentChoiceName() == "Pure_Triangle") {
		value += waveForms.triangle(angle);
	}
	else if (_chipOscParamsPtr->OscWaveType->getCurrentChoiceName() == "Pure_Square50%") {
		value += waveForms.square(angle);
	}
	else if (_chipOscParamsPtr->OscWaveType->getCurrentChoiceName() == "Pure_Square25%") {
		value += waveForms.square25(angle);
	}
	else if (_chipOscParamsPtr->OscWaveType->getCurrentChoiceName() == "Pure_Square12.5%") {
		value += waveForms.square125(angle);
	}
	else if (_chipOscParamsPtr->OscWaveType->getCurrentChoiceName() == "Pure_Lo-bitNoise") {
		value += waveForms.lobitNoise(angle);
	}
	else if (_chipOscParamsPtr->OscWaveType->getCurrentChoiceName() == "Waveform Memory") {
		value += waveForms.waveformMemory(angle, _waveformMemoryParamsPtr);
	}
	return value;
}

bool SimpleVoice::isArpMode()
{
	if (_voicingParamsPtr == nullptr)
	{
		return false;
	}

	if (_voicingParamsPtr->VoicingSwitch->getCurrentChoiceName() == "ARPEGGIO")
	{ 
		return true;
	}
	else
	{
		return false;
	}
}

bool SimpleVoice::canStartNote()
{
	if (!isArpMode())
	{
		return true;
	}
	else
	{
		if (_midiList->size() <= 0)
		{
			return true;
		}
		if (ampEnv.isReleasing() || ampEnv.isReleaseEnded() || ampEnv.isEchoEnded())
		{
			return true;
		}

		return false;
	}
}