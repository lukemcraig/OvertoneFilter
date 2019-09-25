#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MidiWahAudioProcessor::MidiWahAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations //TODO remove this ifndef
    : AudioProcessor(BusesProperties()
                     .withInput("Input", AudioChannelSet::stereo(), true)
                     .withOutput("Output", AudioChannelSet::stereo(), true)
      ),
#endif
      parameterHelper(*this)
{
    //TODO remove this?
    midiDebugNumber = 400.0f;

    inverseSampleRate = 1.0 / 44100.0;
}

MidiWahAudioProcessor::~MidiWahAudioProcessor()
{
}

//==============================================================================
void MidiWahAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    processSpec.sampleRate = sampleRate;
    processSpec.maximumBlockSize = samplesPerBlock;
    processSpec.numChannels = getMainBusNumOutputChannels();

    numWahFilters = getTotalNumInputChannels();

    ladderFilters.resize(numWahFilters);
    for (auto i = 0; i < numWahFilters; ++i)
    {
        ladderFilters[i].reset(new LadderFilter());
        auto filter = ladderFilters[i].get();
        filter->reset();
        filter->prepare(processSpec);
        filter->setMode(LadderFilter::Mode::LPF12);
        filter->setDrive(*parameterHelper.valueTreeState.getRawParameterValue(parameterHelper.PID_DRIVE));
    }

    inverseSampleRate = 1.0 / sampleRate;

    updateFilters();
}

void MidiWahAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MidiWahAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
	ignoreUnused(layouts);
	return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
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

void MidiWahAudioProcessor::processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    keyboardState.processNextMidiBuffer(midiMessages,0,buffer.getNumSamples(),false);

    MidiMessage mResult;
    int mSamplePosition;
    for (MidiBuffer::Iterator i(midiMessages); i.getNextEvent(mResult, mSamplePosition);)
    {
        if (mResult.isNoteOn())
        {
            // convert the midi number to Hz, assuming A is 440Hz
            const auto newFreq = 440.0f * pow(2.0f, (static_cast<float>(mResult.getNoteNumber()) - 69.0f) / 12.0f);
            if (midiDebugNumber != newFreq)
            {
                midiDebugNumber = newFreq;
                updateFilters();
            }

            //midiFreq_.setValue(midiDebugNumber_,nullptr);
        }
    }
    dsp::AudioBlock<float> block(buffer);

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        //auto* channelData = buffer.getWritePointer(channel);

        // wah-wah: 2nd order IIR filter, typically band pass on guitar pedals,
        // resonant low-pass on analog synths, and sometimes a peaking filter.
        // center freq is 400-1200 Hz
        //wahFilters_[channel]->processSamples(channelData, buffer.getNumSamples());

        auto blockChannel = block.getSingleChannelBlock(channel);

        ladderFilters[channel]->process(dsp::ProcessContextReplacing<float>(blockChannel));
    }
    buffer.applyGain(*parameterHelper.valueTreeState.getRawParameterValue(parameterHelper.PID_GAIN));
}

void MidiWahAudioProcessor::updateFilters()
{
    for (int i = 0; i < numWahFilters; ++i)
    {
        auto filter = ladderFilters[i].get();
        filter->setCutoffFrequencyHz(midiDebugNumber);
        filter->setResonance(*parameterHelper.valueTreeState.getRawParameterValue(parameterHelper.PID_Q));
        filter->setDrive(*parameterHelper.valueTreeState.getRawParameterValue(parameterHelper.PID_DRIVE));
    }
}

//==============================================================================
bool MidiWahAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* MidiWahAudioProcessor::createEditor()
{
    return new MidiWahAudioProcessorEditor(*this, parameterHelper, keyboardState);
}

//==============================================================================
void MidiWahAudioProcessor::getStateInformation(MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void MidiWahAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MidiWahAudioProcessor();
}
