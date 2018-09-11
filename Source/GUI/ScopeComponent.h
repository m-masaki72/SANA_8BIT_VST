/*
  ==============================================================================

   This file is part of the JUCE tutorials.
   Copyright (c) 2017 - ROLI Ltd.

   The code included in this file is provided under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license. Permission
   To use, copy, modify, and/or distribute this software for any purpose with or
   without fee is hereby granted provided that the above copyright notice and
   this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES,
   WHETHER EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR
   PURPOSE, ARE DISCLAIMED.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

namespace {
	const Colour PANEL_COLOUR = Colours::cornsilk;
	const Colour HEADER_COLOUR = Colours::darkorange;

	const Colour FONT_COLOUR = Colours::black;

	const float PANEL_NAME_FONT_SIZE = 24.0f;
	const float PARAM_LABEL_FONT_SIZE = 16.0f;
	const int PANEL_NAME_HEIGHT = 42;
	const int LOCAL_MARGIN = 2;
}

template <typename SampleType>
class AudioBufferQueue
{
public:
	// バッファサイズが512の時, サンプリングレート96kHz下での440Hzの正弦波を2周+αまで描画することができる
	// サンプリングレート96kHz(⊿T = 0.01[ms])下での440Hzの正弦波(2.27[ms]) => 1バッファ当たり5.12[ms](= 0.01 * 512)の波形を格納できる
	// バッファサイズが512の時, サンプリングレート48kHz下での440Hzの正弦波を4周+αまで描画することができる
	// 	// サンプリングレート48kHz(⊿T = 0.02[ms])下での440Hzの正弦波(2.27[ms]) => 1バッファ当たり10.24[ms](= 0.02 * 512)の波形を格納できる
	static constexpr size_t order = 9;					// 次のバッファサイズを指定するためのバイトオーダー
	static constexpr size_t bufferSize = 1U << order;	// オーディオバッファーのコンテナ1つ当たりのサイズ(512)
	static constexpr size_t numBuffers = 5;				// オーディオバッファーのコンテナの数

	void push(const SampleType* dataToPush, size_t numSamples)
	{
		//DBG("PUSH");

		jassert(numSamples <= bufferSize);

		int start1, size1, start2, size2;

		// オーディオバッファーのコンテナ実体から書き込むべきコンテナのインデックスを特定する
		// 第一引数:コンテナを何個分書き込みたいか
		// 第二引数:コンテナのインデックス値を受け取る
		// 第三引数:読み込むコンテナの数を受け取る
		// 第四引数:0が入る
		// 第五引数:余りのコンテナ
		abstractFifo.prepareToWrite(1, start1, size1, start2, size2);

		jassert(size1 <= 1);
		jassert(size2 == 0);

		if (size1 > 0) 
		{
			// ベクターコンテナをコピーする。オーディオバッファー入力をキューに保持する。
			// 内部ではmemcpy関数が実行される
			FloatVectorOperations::copy(buffers[(size_t)start1].data(), dataToPush, (int)jmin(bufferSize, numSamples));
		}
	
		// 引数で渡された値だけ、最後尾のインデックスを移動する
		// 内部でインデックス値の循環が行われている
		abstractFifo.finishedWrite(size1);

	}

	void pop(SampleType* outputBuffer)
	{
		//DBG("POP");

		int start1, size1, start2, size2;

		// オーディオバッファーのコンテナ実体から取り出すべきコンテナのインデックスを特定する
		// 第一引数:コンテナを何個分まで読みたいか
		// 第二引数:コンテナのインデックス値を受け取る
		// 第三引数:読み込むコンテナの数を受け取る
		// 第四引数:0が入る
		// 第五引数:余りのコンテナ
		abstractFifo.prepareToRead(1, start1, size1, start2, size2);

		jassert(size1 <= 1);
		jassert(size2 == 0);

		if (size1 > 0) 
		{
			// ベクターコンテナをコピーする。キューに保持されたバッファーデータを描画用のバッファにコピーする。
			// 内部ではmemcpy関数が実行される
			FloatVectorOperations::copy(outputBuffer, buffers[(size_t)start1].data(), (int)bufferSize);
		}

		// 引数で渡された値だけ、先頭のインデックスを移動する
		// 内部でインデックス値の循環が行われている
		abstractFifo.finishedRead(size1);

	}

private:
	// オーディオバッファー。サンプルデータを保持するコンテナをさらにコンテナ化したもの。
	std::array <std::array<SampleType, bufferSize>, numBuffers> buffers;

	// 複数のコンテナ間でFIFO形式の配列操作を行うクラス。コンストラクタの引数にFIFO配列のサイズ数を指定する。
	// 抽象的なFIFO操作を行うための情報を持つクラス。内部にデータは持たないが、FIFO操作によるアドレスの値を特定するための代理人みたいなもの。
	AbstractFifo abstractFifo{ numBuffers };

};

// オーディオバッファからサンプルデータを回収してAudioBufferQueueオブジェクトにサンプルデータを追加するクラス
template<typename SampleType>
class ScopeDataCollector
{
public:
	ScopeDataCollector(AudioBufferQueue<SampleType>& queueToUse)
		: audioBufferQueue(queueToUse)
	{}

	void process(const SampleType* data, size_t numSamples)
	{
		size_t index = 0;

		// トリガーレベルを待機する処理
		if (currentState == State::WaitingForTrigger)
		{
			while (index++ < numSamples)
			{
				auto currentSample = *data++;

				// 波形表示の左端（X=0）においてY=±0となるように調整する処理
				// この処理により、波形が固定されているように見える。
				// 所定のレベル以上の値であればサンプリングするトリガーがオンになり、回収状態に移行する。
				// オシロスコープでいうトリガーモードに類似
				// 第一条件：現在のサンプルが閾値を超えていること
				// 第二条件：前回のサンプルが閾値以下であること
				// 上記2つの条件により、前回と今回のサンプルの変化により閾値を交差するということがif文の条件となる。
				if (currentSample >= triggerLevel && prevSample < triggerLevel)
				{
					numCollected = 0;
					currentState = State::Collecting;
					break;
				}

				prevSample = currentSample;
			}
		}

		// 回収状態であるときの処理
		if (currentState == State::Collecting)
		{
			while (index++ < numSamples)
			{
				buffer[numCollected++] = *data++;

				// バッファキュー1つ分のサンプル数だけコレクションしたら、キューにプッシュする
				if (numCollected == buffer.size())
				{
					audioBufferQueue.push(buffer.data(), buffer.size());
					currentState = State::WaitingForTrigger;
					prevSample = SampleType(100);
					break;
				}
			}
		}
	}

private:
	// オーディオバッファコレクターの状態リスト
	enum class State
	{
		WaitingForTrigger,
		Collecting
	};

	// トリガーが実行される閾値。本実装では0.001の閾値を超えた場合をトリガーとして認識する
	static constexpr auto triggerLevel = SampleType(0.001);

	// オーディオバッファーのコンテナ群を保持するオブジェクト
	AudioBufferQueue<SampleType>& audioBufferQueue;

	// オーディオバッファのコンテナ単体オブジェクト
	std::array<SampleType, AudioBufferQueue<SampleType>::bufferSize> buffer; //std::array<float, 512> buffer;

	// バッファにセットしたサンプルの数を監視するための変数
	size_t numCollected;

	// 前回のサンプルデータの値を保持する変数
	SampleType prevSample = SampleType(100);


	// 現在の動作モードを保持する変数
	State currentState{ State::WaitingForTrigger };
};

// ①AudioBufferQueueオブジェクトに貯められたサンプルデータをプロットして描画するGUIコンポーネントのクラス。
template<typename SampleType>
class ScopeComponent : public juce::Component, private juce::Timer
{
public:
	using Queue = AudioBufferQueue<SampleType>;

	// ②引数付きコンストラクタ。AudioBufferQueueクラスの参照を引数として受け取ってクラス内変数に代入する。
	ScopeComponent(Queue& queueuToUse)
		: audioBufferQueue(queueuToUse)
	{
		sampleData.fill(SampleType(0));
		setFramePerSecond(30);
	}

	// ③波形のプロットを更新する時間間隔を設定する関数
	void setFramePerSecond(int framePerSecond)
	{
		jassert(framePerSecond > 0 && framePerSecond < 1000);
		startTimerHz(framePerSecond);
	}

	// ④SCOPEパネルの状態を描画する関数。パネルの領域を塗りつぶす処理と波形をプロットする処理を実行する。
	void paint(Graphics& g) override
	{
		int panelNameHeight = 42;
		int localMargin = 2;
		Font panelNameFont = Font(24.0f, Font::plain).withTypefaceStyle("Italic");

		{
			int x = 0.0f, y = 0.0f, width = getWidth(), height = getHeight();
			g.setColour(PANEL_COLOUR);
			g.fillRoundedRectangle(x, y, width, height, 10.0f);
		}

		{
			float x = 0.0f, y = 0.0f, width = (float)getWidth(), height = PANEL_NAME_HEIGHT;
			g.setColour(HEADER_COLOUR);
			g.fillRoundedRectangle(x, y, width, height, 10.0f);
		}

		{
			Rectangle<int> bounds = getLocalBounds(); // コンポーネント基準の値
			String text("SCOPE");
			Colour fillColour = Colours::white;
			g.setColour(fillColour);
			g.setFont(panelNameFont);
			g.drawText(text, bounds.removeFromTop(panelNameHeight).reduced(localMargin), Justification::centred, true);
		}

		// 波形を描画する矩形領域を特定する
		Rectangle<int> drawArea = getLocalBounds();
		drawArea.removeFromTop(panelNameHeight);
		drawArea.reduce(drawArea.getWidth()* 0.05f, drawArea.getHeight()* 0.1f);

		// 波形を描画する矩形領域の背景を灰色に塗りつぶす
		g.setColour(juce::Colours::darkgrey);
		g.fillRect(drawArea);

		// 波形をプロットする領域をRectangle<SampleType>型に代入する
		SampleType drawX = (SampleType)drawArea.getX();
		SampleType drawY = (SampleType)drawArea.getY();
		SampleType drawH = (SampleType)drawArea.getHeight();
		SampleType drawW = (SampleType)drawArea.getWidth();
		Rectangle<SampleType> scopeRect = Rectangle<SampleType>{ drawX, drawY, drawW, drawH };

		// 波形をプロットする
		g.setColour(juce::Colours::cyan);
		plot(sampleData.data(), sampleData.size(), g, scopeRect, SampleType(2.0), scopeRect.getHeight() / 2);

	}

	void resized() override {}

private:
	// ⑤juce::Timerクラスのスレッドから呼び出されるコールバック関数。
	void timerCallback() override
	{
		audioBufferQueue.pop(sampleData.data());
		repaint();
	}

	// ⑥サンプルデータの配列から折れ線グラフをプロットする
	static void plot(const SampleType* data
		, size_t numSamples
		, Graphics& g
		, juce::Rectangle<SampleType> rect
		, SampleType scaler = SampleType(1)
		, SampleType offset = SampleType(0))
	{
		auto w = rect.getWidth();
		auto h = rect.getHeight();
		auto right = rect.getRight();
		auto alignedCentre = rect.getBottom() - offset;
		auto gain = h * scaler;

		for (size_t i = 1; i < numSamples; ++i)
		{
			const float x1 = jmap(SampleType(i - 1), SampleType(0), SampleType(numSamples - 1), SampleType(right - w), SampleType(right));
			const float y1 = alignedCentre - gain * data[i - 1];
			const float x2 = jmap(SampleType(i), SampleType(0), SampleType(numSamples - 1), SampleType(right - w), SampleType(right));
			const float y2 = alignedCentre - gain * data[i];
			const float t = 1.0f;
			g.drawLine(x1, y1, x2, y2, t);
		}
	}

	// ⑦クラス変数を宣言する。
	Queue& audioBufferQueue;										// AudioBufferQueueクラスの参照を保持する変数
	std::array<SampleType, Queue::bufferSize> sampleData;		// プロットするサンプルデータを格納する配列コンテナ
};
