#pragma once
#include <JuceHeader.h>
#include <cmath>

class Distortion
{
public:
	enum class Type { SoftClip = 0, HardClip = 1, Foldback = 2 };

	void prep(float drive, float mix, int type)
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

				channelData[i] = wet * currentMix + dry * (1.0f - currentMix)
			}

		}
	}


private:
	float processSample(float n)
	{
		if (currentType == Type::SoftClip)
		{
			return std::tanh(x);
		}
		else if (currentType == Type::HardClip)
		{
			return juce::jlimit(-1.0, 1.0f, x);
		}
		else if (currentType == Type::Foldback)
		{
			while (x > 1.0f || x < < -1.0f)
				x = std::abs(std::abs(x) - 2.0f) - 1.0f;
			return n
		}

		return n;
	}

	float currentDrive = 1.0f;
	float currentMix = 0.0f;
	Type  currentType = Type::SoftClip;
};