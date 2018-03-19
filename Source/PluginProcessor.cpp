/*
  ==============================================================================

	This file was auto-generated!

	It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
MidiWahAudioProcessor::MidiWahAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
	: AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
		.withInput("Input", AudioChannelSet::stereo(), true)
#endif
		.withOutput("Output", AudioChannelSet::stereo(), true)
#endif
	),
#endif
	parameters(*this, nullptr)
{
	wahFilters_ = nullptr;
	numWahFilters_ = 0;
	midiDebugNumber_ = 400.0f;

	parameters.createAndAddParameter(PID_CENTERFREQ, // parameter ID
		"Wah Center Frequency", // paramter Name
		String("Hz"), // parameter label (suffix)
		//NormalisableRange<float>(400.0f, 1200.0f, 0, 0.5f), //range
		NormalisableRange<float>(20.0f, 19000.0f, 0, 0.5f), //range
		600.0f, // default value
		nullptr,
		nullptr);

	parameters.createAndAddParameter(PID_Q, // parameter ID
		"Wah Q", // paramter Name
		String(""), // parameter label (suffix)
		NormalisableRange<float>(0.1f, 10.0f, 0, 1.5f), //range
		5.0f, // default value
		nullptr,
		nullptr);

	parameters.createAndAddParameter(PID_GAIN, // parameter ID
		"Makeup Gain", // paramter Name
		String(""), // parameter label (suffix)
		NormalisableRange<float>(0.0f, 10.0f, 0, 1.5f), //range
		1.0f, // default value
		nullptr,
		nullptr);	

	parameters.state = ValueTree(Identifier("MidiWahParameters"));
	//midiFreq_.referTo(parameters.state, PID_CENTERFREQ, nullptr);

	inverseSampleRate_ = 1.0 / 44100.0;	
}

MidiWahAudioProcessor::~MidiWahAudioProcessor()
{
}

//==============================================================================
const String MidiWahAudioProcessor::getName() const
{
	return JucePlugin_Name;
}

bool MidiWahAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
	return true;
#else
	return false;
#endif
}

bool MidiWahAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
	return true;
#else
	return false;
#endif
}

bool MidiWahAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
	return true;
#else
	return false;
#endif
}

double MidiWahAudioProcessor::getTailLengthSeconds() const
{
	return 0.0;
}

int MidiWahAudioProcessor::getNumPrograms()
{
	return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
				// so this should be at least 1, even if you're not really implementing programs.
}

int MidiWahAudioProcessor::getCurrentProgram()
{
	return 0;
}

void MidiWahAudioProcessor::setCurrentProgram(int index)
{
}

const String MidiWahAudioProcessor::getProgramName(int index)
{
	return {};
}

void MidiWahAudioProcessor::changeProgramName(int index, const String& newName)
{
}

//==============================================================================
void MidiWahAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
	// Use this method as the place to do any pre-playback
	// initialisation that you need..
	numWahFilters_ = getTotalNumInputChannels();

	wahFilters_ = (MyBandPass**)malloc(numWahFilters_ * sizeof(MyBandPass*));

	for (int i = 0; i < numWahFilters_; ++i)
		wahFilters_[i] = new MyBandPass;

	inverseSampleRate_ = 1.0 / sampleRate;

	updateFilters();
}

void MidiWahAudioProcessor::releaseResources()
{
	// When playback stops, you can use this as an opportunity to free up any
	// spare memory, etc.
	for(int i=0;i<numWahFilters_;++i)
		delete wahFilters_[i];
	free(wahFilters_);
	wahFilters_ = nullptr;
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

	MidiMessage mResult;
	int mSamplePosition;
	for (MidiBuffer::Iterator i(midiMessages); i.getNextEvent(mResult, mSamplePosition);)
	{
		if (mResult.isNoteOn())
		{		
			// convert the midi number to Hz, assuming A is 440Hz
			float newFreq_ = 440.0f * pow(2.0f, ((float)mResult.getNoteNumber() - 69.0f) / 12.0f);
			if (midiDebugNumber_ != newFreq_) {
				midiDebugNumber_ = newFreq_;
				updateFilters();
			}

			//midiFreq_.setValue(midiDebugNumber_,nullptr);
		}		
	}

	// This is the place where you'd normally do the guts of your plugin's
	// audio processing...
	// Make sure to reset the state if your inner loop is processing
	// the samples and the outer loop is handling the channels.
	// Alternatively, you can process the samples with the channels
	// interleaved by keeping the same state.
	for (int channel = 0; channel < totalNumInputChannels; ++channel)
	{
		auto* channelData = buffer.getWritePointer(channel);

		// wah-wah: 2nd order IIR filter, typically band pass on guitar pedals,
		// resonant low-pass on analog synths, and sometimes a peaking filter.
		// center freq is 400-1200 Hz
		wahFilters_[channel]->processSamples(channelData, buffer.getNumSamples());
		//for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
		//	float in = channelData[sample];
		//	float out = wahFilter_->processSingleSampleRaw(in);
		//	channelData[sample] = out;
		//}		
	}
	buffer.applyGain(*parameters.getRawParameterValue(PID_GAIN));
}

void MidiWahAudioProcessor::updateFilters()
{	
	for (int i = 0; i < numWahFilters_; ++i)
		//wahFilters_[i]->makeMyBandPass(inverseSampleRate_, (double)*parameters.getRawParameterValue(PID_CENTERFREQ), (double)*parameters.getRawParameterValue(PID_Q));
		wahFilters_[i]->makeMyBandPass(inverseSampleRate_, (double)midiDebugNumber_, (double)*parameters.getRawParameterValue(PID_Q));
}

//==============================================================================
bool MidiWahAudioProcessor::hasEditor() const
{
	return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* MidiWahAudioProcessor::createEditor()
{
	return new MidiWahAudioProcessorEditor(*this, parameters);
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