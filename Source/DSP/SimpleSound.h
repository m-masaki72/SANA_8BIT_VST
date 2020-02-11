/*
  ==============================================================================

    SimpleSound.h
    Created: 20 Apr 2018 2:07:49am
    Author:  COx2

  ==============================================================================
*/

#pragma once

// ①JUCEライブラリのヘッダをインクルードする。
#include "../JuceLibraryCode/JuceHeader.h"

// ②クラス宣言。SynthesiserSoundクラスを継承する。
class SimpleSound : public SynthesiserSound
{
public:
	// ③引数付きコンストラクタ
	SimpleSound(const BigInteger& notes, const BigInteger& channels);

	// デストラクタ
	virtual ~SimpleSound();

	// ④純粋仮想関数をオーバーライド宣言する。
	virtual bool appliesToNote(int midiNoteNumber) override;
	virtual bool appliesToChannel(int midiChannel) override;

private:
	// ⑤トリガー可能なMIDIノート番号の一覧とMIDIチャンネル番号の一覧を表現するビットフラグを保持する変数を宣言する。
	BigInteger midiNotes;
	BigInteger midiChannels;

};