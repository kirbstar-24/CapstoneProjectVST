#pragma once
#include <JuceHeader.h>

class Morph
{
public:
	static constexpr int fftOrder = 10; 
	static constexpr int fftSize = 1 << fftOrder;
	static constexpr int winSlide = fftSize / 4; // quarter window slide
	static constexpr int bins = fftSize / 2 + 1;

	void prep(double sampleRate)
	{
		fft = std::make_unique<juce::dsp::FFT>(fftOrder);

		win = std::make_unique<juce::dsp::WindowingFunction<float>>(
			fftSize, juce::dsp::WindowingFunction<float>::hann); //hann windowing function

		fftBuffer.fill(0.0f);
		inFifo.fill(0.0f);
		outFifo.fill(0.0f);
		target.fill(0.0f);
		wait.fill(0.0f);

		nextFFTBlockReady = false;
		fifoIndex = 0;
		slideCounter = 0;
	}

	void setTarget(juce::AudioBuffer<float>& buffer) //stages spectrum in wait
	{
		//temp buffer
		std::array<float, fftSize * 2> tempBuff {};


			//copy channel buff into temp
		int numSamples = std::min(buffer.getNumSamples(), fftSize);
		for (int i = 0; i < numSamples; ++i)
		{
			tempBuff[i] = buffer.getReadPointer(0)[i];

		}
			//window it
		win->multiplyWithWindowingTable(tempBuff.data(), fftSize);

			//fft it
		fft->performRealOnlyForwardTransform(tempBuff.data());


			//wait - magnitude
		for (int j = 0; j < bins; ++j)
		{
			float real = tempBuff[j * 2];
			float imag = tempBuff[j * 2 + 1];
			wait[j] = std::sqrt(real * real + imag * imag);
		}


		nextFFTBlockReady = true;

			
	}

	void process(juce::AudioBuffer<float>& buffer, float morphAmount)
	{
		if (nextFFTBlockReady)
		{
			target = wait;
			nextFFTBlockReady = false;
		}

		
		const int numSamples = buffer.getNumSamples();
		const int numChannels = buffer.getNumChannels();

		auto* channelData = buffer.getWritePointer(0);

		for (int i = 0; i < numSamples; ++i)
		{
			processSample(channelData[i], morphAmount);
		}

		for (int ch = 1; ch < numChannels; ++ch) // copy to all channels
		{
			buffer.copyFrom(ch, 0, buffer.getReadPointer(0), numSamples);
		}
		
	}



private:
	void processSample(float& sample, float morphAmount)
	{
		inFifo[fifoIndex] = sample; //push incoming sample

		sample = outFifo[fifoIndex]; // 

		outFifo[fifoIndex] = 0.0f; // clear 

		fifoIndex = (fifoIndex + 1) % fftSize;
		slideCounter++;

		if (slideCounter >= winSlide)
		{
			slideCounter = 0;
			processFrame(morphAmount);
		}
	}

	void processFrame(float morphAmount)
	{
		//copy current window out of the inFifo
		for (int i = 0; i < fftSize; ++i)
		{
			fftBuffer[i] = inFifo[(fifoIndex + i) % fftSize];
		}

		for (int i = fftSize; i < fftSize * 2; ++i)
		{
			fftBuffer[i] = 0.0f;
		}

		//window it
		win->multiplyWithWindowingTable(fftBuffer.data(), fftSize);

		//fft it
		fft->performRealOnlyForwardTransform(fftBuffer.data());

		for (int i = 0; i < bins; i++) // for each bin , blend magnitude aka morph
		{
			float real = fftBuffer[i * 2];
			float imag = fftBuffer[i * 2 + 1];

			float mag = std::sqrt(real * real + imag * imag);
			float phase = std::atan2(imag, real);

			float blend = mag + morphAmount * (target[i] - mag);

			fftBuffer[i * 2] = blend * std::cos(phase);
			fftBuffer[i * 2 + 1] = blend * std::sin(phase);
		}

		fft->performRealOnlyInverseTransform(fftBuffer.data());


		float scale = 160.0f / (float)fftSize;

		for (int i = 0; i < fftSize; ++i)
		{
			outFifo[(fifoIndex + i) % fftSize] += fftBuffer[i] * scale;
		}


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