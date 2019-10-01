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
    filterCutoff = 1000.0f;

    inverseSampleRate = 1.0 / 44100.0;

    keyboardState.addListener(this);
}

MidiWahAudioProcessor::~MidiWahAudioProcessor()
{
    keyboardState.removeListener(this);
}

//==============================================================================
void MidiWahAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    processSpec.sampleRate = sampleRate;
    processSpec.maximumBlockSize = samplesPerBlock;
    processSpec.numChannels = getMainBusNumOutputChannels();

    numFilters = getTotalNumInputChannels() * numFiltersPerChannel;

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
    parameterHelper.resetSmoothers(sampleRate);

    updateFilters();

    wetMix.setSize(getTotalNumInputChannels(), samplesPerBlock, false, false, false);
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

    parameterHelper.updateSmoothers();

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        wetMix.copyFrom(channel, 0, buffer, channel, 0, buffer.getNumSamples());
    }
    dsp::AudioBlock<float> block(wetMix);

    for (int i = 0; i < numFilters; ++i)
    {
        auto filter = filters[i].get();
        filter->setResonance(parameterHelper.getQ());
    }
    const auto subBlockSize = 16;
    const int numSubBlocks = buffer.getNumSamples() / subBlockSize;
    const int samplesLeft = buffer.getNumSamples() - (numSubBlocks * subBlockSize);

    // const auto currentWetDry = parameterHelper.getCurrentWetDry();

    // const auto currentGain = parameterHelper.getCurrentGain();

    for (auto channel = 0; channel < 1; ++channel)
    {
        //  parameterHelper.setCurrentWetDry(currentWetDry);
        //  parameterHelper.setCurrentGain(currentGain);
        auto blockChannel = block.getSingleChannelBlock(channel);

        for (auto i = 0; i < numSubBlocks; ++i)
        {
            const auto startSample = i * subBlockSize;
            keyboardState.processNextMidiBuffer(midiMessages, startSample, subBlockSize, false);

            auto subBlock = blockChannel.getSubBlock(i * subBlockSize, subBlockSize);
            for (auto filterN = 0; filterN < numFiltersPerChannel; ++filterN)
            {
                filters[channel * numFiltersPerChannel + filterN]->process(
                    dsp::ProcessContextReplacing<float>(subBlock));
            }
            for (auto sample = 0; sample < subBlockSize; ++sample)
            {
                const auto wetDry = parameterHelper.getWetDry();
                buffer.applyGain(channel, startSample + sample, 1, 1.0f - wetDry);
                buffer.addFrom(channel, startSample + sample, wetMix, channel,
                               startSample + sample, 1, wetDry);

                const auto outGain = parameterHelper.getGain();
                buffer.applyGain(channel, startSample + sample, 1, outGain);
            }
        }
        if (samplesLeft > 0)
        {
            const auto startSample = numSubBlocks * subBlockSize;
            keyboardState.processNextMidiBuffer(midiMessages, startSample, samplesLeft, false);

            auto subBlock = blockChannel.getSubBlock(startSample, samplesLeft);
            for (auto filterN = 0; filterN < numFiltersPerChannel; ++filterN)
            {
                filters[channel * numFiltersPerChannel + filterN]->process(
                    dsp::ProcessContextReplacing<float>(subBlock));
            }
            for (auto sample = 0; sample < samplesLeft; ++sample)
            {
                const auto wetDry = parameterHelper.getWetDry();
                buffer.applyGain(channel, startSample + sample, 1, 1.0f - wetDry);
                buffer.addFrom(channel, startSample + sample, wetMix, channel,
                               startSample + sample, 1, wetDry);

                const auto outGain = parameterHelper.getGain();
                buffer.applyGain(channel, startSample + sample, 1, outGain);
            }
        }
    }
    for (auto channel = 1; channel < totalNumOutputChannels; ++channel)
    {
        buffer.copyFrom(channel, 0, buffer, 0, 0, buffer.getNumSamples());
    }
}

void MidiWahAudioProcessor::updateFilters()
{
    for (int i = 0; i < numFilters; ++i)
    {
        auto filter = filters[i].get();

        filter->setCutoffFrequencyHz(filterCutoff);

        filter->setResonance(parameterHelper.getQ());
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

void MidiWahAudioProcessor::handleNoteOn(MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity)
{
    parameterHelper.useParamWetDry();

    const auto newFreq = 440.0f * pow(2.0f, (static_cast<float>(midiNoteNumber) - 69.0f) / 12.0f);
    if (filterCutoff != newFreq)
    {
        filterCutoff = newFreq;
        updateFilters();
    }
}

void MidiWahAudioProcessor::handleNoteOff(MidiKeyboardState* source, int midiChannel, int midiNoteNumber,
                                          float velocity)
{
    parameterHelper.useNoteOffWetDry();
}

void MidiWahAudioProcessor::parameterChanged(const String& parameterID, float newValue)
{
    if (parameterID == parameterHelper.PID_Q)
    {
        updateFilters();
    }
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MidiWahAudioProcessor();
}
