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
    
    inWritePointer = FFT_SIZE;
    inReadPointer = 0;
    hopCounter = 0;
    inBuffer = new float[CBUFFER_SIZE];
    for (int i=0; i<CBUFFER_SIZE; i++) {
        inBuffer[i] = 0.0;
    }
    
    outWritePointer = FFT_SIZE;
    outReadPointer = 0;
    outBuffer = new float[CBUFFER_SIZE];
    for (int i=0; i<CBUFFER_SIZE; i++) {
        inBuffer[i] = 0.0;
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
    //auto totalNumInputChannels  = getTotalNumInputChannels();
    //auto totalNumOutputChannels = getTotalNumOutputChannels();

    auto* channelData = buffer.getWritePointer (0);
    for (int i=0; i<currentBufferSize; i++) {
        
        // store juce input signal into input buffer
        inBuffer[inWritePointer] = channelData[i];
        inWritePointer++;
        if (inWritePointer >= CBUFFER_SIZE) {
            inWritePointer = 0;
        }
        hopCounter++;
        
        // do spectral processing
        if (hopCounter >= HOP_SIZE) {
            hopCounter = 0;
            processFft();
        }
        
        // read outBuffer (processed signal) and write to juce buffer
        channelData[i] = outBuffer[outReadPointer];
        outReadPointer++;
        if (outReadPointer >= CBUFFER_SIZE) {
            outReadPointer = 0;
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

void FftPassthroughAudioProcessor::processFft() {
    
    // unwrap input circular buffer
    for (int i=0; i<FFT_SIZE; i++) {
        inFft[i] = inBuffer[inReadPointer];
        inReadPointer++;
        if (inReadPointer >= CBUFFER_SIZE) {
            inReadPointer = 0;
        }
    }
    
    computeFft(FFT_SIZE, inFft, outFft);
    // spectral processing start ------------------------
    
    // spectral processing end --------------------------
    computeIfft(FFT_SIZE, outFft, outIfft);
    
    // store outIfft into outBuffer
    for (int i=0; i<FFT_SIZE; i++) {
        outBuffer[outWritePointer] = (outIfft[i] / FFT_SIZE);
        outWritePointer++;
        if (outWritePointer >= CBUFFER_SIZE) {
            outWritePointer = 0;
        }
    }
    
}


void FftPassthroughAudioProcessor::computeFft(int bufferSize, float* input, std::complex<float>* output) {
    double* in;
    fftw_complex* out;
    fftw_plan p;
    
    // allocate mem for in
    in = (double*) fftw_malloc(sizeof(double) * bufferSize);
    // copy input to in
    for (int i=0; i<bufferSize; i++) {
        in[i] = (double) input[i];
    }
    //calculate out size
    //int n_out = ((bufferSize/2)+1);
    // allocate mem for out
    out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * bufferSize);
    
    //plan fft
    p = fftw_plan_dft_r2c_1d(bufferSize, in, out, FFTW_ESTIMATE);
    //perform fft
    fftw_execute(p); /* repeat as needed */

    //copy result to output
    for (int i=0; i<bufferSize; i++) {
        output[i].real(out[i][0]);
        output[i].imag(out[i][1]);
    }
    
    // free resources
    fftw_destroy_plan(p);
    fftw_free(out);
    fftw_free(in);
}

void FftPassthroughAudioProcessor::computeIfft(int bufferSize, std::complex<float>* input, float* output) {
    double* out;
    fftw_complex* in;
    fftw_plan p;
    
    //calculate in size
    //int n_in = ((bufferSize/2)+1);
    // allocate mem for in
    in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * bufferSize);
    // copy input to in
    for (int i=0; i<bufferSize; i++) {
        in[i][0] = input[i].real();
        in[i][1] = input[i].imag();
    }
    // allocate mem for out
    out = (double*) fftw_malloc(sizeof(double) * bufferSize);
    
    //plan ifft
    p = fftw_plan_dft_c2r_1d(bufferSize, in, out, FFTW_ESTIMATE);
    //perform fft
    fftw_execute(p); /* repeat as needed */
    
    // copy result to output
    for (int i=0; i<bufferSize; i++) {
        output[i] = (float) out[i];
    }
    
    // free resources
    fftw_destroy_plan(p);
    fftw_free(in);
    fftw_free(out);
}
