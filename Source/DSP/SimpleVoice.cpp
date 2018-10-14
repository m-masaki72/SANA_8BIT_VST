/*
  ==============================================================================

    SimpleVoice.cpp
    Created: 20 Apr 2018 2:08:09am
	Modified: 12 September 2018
	Author:  MasakiMori, COx2

  ==============================================================================
*/

// ヘッダファイルをインクルードする。
#include "SimpleVoice.h"

// ①無名名前空間に定数宣言を記述する。
namespace {
	const float HALF_PI = MathConstants<float>::halfPi;
	const float ONE_PI = MathConstants<float>::pi;
	const float TWO_PI = MathConstants<float>::twoPi;
}

// ②引数付きコンストラクタ
SimpleVoice::SimpleVoice(ChipOscillatorParameters* chipOscParams, SweepParameters* sweepParams, VibratoParameters* vibratoParams, OptionsParameters* optionsParams, WaveformMemoryParameters* waveformMemoryParams)
	: _chipOscParamsPtr(chipOscParams)
	, _sweepParamsPtr(sweepParams)
	, _vibratoParamsPtr(vibratoParams)
	, _optionsParamsPtr(optionsParams)
	, _waveformMemoryParamsPtr(waveformMemoryParams)
	, ampEnv(chipOscParams->Attack->get(), chipOscParams->Decay->get(), chipOscParams->Sustain->get(), chipOscParams->Release->get())
	, vibratoEnv(vibratoParams->VibratoAttackTime->get(), 0.0f, 1.0f, 0.0f)
	, currentAngle(0.0f), vibratoAngle(0.0f), angleDelta(0.0f)
	, level(0.0f), lastLevel(0.0f), levelDiff(0.0f)
	, pitchBend(0.0f), pitchSweep(0.0f)
{}

// デストラクタ
SimpleVoice::~SimpleVoice()
{}

// ③当ボイスクラスに対応するサウンドクラスが登録されているかどうかを判定する関数。
//   Synthesiserクラスから呼び出されるインターフェース。
bool SimpleVoice::canPlaySound(SynthesiserSound* sound)
{
	return dynamic_cast<const SimpleSound*> (sound) != nullptr;
}

// ④Synthesiserクラスから呼び出されるノートONのトリガー。
//   引数で渡されたノート番号、ベロシティなどの値に対応して波形を生成する前準備を行う。
void SimpleVoice::startNote(int midiNoteNumber, float velocity, SynthesiserSound* sound, int currentPitchWheelPosition)
{
	// 引数として受け取ったノート番号の値とベロシティの値をログとして出力する。
	DBG("[StartNote] NoteNumber: " + juce::String(midiNoteNumber) + ", Velocity: " + juce::String(velocity));

	// ノートON対象のサウンドクラスが当ボイスクラスに対応したものであるかどうかを判定する。
	if (SimpleSound* soundForPlay = dynamic_cast<SimpleSound*> (sound))
	{
		//パラメータ初期化
		{
			pitchSweep = 0.0f;
		}
		// ベロシティ有効/無効のフラグに応じて音量レベルを決定する。有効...ベロシティの値から算出する。 無効...固定値を使用する。
		if (_optionsParamsPtr->IsVelocitySense->get())
		{
			if (velocity <= 0.01f) {
				velocity = 0.01f;
			}
			level = velocity * 0.4f;
		}
		else
		{
			level = 0.8f;
		}
		// ノイズ対策...今回と前回の音量レベルの差分を算出して保持する。
		levelDiff = level - lastLevel;

		pitchBend = ((float)currentPitchWheelPosition - 8192.0f) / 8192.0f;

		// 生成する波形のピッチを再現するサンプルデータ間の角度差⊿θ[rad]の値を決定する。
		float cyclesPerSecond = (float)MidiMessage::getMidiNoteInHertz(midiNoteNumber, _optionsParamsPtr->PitchStandard->get());
		float cyclesPerSample = (float)cyclesPerSecond / (float)getSampleRate();
		angleDelta = cyclesPerSample * TWO_PI;

		ampEnv.attackStart();
		vibratoEnv.attackStart();
	}
}

// ⑤Synthesiserクラスから呼び出されるノートOFFのトリガー。
//   引数で渡される値に対応して波形生成を終了する前準備を行う。
//   キーリリースだとallowTailOff == true、キーリリース直後のボイススチールではallowTailOff == false
void SimpleVoice::stopNote(float velocity, bool allowTailOff)
{
	DBG("stopNote : " + juce::String((int)allowTailOff));

	lastLevel = level;

	if (allowTailOff)
	{
		ampEnv.releaseStart();
		vibratoEnv.releaseStart();
	}
	else
	{
		// キーホールド中(ADSのいずれか)であればangleDeltaをリリース状態に移行
		if (ampEnv.isHolding()) {
			// ボイススチールを受けて直ぐに音量を0にしてしまうと、急峻な変化となりノイズの発生を引き起こすため、それを予防する処理。
			ampEnv.releaseStart();
			vibratoEnv.releaseStart();
		}
		// ボイススチール処理の過程で現在のノート再生状態をクリアする
		clearCurrentNote();
	}
}

// ⑥ピッチホイール操作をトリガーとしてSynthesiserクラスから呼び出される。ピッチベンドの値を保持する変数を更新する。
void SimpleVoice::pitchWheelMoved(int newPitchWheelValue)
{
	pitchBend = ((float)newPitchWheelValue - 8192.0f) / 8192.0f;
}

// ⑦MIDI CC受信をトリガーとしてSynthesiserクラスから呼び出される関数。
void SimpleVoice::controllerMoved(int controllerNumber, int newControllerValue)
{}

// ⑧オーディオバッファに波形をレンダリングする関数。Synthesiserクラスから呼び出される
void SimpleVoice::renderNextBlock(AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
	if (SimpleSound* playingSound = static_cast<SimpleSound*>(getCurrentlyPlayingSound().get()))
	{
		if (angleDelta != 0.0f)
		{
			while (--numSamples >= 0)
			{
				// Vibrato:モジュレーション波形のサンプルデータを生成する。
				float modulationFactor = calcModulationFactor(vibratoAngle);
				modulationFactor *= vibratoEnv.getValue();

				// OSC MIX: 
				float currentSample = 0.0f;

				if (_chipOscParamsPtr->OscWaveType->getCurrentChoiceName() == "NES_Square50%") {
					currentSample += waveForms.nesSquare(currentAngle);
				}
				else if (_chipOscParamsPtr->OscWaveType->getCurrentChoiceName() == "NES_Square25%") {
					currentSample += waveForms.nesSquare25(currentAngle);
				}
				else if (_chipOscParamsPtr->OscWaveType->getCurrentChoiceName() == "NES_Square12.5%") {
					currentSample += waveForms.nesSquare125(currentAngle);
				}
				else if (_chipOscParamsPtr->OscWaveType->getCurrentChoiceName() == "NES_Triangle") {
					currentSample += waveForms.nesTriangle(currentAngle);
				}
				else if (_chipOscParamsPtr->OscWaveType->getCurrentChoiceName() == "NES_LongNoise") {
					currentSample += waveForms.longNoise(angleDelta);
				}
				else if (_chipOscParamsPtr->OscWaveType->getCurrentChoiceName() == "NES_ShortNoise") {
					currentSample += waveForms.shortNoise(angleDelta);
				}
				else if (_chipOscParamsPtr->OscWaveType->getCurrentChoiceName() == "Pure_Sine") {
					currentSample += waveForms.sine(currentAngle);
				}
				else if (_chipOscParamsPtr->OscWaveType->getCurrentChoiceName() == "Pure_Saw") {
					currentSample += waveForms.saw(currentAngle);
				}
				else if (_chipOscParamsPtr->OscWaveType->getCurrentChoiceName() == "Pure_Triangle") {
					currentSample += waveForms.triangle(currentAngle);
				}
				else if (_chipOscParamsPtr->OscWaveType->getCurrentChoiceName() == "Pure_Square50%") {
					currentSample += waveForms.square(currentAngle);
				}
				else if (_chipOscParamsPtr->OscWaveType->getCurrentChoiceName() == "Pure_Square25%") {
					currentSample += waveForms.square25(currentAngle);
				}
				else if (_chipOscParamsPtr->OscWaveType->getCurrentChoiceName() == "Pure_Square12.5%") {
					currentSample += waveForms.square125(currentAngle);
				}
				else if (_chipOscParamsPtr->OscWaveType->getCurrentChoiceName() == "Pure_Lo-bitNoise") {
					currentSample += waveForms.lobitNoise(angleDelta);
				}
				else if (_chipOscParamsPtr->OscWaveType->getCurrentChoiceName() == "Waveform Memory") {
					currentSample += waveForms.waveformMemory(currentAngle, _waveformMemoryParamsPtr);
				}

				// OSC MIX:  // 前回のベロシティとの差異によるノイズ発生を防ぐ。
				levelDiff *= 0.9f;						
				currentSample *= level - levelDiff;

				// AMP EG: エンベロープの値をサンプルデータに反映する。
				currentSample *= ampEnv.getValue();

				// バッファに対して加算処理を行う。ポリフォニックでは、各ボイスの音を加算処理する必要がある。
				for (int channelNum = outputBuffer.getNumChannels(); --channelNum >= 0;) {
					outputBuffer.addSample(channelNum, startSample, currentSample);
				}

				// エンベロープがリリース状態の場合の処理
				if (ampEnv.isReleasing())
				{
					if (ampEnv.getValue() <= 0.005f) //エンベロープの値が十分に小さければ
					{
						ampEnv.releaseEnd();		 // エンベロープをWait状態に移行する。
						vibratoEnv.releaseEnd();
						clearCurrentNote();			 // このボイスが生成するノート情報をクリアする。
						angleDelta = 0.0f;			 // 変数を初期値に戻す。
						currentAngle = 0.0f;		 // 変数を初期値に戻す。
						pitchSweep = 0.0f;
						levelDiff = 0.0f;
						break;
					}
				}

				//============ 次のサンプルデータを生成するための準備 ============

				// LFO:モジュレーション対象がWaveAngleの時のモジュレーション処理
				// ピッチベンド:MIDIキーボードのピッチベンド入力を反映する処理
				if (_vibratoParamsPtr->VibratoEnable->get())
				{
					currentAngle += angleDelta 
						* pow(2.0f , pitchBend / 13.0f * _optionsParamsPtr->PitchBendRange->get())
						* pow(2.0f, pitchSweep) 
						* pow(2.0f, 2 * (modulationFactor / 13.0f));
				}
				else
				{
					currentAngle += angleDelta
						* pow(_optionsParamsPtr->PitchBendRange->get(), pitchBend)
						* pow(2.0f, pitchSweep);
				}

				vibratoAngle += (_vibratoParamsPtr->VibratoSpeed->get() / (float)getSampleRate()) * TWO_PI;

				if (_sweepParamsPtr->SweepSwitch->getCurrentChoiceName() == "Positive")
				{
					pitchSweep += 1 / (float)getSampleRate() / (float)_sweepParamsPtr->SweepTime->get();
				}
				else if (_sweepParamsPtr->SweepSwitch->getCurrentChoiceName() == "Negative")
				{
					pitchSweep -= 1 / (float)getSampleRate() / (float)_sweepParamsPtr->SweepTime->get();
				}

				if (fabsf(levelDiff) <= 0.005f) {
					levelDiff = 0.000f;
				}

				if (currentAngle > TWO_PI) {
					currentAngle -= TWO_PI;
				}

				if (vibratoAngle > TWO_PI) {
					vibratoAngle -= TWO_PI;
				}

				// AMP EG: エンベロープの各パラメータを最新の値に変更する。
				ampEnv.setParameters(_chipOscParamsPtr->Attack->get(), _chipOscParamsPtr->Decay->get(), _chipOscParamsPtr->Sustain->get(), _chipOscParamsPtr->Release->get());
				// AMP EG: エンベロープを1サンプル分進めておく。
				ampEnv.cycle((float)getSampleRate());

				vibratoEnv.setParameters(_vibratoParamsPtr->VibratoAttackTime->get(), 0.0f, 1.0f, 0.0f);
				vibratoEnv.cycle((float)getSampleRate());

				// 書き込み先のオーディオバッファのサンプルインデックス値をインクリメントする。
				++startSample;
			}
		}
	}
}

// ⑨モジュレーション波形を算出する関数。
float SimpleVoice::calcModulationFactor(float angle)
{
	float factor = 0.0f;
	factor = waveForms.sine(angle);

	// factorの値が0.5を中心とした0.0～1.0の値となるように調整する。
	factor *= _vibratoParamsPtr->VibratoAmount->get() / 2;
	return factor;
}
