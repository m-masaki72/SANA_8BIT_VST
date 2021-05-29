#include <math.h>
#include "../JuceLibraryCode/JuceHeader.h"

/*
--------------------------------------------------------------------------------
CMyFilter
http://vstcpp.wpblog.jp/?page_id=523
--------------------------------------------------------------------------------
*/
class CMyFilter {
 private:
  float a0, a1, a2, b0, b1, b2;
  float out1, out2;
  float in1, in2;

  static const int32_t MAX_SIZE = 960;

 public:
  inline CMyFilter();

  inline float Process(float in);

  inline void LowPass(float freq, float q, float samplerate = 44100.0f);
  inline void HighPass(float freq, float q, float samplerate = 44100.0f);
  inline void BandPass(float freq, float bw, float samplerate = 44100.0f);
  inline void Notch(float freq, float bw, float samplerate = 44100.0f);
  inline void LowShelf(float freq, float q, float gain,
                       float samplerate = 44100.0f);
  inline void HighShelf(float freq, float q, float gain,
                        float samplerate = 44100.0f);
  inline void Peaking(float freq, float bw, float gain,
                      float samplerate = 44100.0f);
  inline void AllPass(float freq, float q, float samplerate = 44100.0f);
};

CMyFilter::CMyFilter() {
  a0 = 1.0f;
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

float CMyFilter::Process(float in) {
  float out = a0 * in + a1 * in1 + a2 * in2 + b1 * out1 + b2 * out2;

  in2 = in1;
  in1 = in;

  out2 = out1;
  out1 = out;

  return out;
}

void CMyFilter::LowPass(float fc, float q, float fs) {
  float pi = 3.14159265f;
  float p1, p2;
  float wc;
  float fc_a;

  fc_a = tan(pi * fc / fs) / (2 * pi);
  wc = 2 * pi * fc_a;

  p1 = 1 + wc / q + wc * wc;
  p2 = 1 - wc / q + wc * wc;
  a0 = (wc * wc) / p1;
  a1 = 2 * (wc * wc) / p1;
  a2 = a0;
  b0 = 1;
  b1 = -2 * (wc * wc - 1) / p1;
  b2 = -p2 / p1;
}

void CMyFilter::HighPass(float freq, float q, float samplerate) {
  float omega = 2.0f * 3.14159265f * freq / samplerate;
  float alpha = sin(omega) / (2.0f * q);

  a0 = 1.0f + alpha;
  a1 = -2.0f * cos(omega);
  a2 = 1.0f - alpha;
  b0 = (1.0f + cos(omega)) / 2.0f;
  b1 = -(1.0f + cos(omega));
  b2 = (1.0f + cos(omega)) / 2.0f;
}

void CMyFilter::BandPass(float freq, float bw, float samplerate) {
  float omega = 2.0f * 3.14159265f * freq / samplerate;
  float alpha = sin(omega) * sinh(log(2.0f) / 2.0f * bw * omega / sin(omega));

  a0 = 1.0f + alpha;
  a1 = -2.0f * cos(omega);
  a2 = 1.0f - alpha;
  b0 = alpha;
  b1 = 0.0f;
  b2 = -alpha;
}

void CMyFilter::Notch(float freq, float bw, float samplerate) {
  float omega = 2.0f * 3.14159265f * freq / samplerate;
  float alpha = sin(omega) * sinh(log(2.0f) / 2.0f * bw * omega / sin(omega));

  a0 = 1.0f + alpha;
  a1 = -2.0f * cos(omega);
  a2 = 1.0f - alpha;
  b0 = 1.0f;
  b1 = -2.0f * cos(omega);
  b2 = 1.0f;
}

void CMyFilter::LowShelf(float freq, float q, float gain, float samplerate) {
  float omega = 2.0f * 3.14159265f * freq / samplerate;
  float alpha = sin(omega) / (2.0f * q);
  float A = pow(10.0f, (gain / 40.0f));
  float beta = sqrt(A) / q;

  a0 = (A + 1.0f) + (A - 1.0f) * cos(omega) + beta * sin(omega);
  a1 = -2.0f * ((A - 1.0f) + (A + 1.0f) * cos(omega));
  a2 = (A + 1.0f) + (A - 1.0f) * cos(omega) - beta * sin(omega);
  b0 = A * ((A + 1.0f) - (A - 1.0f) * cos(omega) + beta * sin(omega));
  b1 = 2.0f * A * ((A - 1.0f) - (A + 1.0f) * cos(omega));
  b2 = A * ((A + 1.0f) - (A - 1.0f) * cos(omega) - beta * sin(omega));
}

void CMyFilter::HighShelf(float freq, float q, float gain, float samplerate) {
  float omega = 2.0f * 3.14159265f * freq / samplerate;
  float alpha = sin(omega) / (2.0f * q);
  float A = pow(10.0f, (gain / 40.0f));
  float beta = sqrt(A) / q;

  a0 = (A + 1.0f) - (A - 1.0f) * cos(omega) + beta * sin(omega);
  a1 = 2.0f * ((A - 1.0f) - (A + 1.0f) * cos(omega));
  a2 = (A + 1.0f) - (A - 1.0f) * cos(omega) - beta * sin(omega);
  b0 = A * ((A + 1.0f) + (A - 1.0f) * cos(omega) + beta * sin(omega));
  b1 = -2.0f * A * ((A - 1.0f) + (A + 1.0f) * cos(omega));
  b2 = A * ((A + 1.0f) + (A - 1.0f) * cos(omega) - beta * sin(omega));
}

void CMyFilter::Peaking(float freq, float bw, float gain, float samplerate) {
  float omega = 2.0f * 3.14159265f * freq / samplerate;
  float alpha = sin(omega) * sinh(log(2.0f) / 2.0f * bw * omega / sin(omega));
  float A = pow(10.0f, (gain / 40.0f));

  a0 = 1.0f + alpha / A;
  a1 = -2.0f * cos(omega);
  a2 = 1.0f - alpha / A;
  b0 = 1.0f + alpha * A;
  b1 = -2.0f * cos(omega);
  b2 = 1.0f - alpha * A;
}

void CMyFilter::AllPass(float freq, float q, float samplerate) {
  float omega = 2.0f * 3.14159265f * freq / samplerate;
  float alpha = sin(omega) / (2.0f * q);

  a0 = 1.0f + alpha;
  a1 = -2.0f * cos(omega);
  a2 = 1.0f - alpha;
  b0 = 1.0f - alpha;
  b1 = -2.0f * cos(omega);
  b2 = 1.0f + alpha;
}

class antiAliasFilter {
 public:
  CMyFilter simpleFilter;
  std::int32_t sampleRate;
  std::int32_t upSamplingFactor;

  antiAliasFilter(){};

  void prepare(std::int32_t _sampleRate, std::int32_t _upsamplingFactor) {
    sampleRate = _sampleRate;
    upSamplingFactor = _upsamplingFactor;

    simpleFilter.LowPass(20000.0f, 1 / 4.0f, float(sampleRate * upSamplingFactor));
  };

  void process(AudioBuffer<float> &buffer, AudioBuffer<float> &upSampleBuffer,
               std::int32_t totalNumInputChannels,
               std::int32_t totalNumOutputChannels) {
    auto size = buffer.getNumSamples();
    auto upSize = buffer.getNumSamples() * upSamplingFactor;
    std::vector<float> input(upSize);
    std::vector<float> output(upSize);

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i) {
      for (auto j = 0; j < upSize; ++j) {
        input[j] = upSampleBuffer.getSample(i, j);
      }
    }

    // apply LPF for anti aliasing
    for (auto i = 0; i < upSize; ++i) {
      output[i] = simpleFilter.Process(input[i]);
    }

    // DownSampling
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i) {
      for (auto j = 0; j < size; ++j) {
        buffer.setSample(i, j, output[upSamplingFactor * j]);
      }
    }
  }

 private:
};