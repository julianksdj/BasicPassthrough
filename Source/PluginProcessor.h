/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <complex>

// fft defines
#define FFT_SIZE 1024
#define HOP_SIZE 512

// circular buffer defines
#define CBUFFER_SIZE 16384

//==============================================================================
/**
*/
class FftPassthroughAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    FftPassthroughAudioProcessor();
    ~FftPassthroughAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    void computeFft(int bufferSize, float* input, std::complex<float>* output);
    void computeIfft(int bufferSize, std::complex<float>* input, float* output);
    
    void processFft();
    
private:
    
    float currentBufferSize;
    float currentSampleRate;
    
    // circular buffer
    float* cBuffer;
    int cBufferPointer;
    int hopCounter;
    
    float* inFft = new float[FFT_SIZE];
    std::complex<float>* outFft = new std::complex<float>[FFT_SIZE];
    float* outIfft = new float[FFT_SIZE];
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FftPassthroughAudioProcessor)
};
