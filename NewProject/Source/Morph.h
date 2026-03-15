#pragma once
#include <JuceHeader.h>

class Morph
{
public:
	static constexpr fftOrder = 10; 
	static constexpr fftSize = 1 << fftOrder;
	static constexpr winSlide = fftSize / 4; // quarter window slide
	static constexpr bins = fftSize / 2 + 1

	void prep(double sampleRate)
	{
		fft = std::make_unique<juce::dsp::FFT>(fftOrder);
		win = std::make_unique<juce::dsp::WindowingFunction<float>>(
			juce::dsp::WindowingFunction<float>::hann); //hann windowing function

		fftBuffer.fill(0.0f);
		inFifo.fill(0.0f);
		outFifo.fill(0.0f);
		target.fill(0.0f);
		wait.fill(0.0f);

		nextFFTBlockReady = false;
		fifoIndex = 0;
		slideCounter = 0;
	}

	void setTarget()
	{

	}

	void process()
	{

	}



private:
	void processWindow()
	{

	}

	std::unique_ptr<juce::dsp::FFT> fft;
	std::unique_ptr <juce::dsp::WindowingFunction<float>> win;

	std::array<float, fftSize * 2> fftBuffer;
	std::array<float, fftSize> inFifo;
	std::array<float, fftSize * 2> outFifo;
	std::array<float, bins> target;
	std::array<float, bins> wait;

	bool nextFFTBlockReady = false;
	int fifoIndex = 0;
	int slideCounter = 0;
};