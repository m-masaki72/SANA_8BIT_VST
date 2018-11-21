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
SimpleVoice::SimpleVoice(ChipOscillatorParameters* chipOscParams, SweepParameters* sweepParams, VibratoParameters* vibratoParams, VoicingParameters* voicingParams, OptionsParameters* optionsParams, MidiEchoParameters* midiEchoParams, WaveformMemoryParameters* waveformMemoryParams)
	: _chipOscParamsPtr(chipOscParams)
	, _sweepParamsPtr(sweepParams)
	, _vibratoParamsPtr(vibratoParams)
	, _voicingParamsPtr(voicingParams)
	, _optionsParamsPtr(optionsParams)
	, _midiEchoParamsPtr(midiEchoParams)
	, _waveformMemoryParamsPtr(waveformMemoryParams)
	, ampEnv(chipOscParams->Attack->get(), chipOscParams->Decay->get(), chipOscParams->Sustain->get(), chipOscParams->Release->get(), midiEchoParams->EchoDuration->get() * midiEchoParams->EchoRepeat->get())
	, vibratoEnv(vibratoParams->VibratoAttackTime->get(), 0.1f, 1.0f, 0.1f, 0.0f)
	, portaEnv(voicingParams->PortaTime->get(), 0.0f, 1.0f, 0.0f, 0.0f)
	, currentAngle(0.0f), vibratoAngle(0.0f), angleDelta(0.0f), portaAngleDelta(0.0f)
	, level(0.0f)
	, pitchBend(0.0f), pitchSweep(0.0f)
	, eb(getSampleRate(), midiEchoParams->EchoDuration->get())
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
			eb.init();
			eb.changeDeleyTime(_midiEchoParamsPtr->EchoDuration->get());
		}
		// ベロシティ有効/無効のフラグに応じて音量レベルを決定する。有効...ベロシティの値から算出する。 無効...固定値を使用する。
		if (_optionsParamsPtr->IsVelocitySense->get())
		{
			if (velocity <= 0.01f) {
				velocity = 0.01f;
			}
			level = velocity * 1.0f;
		}
		else
		{
			level = 0.8f;
		}

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

// ⑤Synthesiserクラスから呼び出されるノートOFFのトリガー。
//   引数で渡される値に対応して波形生成を終了する前準備を行う。
//   キーリリースだとallowTailOff == true、キーリリース直後のボイススチールではallowTailOff == false
void SimpleVoice::stopNote(float velocity, bool allowTailOff)
{
	DBG("stopNote : " + juce::String((int)allowTailOff));

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
		else {
			portaAngleDelta = 0.0f;
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
				modulationFactor *= vibratoEnv.getValue() * vibratoEnv.getValue();

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

				// AMP EG: エンベロープの値をサンプルデータに反映する。
				currentSample *= ampEnv.getValue();

				rb.push_back(currentSample);


				//エコー処理とエコーレンダリング
				if (_midiEchoParamsPtr->IsEchoEnable->get())
				{
					eb.addSample(currentSample, _midiEchoParamsPtr->VolumeOffset->get() / 100.0f);

					for (int channelNum = outputBuffer.getNumChannels(); --channelNum >= 0;)
					{
						outputBuffer.addSample(channelNum, startSample, eb.getSample(0));
						outputBuffer.addSample(channelNum, startSample, eb.getSample(1));
					}
				}
				eb.countUp();

				//	リアルタイム音処理  バッファに対して加算処理を行う。ポリフォニックでは、各ボイスの音を加算処理する必要がある。
				for (int channelNum = outputBuffer.getNumChannels(); --channelNum >= 0;) 
				{
					outputBuffer.addSample(channelNum, startSample, rb.getCurrentValue());
				}

				// VOiceの初期化処理
				// エンベロープにおいて，エフェクトエコーが終わっている or リリース状態のとき				
				if (ampEnv.isEchoEnded() || (ampEnv.isReleasing() && !_midiEchoParamsPtr->IsEchoEnable->get()))
				{
					{
						ampEnv.releaseEnd();		 // エンベロープをWait状態に移行する。
						clearCurrentNote();			 // このボイスが生成するノート情報をクリアする。
						angleDelta = 0.0f;			 // 変数を初期値に戻す。
						portaAngleDelta = 0.0f;
						currentAngle = 0.0f;
						pitchSweep = 0.0f;
						break;
					}
				}
				
				//============ 次のサンプルデータを生成するための準備 ============

				//ピッチ処理
				//モジュレーションとピッチベンド，ピッチスイープにより変化
				if (_vibratoParamsPtr->VibratoEnable->get())
				{
					currentAngle += angleDelta
						* pow(2.0f, pitchBend / 13.0f * _optionsParamsPtr->PitchBendRange->get())
						* pow(2.0f, pitchSweep)
						* pow(2.0f, modulationFactor / 13.0f);
				}
				else
				{
					currentAngle += angleDelta
						* pow(2.0f, pitchBend / 13.0f * _optionsParamsPtr->PitchBendRange->get())
						* pow(2.0f, pitchSweep);
				}

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

				if (currentAngle > TWO_PI) {
					currentAngle -= TWO_PI;
				}
				if (vibratoAngle > TWO_PI) {
					vibratoAngle -= TWO_PI;
				}

				// エンベロープパラメータを更新して時間分進める
				ampEnv.setParameters(_chipOscParamsPtr->Attack->get(), _chipOscParamsPtr->Decay->get(), _chipOscParamsPtr->Sustain->get(), _chipOscParamsPtr->Release->get(), _midiEchoParamsPtr->EchoDuration->get() * _midiEchoParamsPtr->EchoRepeat->get());
				ampEnv.cycle((float)getSampleRate());
				vibratoEnv.setParameters(_vibratoParamsPtr->VibratoAttackTime->get(), 0.1f, 1.0f, 0.1f, 0.0f);
				vibratoEnv.cycle((float)getSampleRate());
				portaEnv.setParameters(_voicingParamsPtr->PortaTime->get(), 0.0f, 1.0f, 0.0f, 0.0f);
				portaEnv.cycle((float)getSampleRate());

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
	factor *= _vibratoParamsPtr->VibratoAmount->get();
	return factor;
}
