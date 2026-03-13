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
    auto leftSection = area.removeFromLeft(area.getWidth() / 2);
    auto rightSection = area;

    // --- BitCrush section: one knob centred ---
    auto bitArea = leftSection.reduced(20);
    bitArea.removeFromTop(30); // space for label
    bitDepthSlider.setBounds(bitArea.withSizeKeepingCentre(100, 100));

    // --- Distortion section: two knobs side by side, selector below ---
    auto distKnobRow = rightSection.reduced(20);
    distKnobRow.removeFromTop(30); // space for labels

    auto driveArea = distKnobRow.removeFromLeft(distKnobRow.getWidth() / 2);
    distDriveSlider.setBounds(driveArea.withSizeKeepingCentre(100, 100));

    distMixSlider.setBounds(distKnobRow.withSizeKeepingCentre(100, 100));

    // Selector sits below the knobs
    auto selectorArea = rightSection.removeFromBottom(50).reduced(20, 0);
    selectorArea.removeFromTop(20); // label space
    distTypeBox.setBounds(selectorArea.withSizeKeepingCentre(160, 24));

}
