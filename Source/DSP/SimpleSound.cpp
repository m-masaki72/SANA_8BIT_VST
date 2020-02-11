/*
  ==============================================================================

    SimpleSound.cpp
    Created: 20 Apr 2018 2:07:49am
    Author:  COx2

  ==============================================================================
*/

// ヘッダファイルをインクルードする
#include "SimpleSound.h"

// ①引数付きコンストラクタ。初期化指定子でクラス内変数に初期値を代入する。
//   BigIntegerクラスのコンストラクタにBigInteger型の参照を渡すと、参照先の値をインスタンス側にコピーする処理が行われる。
SimpleSound::SimpleSound(const BigInteger& notes, const BigInteger& channels)
	: midiNotes(notes), midiChannels(channels)
{
}

// デストラクタ
SimpleSound::~SimpleSound()
{
}

// ②トリガー可能なMIDIノート番号かどうかを判定する。
//   クラス内変数midiNotesが保持するビットフラグに対して、引数midiNoteNumberの値に該当するビットが立っているかどうかを返す。
bool SimpleSound::appliesToNote(int midiNoteNumber)
{
	DBG(juce::String("midiNoteNumber: ") + juce::String(midiNoteNumber));

	// BigIntegerクラスのオペレータ[]を実行するとビットフラグが立っているかどうかを確認することができる。
	return midiNotes[midiNoteNumber];
}

// ③トリガー可能なMIDIチャンネル番号かどうかを判定する。
//   クラス内変数midiChannelsが保持するビットフラグに対して、引数midiChannelの値に該当するビットが立っているかどうかを返す。
bool SimpleSound::appliesToChannel(int midiChannel)
{
	DBG(juce::String("midiChannel: ") + juce::String(midiChannel));

	return midiChannels[midiChannel];
}
