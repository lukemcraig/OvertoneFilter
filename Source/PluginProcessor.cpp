/*
  ==============================================================================
    Author:  Luke McDuffie Craig
  ==============================================================================
*/
#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
OvertoneFilterAudioProcessor::OvertoneFilterAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
                     .withInput("Input", AudioChannelSet::stereo(), true)
                     .withOutput("Output", AudioChannelSet::stereo(), true)
      ),
#endif
      parameterHelper(*this)
{
}

OvertoneFilterAudioProcessor::~OvertoneFilterAudioProcessor()
{
}

//==============================================================================
void OvertoneFilterAudioProcessor::prepareToPlay(const double sampleRate, const int samplesPerBlock)
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

    parameterHelper.prepare(numInputChannels);
    parameterHelper.resetSmoothers(sampleRate);
    parameterHelper.instantlyUpdateSmoothers();

    for (auto channel = 0; channel < numInputChannels; ++channel)
    {
        parameterHelper.useNoteOffMix(channel);
    }

    filterCutoff.resize(numInputChannels);
    for (auto&& cutoff : filterCutoff)
        cutoff = 500.0f;

    wetMix.setSize(numInputChannels, samplesPerBlock, false, false, false);
    wetMix.clear();
    //--------
    notesHeldDown.resize(numInputChannels);
    //--------
    inputLevel.prepare(0.010f, static_cast<float>(sampleRate));
    wetMixLevel.prepare(0.010f, static_cast<float>(sampleRate));
    outputLevel.prepare(0.010f, static_cast<float>(sampleRate));
    //--------
    outputSpectrumSource.setSampleRate(sampleRate);
    inputSpectrumSource.setSampleRate(sampleRate);
}

void OvertoneFilterAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

void OvertoneFilterAudioProcessor::reset()
{
    for (auto i = 0; i < numInputChannels; ++i)
        parameterHelper.useNoteOffMix(i);
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool OvertoneFilterAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
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

void OvertoneFilterAudioProcessor::handleNoteOn(const float noteNumber)
{
    for (auto i = 0; i < numInputChannels; ++i)
        handleNoteOn(i, noteNumber);
}

void OvertoneFilterAudioProcessor::handleNoteOn(const int channel, const float noteNumber)
{
    auto& l = notesHeldDown[channel];
    // a note is held down, so use the parameter (slider) mix value
    parameterHelper.useParamMix(channel);
    // add the new note to the end of the linked list
    l.push_back(noteNumber);
    // if no other note was held down already, update the cutoff frequency
    if (l.size() == 1)
    {
        const auto standard = parameterHelper.getCurrentPitchStandard(channel);
        const auto newFreq = standard * std::pow(2.0f, (noteNumber - 69.0f) / 12.0f);
        filterCutoff[channel] = newFreq;
    }
}

void OvertoneFilterAudioProcessor::handleNoteOff(const float noteNumber)
{
    for (auto i = 0; i < numInputChannels; ++i)
        handleNoteOff(i, noteNumber);
}

void OvertoneFilterAudioProcessor::handleNoteOff(const int channel, const float noteNumber)
{
    auto& l = notesHeldDown[channel];
    l.remove(noteNumber);
    // if no notes are held down, smooth the mix to dry
    if (l.empty())
        parameterHelper.useNoteOffMix(channel);
    else
    {
        // otherwise, set the cutoff to the oldest note held down
        const auto oldestNoteNumber = l.front();
        //todo code duplication
        const auto standard = parameterHelper.getCurrentPitchStandard(channel);
        const auto newFreq = standard * std::pow(2.0f, (oldestNoteNumber - 69.0f) / 12.0f);
        filterCutoff[channel] = newFreq;
    }
}

void OvertoneFilterAudioProcessor::processSubBlockMidi(MidiBuffer& midiMessages, const int startSample, const int numSamples,
                                                       const int channel)
{
    MidiBuffer::Iterator iterator(midiMessages);
    iterator.setNextSamplePosition(startSample);
    MidiMessage message;
    int sampleNumber;
    while (iterator.getNextEvent(message, sampleNumber))
    {
        if (sampleNumber > startSample + numSamples)
            break;
        if (message.isNoteOn())
        {
            const auto noteNumber = static_cast<float>(message.getNoteNumber());
            handleNoteOn(channel, noteNumber);
        }

        else if (message.isNoteOff())
        {
            const auto noteNumber = static_cast<float>(message.getNoteNumber());
            handleNoteOff(channel, noteNumber);
        }
    }
}

void OvertoneFilterAudioProcessor::processSubBlock(AudioBuffer<float>& buffer,
                                                   const dsp::AudioBlock<float> blockChannel,
                                                   MidiBuffer& midiMessages,
                                                   const int startSample,
                                                   const int numSamples,
                                                   const int channel)
{
    processSubBlockMidi(midiMessages, startSample, numSamples, channel);

    // apply gain to the input (wetMix has already been copied, so this doesn't apply to it)
    for (auto sample = 0; sample < numSamples; ++sample)
    {
        buffer.applyGain(channel, startSample + sample, 1, parameterHelper.getInputGain(channel));

        // input meter
        if (channel == 0)
        {
            const auto sampleData = buffer.getSample(0, startSample + sample);
            inputLevel.pushSample(sampleData);
            inputSpectrumSource.pushSample(sampleData);
        }
    }

    auto subBlock = blockChannel.getSubBlock(startSample, numSamples);

    const auto resonance = parameterHelper.getCurrentResonance(channel);
    for (auto filterN = 0; filterN < numFiltersPerChannel; ++filterN)
    {
        const auto filterIndex = channel * numFiltersPerChannel + filterN;
        filters[filterIndex]->setCutoffFrequencyHz(filterCutoff[channel]);
        filters[filterIndex]->setResonance(resonance);
        filters[filterIndex]->process(
            dsp::ProcessContextReplacing<float>(subBlock));
    }
    for (auto sample = 0; sample < numSamples; ++sample)
    {
        // apply the gain to the wet signal
        wetMix.applyGain(channel, startSample + sample, 1, parameterHelper.getWetGain(channel));

        // wetMix meter
        if (channel == 0)
        {
            wetMixLevel.pushSample(subBlock.getSample(0, sample));
        }

        const auto wetDry = parameterHelper.getMix(channel);
        // blend the wet mix and the dry mix
        buffer.applyGain(channel, startSample + sample, 1, 1.0f - wetDry);
        buffer.addFrom(channel, startSample + sample, wetMix, channel,
                       startSample + sample, 1, wetDry);

        // apply the gain to the blended output signal
        buffer.applyGain(channel, startSample + sample, 1, parameterHelper.getOutGain(channel));

        // output meter
        if (channel == 0)
        {
            const auto sampleData = buffer.getReadPointer(0)[startSample + sample];

            outputLevel.pushSample(sampleData);
            outputSpectrumSource.pushSample(sampleData);
        }
    }
    parameterHelper.skipResonance(channel, numSamples);
    parameterHelper.skipPitchStandard(channel, numSamples);
}

void OvertoneFilterAudioProcessor::processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    const auto totalNumInputChannels = getTotalNumInputChannels();
    const auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    parameterHelper.updateSmoothers();

    // this just updates the GUI keyboard component
    keyboardState.processNextMidiBuffer(midiMessages, 0, buffer.getNumSamples(), false);

    for (auto channel = 0; channel < totalNumInputChannels; ++channel)
    {
        wetMix.copyFrom(channel, 0, buffer, channel, 0, buffer.getNumSamples());
    }
    const dsp::AudioBlock<float> block(wetMix);

    const auto subBlockSize = 16;
    const auto numSubBlocks = buffer.getNumSamples() / subBlockSize;
    const auto samplesLeft = buffer.getNumSamples() - (numSubBlocks * subBlockSize);

    for (auto channel = 0; channel < totalNumInputChannels; ++channel)
    {
        const auto blockChannel = block.getSingleChannelBlock(channel);

        for (auto i = 0; i < numSubBlocks; ++i)
        {
            const auto startSample = i * subBlockSize;
            processSubBlock(buffer, blockChannel, midiMessages, startSample, subBlockSize, channel);
        }
        if (samplesLeft > 0)
        {
            const auto startSample = numSubBlocks * subBlockSize;
            processSubBlock(buffer, blockChannel, midiMessages, startSample, samplesLeft, channel);
        }
    }
}

//==============================================================================
bool OvertoneFilterAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* OvertoneFilterAudioProcessor::createEditor()
{
    return new OvertoneFilterEditor(*this, parameterHelper, keyboardState, inputLevel, wetMixLevel, outputLevel,
                                    inputSpectrumSource, outputSpectrumSource);
}

//==============================================================================
void OvertoneFilterAudioProcessor::getStateInformation(MemoryBlock& destData)
{
    const auto state = parameterHelper.valueTreeState.copyState();
    const auto xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void OvertoneFilterAudioProcessor::setStateInformation(const void* data, const int sizeInBytes)
{
    const auto xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState != nullptr)
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
    return new OvertoneFilterAudioProcessor();
}
