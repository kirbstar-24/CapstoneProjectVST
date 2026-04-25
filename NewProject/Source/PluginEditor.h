/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Style.h"

//==============================================================================
/**
*/
class NewProjectAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    NewProjectAudioProcessorEditor (NewProjectAudioProcessor&);
    ~NewProjectAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    // ===== Bit slider =====
    juce::Slider bitDepthSlider;
    juce::Label bitDepthLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        bitDepthAttachment;

    // ===== Dist Slider =====
    juce::Slider    distDriveSlider;
    juce::Label     distDriveLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> distDriveAttachment;

    juce::Slider    distMixSlider;
    juce::Label     distMixLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> distMixAttachment;

    juce::ComboBox  distTypeBox;
    juce::Label     distTypeLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> distTypeAttachment;

    // === Morph ===
    juce::Slider    morphAmountSlider;
    juce::Label     morphAmountLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> morphAmountAttachment;

    juce::Slider    morphFreqSlider;
    juce::Label     morphFreqLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> morphFreqAttachment;

    juce::ComboBox  morphSourceBox;
    juce::Label     morphSourceLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> morphSourceAttachment;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    NewProjectAudioProcessor& audioProcessor;

    Style style;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NewProjectAudioProcessorEditor)
};
