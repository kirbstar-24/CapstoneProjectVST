/*
  ==============================================================================

    This file contains the spectrum component for the GUI
    Utilized this tutorial: https://juce.com/tutorials/tutorial_spectrum_analyser/

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "Morph.h"

class SpectrumDisplay : public juce::Component, private juce::Timer
{
public:

SpectrumDisplay()
{
	smoothed.fill(0.0f);
	startTimer(30);
}

~SpectrumDisplay() override
{
    stopTimer();
}

void sendMagnitudes(const std::array<float, Morph::bins>& mags)
{
	pending = mags;
	newDataReady.store(true);
}

void paint(juce::Graphics& g) override
{
    auto bounds = getLocalBounds().toFloat();
    auto w = bounds.getWidth();
    auto h = bounds.getHeight();

    // bckground
    g.setColour(juce::Colour(0xFF0D0D10));
    g.fillRoundedRectangle(bounds, 4.0f);

    // grid lines
    g.setColour(juce::Colour(0xFF1A1A22));
    for (int db = -60; db <= 0; db += 20)
    {
        float y = h - juce::jmap((float)db, -80.0f, 0.0f, 0.0f, h);
        g.drawHorizontalLine((int)y, 0.0f, w);
    }

    // frequency
    const float freqs[] = {100.f, 500.f, 1000.f, 5000.f, 10000.f};
    const float maxFreq = 20000.0f;
    for (auto freq : freqs)
    {
        float xNorm = std::log10(1.0f + 9.0f * freq / maxFreq);
        g.drawVerticalLine((int)(xNorm * w), 0.0f, h);
    }

    juce::Path fillPath;
    fillPath.startNewSubPath(0.0f, h);

    for (int i = 1; i < Morph::bins; ++i)
    {
        float xNorm = std::log10(1.0f + 9.0f * (float)i / (float)Morph::bins);
        float x = xNorm * w;
        float db = juce::Decibels::gainToDecibels(smoothed[i] / 512.0f, -80.0f);
        float yNorm = juce::jmap(db, -80.0f, 0.0f, 0.0f, 1.0f);
        float y = h - yNorm * h;
        fillPath.lineTo(x, juce::jlimit(0.0f, h, y));
    }
    fillPath.lineTo(w, h);
    fillPath.closeSubPath();

    juce::ColourGradient fillGrad(juce::Colour(0xFF00C87A).withAlpha(0.45f), 0.0f, 0.0f,
        juce::Colour(0xFF00C87A).withAlpha(0.0f), 0.0f, h,false);
    g.setGradientFill(fillGrad);
    g.fillPath(fillPath);

    // spctrum line
    juce::Path linePath;
    bool started = false;
    for (int i = 1; i < Morph::bins; ++i)
    {
        float xNorm = std::log10(1.0f + 9.0f * (float)i / (float)Morph::bins);
        float x = xNorm * w;
        float db = juce::Decibels::gainToDecibels(smoothed[i] / 512.0f, -80.0f);
        float yNorm = juce::jmap(db, -80.0f, 0.0f, 0.0f, 1.0f);
        float y = h - yNorm * h;

        if (!started) 
        {
            linePath.startNewSubPath(x, y); started = true;
        }
        else
        {
            linePath.lineTo(x, y);
        }
    }
    g.setColour(juce::Colour(0xFF00C87A));
    g.strokePath(linePath, juce::PathStrokeType(1.5f, juce::PathStrokeType::curved,
        juce::PathStrokeType::rounded));

    // frequency labels
    g.setFont(juce::Font(juce::Font::getDefaultMonospacedFontName(), 8.0f, juce::Font::plain));
    g.setColour(juce::Colour(0xFF2A2A38));
    const char* labels[] = { "100", "500", "1k", "5k", "10k" };
    for (int i = 0; i < 5; ++i)
    {
        float xNorm = std::log10(1.0f + 9.0f * freqs[i] / maxFreq);
        int   lx = (int)(xNorm * w);
        g.drawText(labels[i], lx - 12, (int)h - 14, 28, 12, juce::Justification::centred);
    }

    // border
    g.setColour(juce::Colour(0xFF1E1E26));
    g.drawRoundedRectangle(bounds.reduced(0.5f), 4.0f, 1.0f);
}

void resized() override {}

private:
    void timerCallback() override
    {
        if (newDataReady.exchange(false))
        {
            for (int i = 0; i < Morph::bins; ++i)
                smoothed[i] = smoothed[i] * 0.75f + pending[i] * 0.25f;
            repaint();
        }
    }

    std::array<float, Morph::bins> smoothed{};
    std::array<float, Morph::bins> pending{};
    std::atomic<bool> newDataReady{ false };

};