#include <vector>

//エコーエフェクト用のサンプリングバッファ
class EchoBuffer
{
public:
	EchoBuffer(int freq, float sec, int count)
	{
		sampleRate = freq;
		echoTime = sec;
		echoCount = count;
		init();
	};

	~EchoBuffer()
	{};

	void init() {

		index = 0;
		bufSize = (int)(sampleRate * echoTime);

		if (bufSize <= 0)
		{
			bufSize = 0;
		}

		buf.resize(echoCount);

		for (int i = 0; i < echoCount; ++i)
		{
			buf[i].resize(bufSize);
		}

		for (int i = 0; i < echoCount; ++i)
		{
			for (int j = 0; j < bufSize; ++j)
			{
				buf[i][j] = 0.0f;
			}
		}
	}

	void addSample(float val, float amp)
	{
		if (index >= bufSize)
		{
			init();
		}
		//一つずつ前のバッファのサンプルを補完，1サンプル前のバッファを補完していく
		for (int i = echoCount - 1; i > 0; --i)
		{
			buf[i][index] = buf[i - 1][index] * amp;
		}
		//現在のサンプルを補完
		buf[0][index] = val * amp;
	};

	// バッファの最後尾のサンプルを返す．
	// 最後尾はbuffSizeだけ遅れたサンプルになるのでDelayされる
	float getSample(int repeatCount)
	{
		if (repeatCount >= echoCount)
		{
			init();
			return 0.0f;
		}

		if (index >= bufSize)
		{
			init();
			return 0.0f;
		}

		return buf[repeatCount][index];
	};

	void updateParam(float sec, int count)
	{
		if (echoTime != sec || echoCount != count)
		{
			echoTime = sec;
			echoCount = count;
			init();
		}
	}

	void cycle()
	{
		index += 1;
		if (index >= bufSize)
		{
			index = 0;
		}
	}

private:
	std::vector<std::vector<float>> buf;
	int sampleRate;
	int echoCount;
	float echoTime;

	int bufSize;
	int index;
};
