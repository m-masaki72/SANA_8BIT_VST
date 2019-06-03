
#include "../JuceLibraryCode/JuceHeader.h"
#include <math.h>

/* 
--------------------------------------------------------------------------------
CMyFilter
http://vstcpp.wpblog.jp/?page_id=523
--------------------------------------------------------------------------------
example
// それぞれの変数は下記のとおりとする
// 　float input[]  …44100Hzでサンプリングされた入力信号の格納されたバッファ。
// 　float output[] …フィルタ処理した値を書き出す出力信号のバッファ。
// 　int   size     …入力信号・出力信号のバッファのサイズ。
*/
class CMyFilter
{
private:
	// フィルタの係数
	float a0, a1, a2, b0, b1, b2;
	// バッファ
	float out1, out2;
	float in1, in2;

public:
	inline CMyFilter();

	// 入力信号にフィルタを適用する関数
	inline float Process(float in);

	// フィルタ係数を計算するメンバー関数
	inline void LowPass(float freq, float q, float samplerate = 44100.0f);
	inline void HighPass(float freq, float q, float samplerate = 44100.0f);
	inline void BandPass(float freq, float bw, float samplerate = 44100.0f);
	inline void Notch(float freq, float bw, float samplerate = 44100.0f);
	inline void LowShelf(float freq, float q, float gain, float samplerate = 44100.0f);
	inline void HighShelf(float freq, float q, float gain, float samplerate = 44100.0f);
	inline void Peaking(float freq, float bw, float gain, float samplerate = 44100.0f);
	inline void AllPass(float freq, float q, float samplerate = 44100.0f);
};

// --------------------------------------------------------------------------------
// コンストラクタ
// --------------------------------------------------------------------------------
CMyFilter::CMyFilter()
{
	// メンバー変数を初期化
	a0 = 1.0f; // 0以外にしておかないと除算でエラーになる
	a1 = 0.0f;
	a2 = 0.0f;
	b0 = 1.0f;
	b1 = 0.0f;
	b2 = 0.0f;

	in1 = 0.0f;
	in2 = 0.0f;

	out1 = 0.0f;
	out2 = 0.0f;
};

// --------------------------------------------------------------------------------
// 入力信号にフィルタを適用する関数
// --------------------------------------------------------------------------------
float CMyFilter::Process(float in)
{
	// 入力信号にフィルタを適用し、出力信号変数に保存。
	//float out = b0 / a0 * in + b1 / a0 * in1 + b2 / a0 * in2
	//	- a1 / a0 * out1 - a2 / a0 * out2;

	float out = a0 * in + a1 * in1 + a2 * in2 + b1 * out1 + b2 * out2;

	in2 = in1; // 2つ前の入力信号を更新
	in1 = in;  // 1つ前の入力信号を更新

	out2 = out1; // 2つ前の出力信号を更新
	out1 = out;  // 1つ前の出力信号を更新

				 // 出力信号を返す
	return out;
}

// --------------------------------------------------------------------------------
// フィルタ係数を計算するメンバー関数
// --------------------------------------------------------------------------------
void CMyFilter::LowPass(float fc, float q, float fs)
{
	/*
	// フィルタ係数計算で使用する中間値を求める。
	float omega = 2.0f * 3.14159265f *  freq / samplerate;
	float alpha = sin(omega) / (2.0f * q);

	// フィルタ係数を求める。
	a0 = 1.0f + alpha;
	a1 = -2.0f * cos(omega);
	a2 = 1.0f - alpha;
	b0 = (1.0f - cos(omega)) / 2.0f;
	b1 = 1.0f - cos(omega);
	b2 = (1.0f - cos(omega)) / 2.0f;
	*/

	double pi = 3.14159265;
	double p1, p2; //計算用
	double wc;  //カットオフ角周波数
	double fc_a; //アナログLPFカットオフ周波数

	fc_a = tan(pi * fc / fs) / (2 * pi);
	wc = 2 * pi * fc_a;

	//係数計算
	p1 = 1 + wc / q + wc * wc;
	p2 = 1 - wc / q + wc * wc;
	a0 = (wc * wc) / p1;
	a1 = 2 * (wc * wc) / p1;
	a2 = a0;
	b0 = 1;
	b1 = -2 * (wc * wc - 1) / p1;
	b2 = -p2 / p1;
}

void CMyFilter::HighPass(float freq, float q, float samplerate)
{
	// フィルタ係数計算で使用する中間値を求める。
	float omega = 2.0f * 3.14159265f *  freq / samplerate;
	float alpha = sin(omega) / (2.0f * q);

	// フィルタ係数を求める。
	a0 = 1.0f + alpha;
	a1 = -2.0f * cos(omega);
	a2 = 1.0f - alpha;
	b0 = (1.0f + cos(omega)) / 2.0f;
	b1 = -(1.0f + cos(omega));
	b2 = (1.0f + cos(omega)) / 2.0f;
}

void CMyFilter::BandPass(float freq, float bw, float samplerate)
{
	// フィルタ係数計算で使用する中間値を求める。
	float omega = 2.0f * 3.14159265f *  freq / samplerate;
	float alpha = sin(omega) * sinh(log(2.0f) / 2.0 * bw * omega / sin(omega));

	// フィルタ係数を求める。
	a0 = 1.0f + alpha;
	a1 = -2.0f * cos(omega);
	a2 = 1.0f - alpha;
	b0 = alpha;
	b1 = 0.0f;
	b2 = -alpha;
}

void CMyFilter::Notch(float freq, float bw, float samplerate)
{
	// フィルタ係数計算で使用する中間値を求める。
	float omega = 2.0f * 3.14159265f *  freq / samplerate;
	float alpha = sin(omega) * sinh(log(2.0f) / 2.0 * bw * omega / sin(omega));

	// フィルタ係数を求める。
	a0 = 1.0f + alpha;
	a1 = -2.0f * cos(omega);
	a2 = 1.0f - alpha;
	b0 = 1.0f;
	b1 = -2.0f * cos(omega);
	b2 = 1.0f;
}

void CMyFilter::LowShelf(float freq, float q, float gain, float samplerate)
{
	// フィルタ係数計算で使用する中間値を求める。
	float omega = 2.0f * 3.14159265f *  freq / samplerate;
	float alpha = sin(omega) / (2.0f * q);
	float A = pow(10.0f, (gain / 40.0f));
	float beta = sqrt(A) / q;

	// フィルタ係数を求める。
	a0 = (A + 1.0f) + (A - 1.0f) * cos(omega) + beta * sin(omega);
	a1 = -2.0f * ((A - 1.0f) + (A + 1.0f) * cos(omega));
	a2 = (A + 1.0f) + (A - 1.0f) * cos(omega) - beta * sin(omega);
	b0 = A * ((A + 1.0f) - (A - 1.0f) * cos(omega) + beta * sin(omega));
	b1 = 2.0f * A * ((A - 1.0f) - (A + 1.0f) * cos(omega));
	b2 = A * ((A + 1.0f) - (A - 1.0f) * cos(omega) - beta * sin(omega));
}

void CMyFilter::HighShelf(float freq, float q, float gain, float samplerate)
{
	// フィルタ係数計算で使用する中間値を求める。
	float omega = 2.0f * 3.14159265f *  freq / samplerate;
	float alpha = sin(omega) / (2.0f * q);
	float A = pow(10.0f, (gain / 40.0f));
	float beta = sqrt(A) / q;

	// フィルタ係数を求める。
	a0 = (A + 1.0f) - (A - 1.0f) * cos(omega) + beta * sin(omega);
	a1 = 2.0f * ((A - 1.0f) - (A + 1.0f) * cos(omega));
	a2 = (A + 1.0f) - (A - 1.0f) * cos(omega) - beta * sin(omega);
	b0 = A * ((A + 1.0f) + (A - 1.0f) * cos(omega) + beta * sin(omega));
	b1 = -2.0f * A * ((A - 1.0f) + (A + 1.0f) * cos(omega));
	b2 = A * ((A + 1.0f) + (A - 1.0f) * cos(omega) - beta * sin(omega));
}

void CMyFilter::Peaking(float freq, float bw, float gain, float samplerate)
{
	// フィルタ係数計算で使用する中間値を求める。
	float omega = 2.0f * 3.14159265f *  freq / samplerate;
	float alpha = sin(omega) * sinh(log(2.0f) / 2.0 * bw * omega / sin(omega));
	float A = pow(10.0f, (gain / 40.0f));

	// フィルタ係数を求める。
	a0 = 1.0f + alpha / A;
	a1 = -2.0f * cos(omega);
	a2 = 1.0f - alpha / A;
	b0 = 1.0f + alpha * A;
	b1 = -2.0f * cos(omega);
	b2 = 1.0f - alpha * A;
}

void CMyFilter::AllPass(float freq, float q, float samplerate)
{
	// フィルタ係数計算で使用する中間値を求める。
	float omega = 2.0f * 3.14159265f *  freq / samplerate;
	float alpha = sin(omega) / (2.0f * q);

	// フィルタ係数を求める。
	a0 = 1.0f + alpha;
	a1 = -2.0f * cos(omega);
	a2 = 1.0f - alpha;
	b0 = 1.0f - alpha;
	b1 = -2.0f * cos(omega);
	b2 = 1.0f + alpha;
}

class antiAliasFilter
{
public:
	CMyFilter simpleFilter;
	std::int32_t sampleRate;
	std::int32_t upSamplingFactor;

	antiAliasFilter()
	{};

	void prepare(std::int32_t _sampleRate, std::int32_t _upsamplingFactor)
	{
		sampleRate = _sampleRate;
		upSamplingFactor = _upsamplingFactor;

		// カットオフ周波数 1000Hz、Q値 1/√2のローパスフィルタに設定
		simpleFilter.LowPass(20000.0, 1 / 4.0, sampleRate * upSamplingFactor);
	};

	void process(AudioBuffer<float> &buffer, AudioBuffer<float> &upSampleBuffer, std::int32_t totalNumInputChannels, std::int32_t totalNumOutputChannels)
	{
		auto size = buffer.getNumSamples();
		float* input = new float[size * upSamplingFactor];
		float* output = new float[size * upSamplingFactor];

		for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
		{
			for (auto j = 0; j < size * upSamplingFactor; ++j)
			{
				input[j] = upSampleBuffer.getSample(i, j);
			}
		}

		// apply LPF for anti aliasing
		for (auto i = 0; i < size * upSamplingFactor; ++i)
		{
			output[i] = simpleFilter.Process(input[i]);
		}

		//DownSampling
		for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
		{
			for (auto j = 0; j < size; ++j)
			{
				buffer.setSample(i, j, output[upSamplingFactor * j]);
			}
		}

		delete[] input;
		delete[] output;
	}

private:
};