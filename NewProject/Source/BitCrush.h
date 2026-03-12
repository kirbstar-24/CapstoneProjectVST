/*
  ==============================================================================

    This file contains the code for the bitcrush effect

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include <cmath>

class BitCrush
{
public:
    void prep(double sampleRate)
    {
        sr = sampleRate;
        lastSample = 0.0f;
        counter = 0;
    }

    void setParameters(float newBitDepth, float newDownSample)
    {
        bitDepth = newBitDepth;
        downsampleFactor = juce::jmax(1, (int)newDownSample);
        updateStepSize();
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
                if (counter == 0)
                {
                    lastSample = channelData[i];
                }

                counter = (counter + 1) % downsampleFactor;

                float crushed = lastSample;

                crushed = std::round(crushed / stepSize) * stepSize;

                channelData[i] = crushed;
            }
        }
    }

private:
    void updateStepSize()
    {
        int levels = 1 << (int)bitDepth;
        stepSize = 2.0f / levels;
    }

    double sr = 44100.0;
    float bitDepth = 8.0f;
    int downsampleFactor = 1;

    float stepSize = 0.01f;
    float lastSample = 0.0f;
    int counter = 0;
};