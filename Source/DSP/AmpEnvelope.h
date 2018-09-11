/*
  ==============================================================================

    AmpEnvelope.h
    Created: 20 Apr 2018 2:08:26am
    Author:  COx2

  ==============================================================================
*/

#pragma once

// ①クラス名宣言
class AmpEnvelope 
{
public:
	// ②enum型を利用してアンプ・エンベロープ・ジェネレータの状態遷移を表現する。
	enum class AMPENV_STATE
	{
		ATTACK = 0,
		DECAY,
		SUSTAIN,
		RELEASE,
		WAIT,
	};

	// ③引数付きコンストラクタ。引数として各パラメータの初期値を渡す。
	AmpEnvelope(float attackTime, float decayTime, float sustain, float releaseTime);
	~AmpEnvelope();

	// ④パラメータの状態を取得する関数、状態を変更する関数を宣言する。
	AMPENV_STATE getState();
	float getValue();
	void setParameters(float attackTime, float decayTime, float sustain, float releaseTime);
	void attackStart();
	void releaseStart();
	void releaseEnd();
	bool isHolding();
	bool isReleasing();
	void cycle(float sampleRate);

private:
	// 引数無しコンストラクタをprivate領域で宣言することで、クラス外から引数無しコンストラクタを呼べないようにする。
	AmpEnvelope();

	// ⑤クラス内変数を宣言する
	AMPENV_STATE _ampState;											// エンベロープの状態を保持する変数
	float _attackTime, _decayTime, _sustainValue, _releaseTime;		// エンベロープのパラメータを保持する変数
	float _value, _valueOnReleaseStart;								// エンベロープの値を保持する変数
	float _sampleRate;												// シンセサイザーのサンプリングレートを保持する変数
};