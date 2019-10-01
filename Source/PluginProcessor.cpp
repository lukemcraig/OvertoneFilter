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

    numInputChannels = getTotalNumInputChannels();
    numFilters = numInputChannels * numFiltersPerChannel;

    filters.resize(numFilters);
    for (auto i = 0; i < numFilters; ++i)
    {
        filters[i].reset(new LadderFilter());
        auto filter = filters[i].get();
        filter->reset();
        filter->prepare(processSpec);
        filter->setMode(LadderFilter::Mode::LPF24);
    }

    inverseSampleRate = 1.0 / sampleRate;

    parameterHelper.prepare(numInputChannels);
    parameterHelper.resetSmoothers(sampleRate);
    parameterHelper.instantlyUpdateSmoothers();

    filterCutoff.resize(numInputChannels);
    for (auto&& cutoff : filterCutoff)
        cutoff = 500.0f;

    wetMix.setSize(numInputChannels, samplesPerBlock, false, false, false);
}

void MidiWahAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

void MidiWahAudioProcessor::reset()
{
    for (int i = 0; i < numInputChannels; ++i)
        parameterHelper.useNoteOffWetDry(i);
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

void MidiWahAudioProcessor::handleNoteOn(const float noteNumber)
{
    for (auto i = 0; i < numInputChannels; ++i)
        handleNoteOn(i, noteNumber);
}

void MidiWahAudioProcessor::handleNoteOn(int channel, const float noteNumber)
{
    parameterHelper.useParamWetDry(channel);
    //todo tuning standard
    const auto newFreq = 440.0f * pow(2.0f, (noteNumber - 69.0f) / 12.0f);
    filterCutoff[channel] = newFreq;
}

void MidiWahAudioProcessor::handleNoteOff()
{
    for (auto i = 0; i < numInputChannels; ++i)
        handleNoteOff(i);
}

void MidiWahAudioProcessor::handleNoteOff(int channel)
{
    parameterHelper.useNoteOffWetDry(channel);
}

void MidiWahAudioProcessor::processSubBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages,
                                            const int subBlockSize, int channel, dsp::AudioBlock<float> blockChannel,
                                            int startSample)
{
    {
        MidiBuffer::Iterator iterator(midiMessages);
        iterator.setNextSamplePosition(startSample);
        MidiMessage message;
        int sampleNumber;
        while (iterator.getNextEvent(message, sampleNumber))
        {
            if (sampleNumber > startSample + subBlockSize)
                break;
            if (message.isNoteOn())
            {
                const auto noteNumber = static_cast<float>(message.getNoteNumber());
                handleNoteOn(channel, noteNumber);
            }

            else if (message.isNoteOff())
            {
                handleNoteOff(channel);
            }
        }
    }
    //todo move this out of subblock loop?
    keyboardState.processNextMidiBuffer(midiMessages, startSample, subBlockSize, false);

    auto subBlock = blockChannel.getSubBlock(startSample, subBlockSize);
    const auto resonance = parameterHelper.getQ(channel);
    for (auto filterN = 0; filterN < numFiltersPerChannel; ++filterN)
    {
        const auto filterIndex = channel * numFiltersPerChannel + filterN;
        filters[filterIndex]->setCutoffFrequencyHz(filterCutoff[channel]);
        filters[filterIndex]->setResonance(resonance);
        filters[filterIndex]->process(
            dsp::ProcessContextReplacing<float>(subBlock));
    }
    for (auto sample = 0; sample < subBlockSize; ++sample)
    {
        const auto wetDry = parameterHelper.getWetDry(channel);
        buffer.applyGain(channel, startSample + sample, 1, 1.0f - wetDry);
        buffer.addFrom(channel, startSample + sample, wetMix, channel,
                       startSample + sample, 1, wetDry);

        const auto outGain = parameterHelper.getGain(channel);
        buffer.applyGain(channel, startSample + sample, 1, outGain);
    }
}

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

    parameterHelper.updateSmoothers();

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        wetMix.copyFrom(channel, 0, buffer, channel, 0, buffer.getNumSamples());
    }
    dsp::AudioBlock<float> block(wetMix);

    const auto subBlockSize = 16;
    const int numSubBlocks = buffer.getNumSamples() / subBlockSize;
    const int samplesLeft = buffer.getNumSamples() - (numSubBlocks * subBlockSize);

    for (auto channel = 0; channel < totalNumInputChannels; ++channel)
    {

        auto blockChannel = block.getSingleChannelBlock(channel);

        for (auto i = 0; i < numSubBlocks; ++i)
        {
            const auto startSample = i * subBlockSize;
            processSubBlock(buffer, midiMessages, subBlockSize, channel, blockChannel, startSample);
        }
        if (samplesLeft > 0)
        {
            const auto startSample = numSubBlocks * subBlockSize;
            processSubBlock(buffer, midiMessages, samplesLeft, channel, blockChannel, startSample);
        }
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
    auto state = parameterHelper.valueTreeState.copyState();
    std::unique_ptr<XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void MidiWahAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
    {
        if (xmlState->hasTagName(parameterHelper.valueTreeState.state.getType()))
        {
            parameterHelper.valueTreeState.replaceState(ValueTree::fromXml(*xmlState));
            parameterHelper.instantlyUpdateSmoothers();
        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MidiWahAudioProcessor();
}
