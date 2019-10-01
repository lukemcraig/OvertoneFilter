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

    parameterHelper.prepare(getTotalNumInputChannels());
    parameterHelper.resetSmoothers(sampleRate);
    parameterHelper.instantlyUpdateSmoothers();

    filterCutoff.resize(getTotalNumInputChannels());
    for (auto&& cutoff : filterCutoff)
        cutoff = 500.0f;

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

    const auto subBlockSize = 16;
    const int numSubBlocks = buffer.getNumSamples() / subBlockSize;
    const int samplesLeft = buffer.getNumSamples() - (numSubBlocks * subBlockSize);

    for (auto channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto blockChannel = block.getSingleChannelBlock(channel);

        for (auto i = 0; i < numSubBlocks; ++i)
        {
            const auto startSample = i * subBlockSize;
            keyboardState.processNextMidiBuffer(midiMessages, startSample, subBlockSize, false);

            auto subBlock = blockChannel.getSubBlock(i * subBlockSize, subBlockSize);
            auto resonance = parameterHelper.getQ(channel);
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
        if (samplesLeft > 0)
        {
            const auto startSample = numSubBlocks * subBlockSize;
            keyboardState.processNextMidiBuffer(midiMessages, startSample, samplesLeft, false);

            auto subBlock = blockChannel.getSubBlock(startSample, samplesLeft);
            auto resonance = parameterHelper.getQ(channel);
            for (auto filterN = 0; filterN < numFiltersPerChannel; ++filterN)
            {
                const auto filterIndex = channel * numFiltersPerChannel + filterN;
                filters[filterIndex]->setCutoffFrequencyHz(filterCutoff[channel]);
                filters[filterIndex]->setResonance(resonance);
                filters[filterIndex]->process(
                    dsp::ProcessContextReplacing<float>(subBlock));
            }
            for (auto sample = 0; sample < samplesLeft; ++sample)
            {
                const auto wetDry = parameterHelper.getWetDry(channel);
                buffer.applyGain(channel, startSample + sample, 1, 1.0f - wetDry);
                buffer.addFrom(channel, startSample + sample, wetMix, channel,
                               startSample + sample, 1, wetDry);

                const auto outGain = parameterHelper.getGain(channel);
                buffer.applyGain(channel, startSample + sample, 1, outGain);
            }
        }
    }
}

void MidiWahAudioProcessor::updateFilters()
{
    DBG("updateFilters");
    //for (int i = 0; i < numFilters; ++i)
    //{
    //    auto filter = filters[i].get();

    //    filter->setCutoffFrequencyHz(filterCutoff);

    //    filter->setResonance(parameterHelper.getQ());
    //}
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
    parameterHelper.useParamWetDry(currentChannel);

    const auto newFreq = 440.0f * pow(2.0f, (static_cast<float>(midiNoteNumber) - 69.0f) / 12.0f);
    if (filterCutoff[currentChannel] != newFreq)
    {
        filterCutoff[currentChannel] = newFreq;
        updateFilters();
    }
}

void MidiWahAudioProcessor::handleNoteOff(MidiKeyboardState* source, int midiChannel, int midiNoteNumber,
                                          float velocity)
{
    parameterHelper.useNoteOffWetDry(currentChannel);
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
