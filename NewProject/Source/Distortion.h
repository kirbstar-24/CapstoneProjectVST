/*
  ==============================================================================

	This file contains the code for the distortion effect

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include <cmath>

class Distortion
{
public:
	enum class Type { SoftClip = 0, HardClip = 1, Foldback = 2 };

	void setParameters(float drive, float mix, int type)
	{
		currentDrive = drive;
		currentMix = mix;
		currentType = static_cast<Type>(type);
	}

	void process(juce::AudioBuffer<float>& buffer)
	{
		const int numSamples = buffer.getNumSamples();
		const int numChannels = buffer.getNumChannels();
		for (int ch = 0; ch < numChannels; ++ch)
		{
			auto* channelData = buffer.getWritePointer(ch);

			for (int i = 0; i < numSamples; ++i)
			{
				float dry = channelData[i];
				float wet = processSample(dry * currentDrive);

				wet *= (1.0f / std::sqrt(currentDrive)); //gain compensation

				channelData[i] = wet * currentMix + dry * (1.0f - currentMix);
			}

		}
	}


private:
	float processSample(float n)
	{
		if (currentType == Type::SoftClip)
		{
			return std::tanh(n);
		}
		else if (currentType == Type::HardClip)
		{
			return juce::jlimit(-1.0f, 1.0f, n);
		}
		else if (currentType == Type::Foldback)
		{
			while (n > 1.0f || n < -1.0f)
				n = std::abs(std::abs(n) - 2.0f) - 1.0f;
			return n;
		}

		return n;
	}

	float currentDrive = 1.0f;
	float currentMix = 0.0f;
	Type  currentType = Type::SoftClip;
};