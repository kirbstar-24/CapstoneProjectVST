#pragma once
class SoundBank
{
public:
	enum class Wave {Sin = 0, Saw, Square, Tri};

	void prep(double sampleRate)
	{
		sr = sampleRate;
		phase = 0.0f;
		waveTable();
	}

	void setWave(Wave type)
	{
		if (type != currentWave)
		{
			currentWave = type;
			waveTable();
		}
	}

	void process(juce::AudioBuffer<float>& buffer, float frequency = 110.0f)
	{
		const int numSamples = buffer.getNumSamples();
		const int numChannels = buffer.getNumChannels();
		float phaseInc = frequency / (float)sr;

		for (int ch = 0; ch < numChannels; ++ch)
		{
			auto* channelData = buffer.getWritePointer(ch);
			float p = phase;

			for (int i = 0; i < numSamples; ++i)
			{
				float tablePos = p * (float)tableSize;
				int   idx0 = (int)tablePos % tableSize;
				int   idx1 = (idx0 + 1) % tableSize;
				float frac = tablePos - (int)tablePos;
				channelData[i] = table[idx0] + frac * (table[idx1] - table[idx0]);

				p += phaseInc;
				if (p >= 1.0f) p -= 1.0f;
			}
		}

		phase += phaseInc * numSamples;
		if (phase >= 1.0f) phase -= std::floor(phase);

	}

private:
	static constexpr int tableSize = 2048;
	float table[tableSize] = {};
	float phase = 0.0f;
	double sr = 44100.0;
	Wave currentWave = Wave::Sin;


	void waveTable()
	{
		for (int i = 0; i < tableSize; i++)
		{
			float t = (float)i / (float)tableSize;

			if (currentWave == Wave::Sin)
			{
				table[i] = std::sin(juce::MathConstants<float>::twoPi * t);
			}
			else if (currentWave == Wave::Saw)
			{
				table[i] = 2.0f * t - 1.0f;
			}
			else if (currentWave == Wave::Square)
			{
				if (t < 0.5f)
				{
					table[i] = 1.0f;
				}
				else
				{
					table[i] = -1.0f;
				}
			}
			else if (currentWave == Wave::Tri)
			{
				if (t < 0.5f)
				{
					table[i] = (4.0f * t - 1.0f);
				}
				else
				{
					table[i] = (3.0f - 4.0f * t);
				}
			}
		}
	}
};