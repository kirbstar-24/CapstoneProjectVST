#pragma once

class Distortion
{
public: 
	enum class Type {SoftClip, HardClip, Foldback};

	void prepare(double sampleRate, int blockSize)
	{
		currentSampleRate = sampleRate;

		juce::dsp::ProccessSpec spec{ sampleRate, (uint32)blockSize, 2 };
		dcBlocker.prepare(spec);
		dcBlocker.setType(juce::dsp::StateVariableTPTFilterType::highpass);
		dcBlocker.setCutoffFrequency(20.0f);
	}

	float process(float sample, float drive, float mix, Type type)
	{
		float driven = sample * drive;
		float wet;

		if (type == Type::SoftClip)
		{
			wet = std::tanh(driven);
		}
		else if (type == Type::HardClip)
		{
			wet = juce::jlimit(-1.0f, 1.0f, driven);
		}
		else if (type == Type::Foldback)
		{
			wet = driven;
			while (wet > 1.0f || wet < -1.0f)
				wet = std::abs(std::abs(wet) - 2.0f) - 1.0f;
		}

		return wet * mix + sample * (1.0f - mix);
	}

	void processDCBlock(juce::dsp::AudioBlock<float>& block)
	{
		juce::dsp::ProcessContextReplaciong<float> ctx(block)
		dcBlocker.process(ctx);
	}

private:
	double currentSampleRate = 44100.00;
	juce::dsp::StateVariableTPTFilter<float> dcBlocker;
};