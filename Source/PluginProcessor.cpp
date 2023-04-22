/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <fftw3.h>

//==============================================================================
FftPassthroughAudioProcessor::FftPassthroughAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

FftPassthroughAudioProcessor::~FftPassthroughAudioProcessor()
{
}

//==============================================================================
const juce::String FftPassthroughAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool FftPassthroughAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool FftPassthroughAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool FftPassthroughAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double FftPassthroughAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int FftPassthroughAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int FftPassthroughAudioProcessor::getCurrentProgram()
{
    return 0;
}

void FftPassthroughAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String FftPassthroughAudioProcessor::getProgramName (int index)
{
    return {};
}

void FftPassthroughAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void FftPassthroughAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currentBufferSize = (float) samplesPerBlock;
    currentSampleRate = sampleRate;
    
    cBufferPointer = 0;
    hopCounter = 0;
    cBuffer = std::vector<float>(CBUFFER_SIZE);
    for (int i=0; i<CBUFFER_SIZE; i++) {
        cBuffer[i] = 0;
    }
}

void FftPassthroughAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool FftPassthroughAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void FftPassthroughAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    auto* channelData = buffer.getWritePointer (0);
    for (int i=0; i<currentBufferSize; i++) {
        cBuffer[cBufferPointer] = channelData[i];
        cBufferPointer++;
        if (cBufferPointer >= CBUFFER_SIZE) {
            cBufferPointer = 0;
        }

    }
    
}

//==============================================================================
bool FftPassthroughAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* FftPassthroughAudioProcessor::createEditor()
{
    return new FftPassthroughAudioProcessorEditor (*this);
}

//==============================================================================
void FftPassthroughAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void FftPassthroughAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FftPassthroughAudioProcessor();
}
