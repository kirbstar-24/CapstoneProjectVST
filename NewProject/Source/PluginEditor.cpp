/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
NewProjectAudioProcessorEditor::NewProjectAudioProcessorEditor (NewProjectAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (800, 400);
    // ===== Bit Slider ======
    bitDepthSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    bitDepthSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    bitDepthSlider.setRange(1.0, 16.0, 1.0);
    addAndMakeVisible(bitDepthSlider);
    bitDepthLabel.setText("Bit Depth", juce::dontSendNotification);
    bitDepthLabel.attachToComponent(&bitDepthSlider, false);
    addAndMakeVisible(bitDepthLabel);

    bitDepthAttachment = std::make_unique<
        juce::AudioProcessorValueTreeState::SliderAttachment>(
            audioProcessor.apvts,
            "bitDepth",
            bitDepthSlider);

    // ==== Dist ====
    distDriveSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    distDriveSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    distDriveSlider.setRange(1.0, 20.0, 0.01);
    addAndMakeVisible(distDriveSlider);
    distDriveLabel.setText("Drive", juce::dontSendNotification);
    distDriveLabel.attachToComponent(&distDriveSlider, false);
    addAndMakeVisible(distDriveLabel);
    distDriveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "distDrive", distDriveSlider);
    // === Dist Mix ====
    distMixSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    distMixSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    distMixSlider.setRange(0.0, 1.0, 0.01);
    addAndMakeVisible(distMixSlider);
    distMixLabel.setText("Dist Mix", juce::dontSendNotification);
    distMixLabel.attachToComponent(&distMixSlider, false);
    addAndMakeVisible(distMixLabel);
    distMixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "distMix", distMixSlider);
    // ==== Dist Clip =====
    distTypeBox.addItem("Soft Clip", 1);
    distTypeBox.addItem("Hard Clip", 2);
    distTypeBox.addItem("Foldback", 3);
    distTypeBox.setSelectedId(1, juce::dontSendNotification);
    addAndMakeVisible(distTypeBox);
    distTypeLabel.setText("Clip Type", juce::dontSendNotification);
    distTypeLabel.attachToComponent(&distTypeBox, false);
    addAndMakeVisible(distTypeLabel);
    distTypeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        audioProcessor.apvts, "distType", distTypeBox);

    // === Morph Amount ===
    morphAmountSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    morphAmountSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(morphAmountSlider);
    morphAmountLabel.setText("Morph", juce::dontSendNotification);
    morphAmountLabel.attachToComponent(&morphAmountSlider, false);
    addAndMakeVisible(morphAmountLabel);
    morphAmountAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "morphAmount", morphAmountSlider);

    // === Morph Frequency ===
    morphFreqSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    morphFreqSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(morphFreqSlider);
    morphFreqLabel.setText("Morph Freq", juce::dontSendNotification);
    morphFreqLabel.attachToComponent(&morphFreqSlider, false);
    addAndMakeVisible(morphFreqLabel);
    morphFreqAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "morphFreq", morphFreqSlider);

    // === Morph Source ===
    morphSourceBox.addItem("Sidechain", 1);
    morphSourceBox.addItem("Sine", 2);
    morphSourceBox.addItem("Saw", 3);
    morphSourceBox.addItem("Square", 4);
    morphSourceBox.addItem("Tri", 5);
    morphSourceBox.setSelectedId(2, juce::dontSendNotification);
    addAndMakeVisible(morphSourceBox);
    morphSourceLabel.setText("Source", juce::dontSendNotification);
    morphSourceLabel.attachToComponent(&morphSourceBox, false);
    addAndMakeVisible(morphSourceLabel);
    morphSourceAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        audioProcessor.apvts, "morphSource", morphSourceBox);
}

NewProjectAudioProcessorEditor::~NewProjectAudioProcessorEditor()
{
}

//==============================================================================
void NewProjectAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (15.0f));
}

void NewProjectAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(20);
    auto sectionW = area.getWidth() / 3;
    int  knobSize = 90;
    int  labelH = 30;
    int  comboH = 24;

    // === BitCrusher ===
    auto bitArea = area.removeFromLeft(sectionW);
    bitArea.removeFromTop(labelH);
    auto bitRow = bitArea.removeFromTop(knobSize + labelH);
    bitDepthSlider.setBounds(bitRow.removeFromLeft(sectionW / 2).withSizeKeepingCentre(knobSize, knobSize));

    // === Distortion ===
    auto distArea = area.removeFromLeft(sectionW);
    distArea.removeFromTop(labelH);
    auto distRow = distArea.removeFromTop(knobSize + labelH);
    distDriveSlider.setBounds(distRow.removeFromLeft(sectionW / 2).withSizeKeepingCentre(knobSize, knobSize));
    distMixSlider.setBounds(distRow.withSizeKeepingCentre(knobSize, knobSize));
    auto distComboRow = distArea.removeFromTop(labelH + comboH);
    distComboRow.removeFromTop(labelH);
    distTypeBox.setBounds(distComboRow.withSizeKeepingCentre(160, comboH));

    // === Morph ===
    auto morphArea = area;
    morphArea.removeFromTop(labelH);
    auto morphRow = morphArea.removeFromTop(knobSize + labelH);
    morphAmountSlider.setBounds(morphRow.removeFromLeft(sectionW / 2).withSizeKeepingCentre(knobSize, knobSize));
    morphFreqSlider.setBounds(morphRow.withSizeKeepingCentre(knobSize, knobSize));
    auto morphComboRow = morphArea.removeFromTop(labelH + comboH);
    morphComboRow.removeFromTop(labelH);
    morphSourceBox.setBounds(morphComboRow.withSizeKeepingCentre(160, comboH));
}
