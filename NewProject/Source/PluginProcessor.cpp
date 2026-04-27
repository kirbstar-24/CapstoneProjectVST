/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MorphMiteAudioProcessor::MorphMiteAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                       .withInput("Sidechain", juce::AudioChannelSet::stereo(), false)
                       ),
                       
    apvts(*this, nullptr, "Parameters", createParameterLayout())
#endif
{
}

MorphMiteAudioProcessor::~MorphMiteAudioProcessor()
{
}

//==============================================================================
const juce::String MorphMiteAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MorphMiteAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool MorphMiteAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool MorphMiteAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double MorphMiteAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MorphMiteAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int MorphMiteAudioProcessor::getCurrentProgram()
{
    return 0;
}

void MorphMiteAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String MorphMiteAudioProcessor::getProgramName (int index)
{
    return {};
}

void MorphMiteAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void MorphMiteAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    bitCrush.prep(sampleRate);
    soundBank.prep(sampleRate);
    morph.prep(sampleRate);

    morphSourceBuffer.setSize(2, samplesPerBlock);
    morphSourceBuffer.clear();

    setLatencySamples(Morph::fftSize);
}

void MorphMiteAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MorphMiteAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void MorphMiteAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    currentBitDepth = *apvts.getRawParameterValue("bitDepth");
    currentDownsample = *apvts.getRawParameterValue("downsample");

    currentDistDrive = *apvts.getRawParameterValue("distDrive");
    currentDistMix = *apvts.getRawParameterValue("distMix");
    currentDistType = (int) *apvts.getRawParameterValue("distType");

    currentWaveType = (int)*apvts.getRawParameterValue("morphSource");

    morphAmount = *apvts.getRawParameterValue("morphAmount");

    currentMorphFreq = *apvts.getRawParameterValue("morphFreq");


    // Update DSP module
    bitCrush.setParameters(currentBitDepth, currentDownsample);
    distortion.setParameters(currentDistDrive, currentDistMix, currentDistType);
    
    // Process 
    bitCrush.process(buffer);
    distortion.process(buffer);

    if (morphAmount > 0.0f)
    {
        if (currentWaveType == 0) //select if sidechain or soundbank
        {
            auto* sideBus = getBus(true, 1);
            if (sideBus != nullptr && sideBus->isEnabled())
            {
                auto sideBuffer = getBusBuffer(buffer, true, 1);
                morph.setTarget(sideBuffer);
            }
        }
        else
        {
            auto waveType = static_cast<SoundBank::Wave>(currentWaveType - 1);
            soundBank.setWave(waveType);
            morphSourceBuffer.setSize(buffer.getNumChannels(), buffer.getNumSamples(), false, false, false);
            morphSourceBuffer.clear();
            soundBank.process(morphSourceBuffer, currentMorphFreq);
            morph.setTarget(morphSourceBuffer);
        }
    }
    morph.process(buffer, morphAmount);


    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

}

//==============================================================================
bool MorphMiteAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* MorphMiteAudioProcessor::createEditor()
{
    return new MorphMiteAudioProcessorEditor (*this);
}

//==============================================================================
void MorphMiteAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void MorphMiteAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

juce::AudioProcessorValueTreeState::ParameterLayout
MorphMiteAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // BitCrusher
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "bitDepth", "Bit Depth",
        juce::NormalisableRange<float>(1.0f, 16.0f, 1.0f), 8.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "downsample", "Downsample",
        juce::NormalisableRange<float>(1.0f, 20.0f, 1.0f), 1.0f));

    // Distortion
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "distDrive", "Drive",
        juce::NormalisableRange<float>(1.0f, 20.0f, 0.01f, 0.4f), 1.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "distMix", "Distortion Mix",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.0f));

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        "distType", "Distortion Type",
        juce::StringArray{ "Soft Clip", "Hard Clip", "Foldback" }, 0));

    // SoundBank
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        "morphSource", "Morph Source",
        juce::StringArray{ "Sidechain", "Sine", "Saw", "Square", "Tri" }, 1));

    // Morph
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "morphAmount", "Morph",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "morphFreq", "Morph Frequency",
        juce::NormalisableRange<float>(20.0f, 2000.0f, 0.1f, 0.3f), 110.0f));

    return layout;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MorphMiteAudioProcessor();
}
