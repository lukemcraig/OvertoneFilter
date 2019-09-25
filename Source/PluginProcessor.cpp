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
    filterCutoff = 400.0f;

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

    numWahFilters = getTotalNumInputChannels();

    ladderFilters.resize(numWahFilters);
    for (auto i = 0; i < numWahFilters; ++i)
    {
        ladderFilters[i].reset(new LadderFilter());
        auto filter = ladderFilters[i].get();
        filter->reset();
        filter->prepare(processSpec);
        filter->setMode(LadderFilter::Mode::LPF24);
        //filter->setDrive(*parameterHelper.valueTreeState.getRawParameterValue(parameterHelper.PID_DRIVE));
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

    keyboardState.processNextMidiBuffer(midiMessages, 0, buffer.getNumSamples(), false);

    dsp::AudioBlock<float> block(buffer);

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
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
        filter->setCutoffFrequencyHz(filterCutoff);
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
    auto state = parameterHelper.valueTreeState.copyState();
    std::unique_ptr<XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void MidiWahAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(parameterHelper.valueTreeState.state.getType()))
            parameterHelper.valueTreeState.replaceState(ValueTree::fromXml(*xmlState));
}

void MidiWahAudioProcessor::handleNoteOn(MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity)
{
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
    DBG("note off");
}

void MidiWahAudioProcessor::parameterChanged(const String& parameterID, float newValue)
{
    //if (parameterID == parameterHelper.PID_CENTERFREQ)
    //{
    //    DBG("PID_CENTERFREQ changed");
    //    //	processor.updateFilters();
    //}
    if (parameterID == parameterHelper.PID_Q)
    {
        updateFilters();
    }
    //else if (parameterID == parameterHelper.PID_GAIN)
    //{
    //    processor.updateFilters();
    //}
    else if (parameterID == parameterHelper.PID_DRIVE)
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
