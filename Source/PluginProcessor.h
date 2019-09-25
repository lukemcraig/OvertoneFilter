/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "ParameterHelper.h"

//==============================================================================
/**
*/
class MidiWahAudioProcessor : public AudioProcessor,
                              private MidiKeyboardStateListener
{
public:
    typedef dsp::LadderFilter<float> LadderFilter;

    //==============================================================================
    MidiWahAudioProcessor();

    ~MidiWahAudioProcessor();

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;

    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

    void processBlock(AudioBuffer<float>&, MidiBuffer&) override;

    void updateFilters();

    //==============================================================================
    AudioProcessorEditor* createEditor() override;

    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override { return JucePlugin_Name; }

    bool acceptsMidi() const override { return true; }

    bool producesMidi() const override { return false; }

    bool isMidiEffect() const override { return false; }

    double getTailLengthSeconds() const override { return 0.0; }

    //==============================================================================
    int getNumPrograms() override { return 1; }

    int getCurrentProgram() override { return 0; }

    void setCurrentProgram(int index) override { ; }

    const String getProgramName(int index) override { return {}; }

    void changeProgramName(int index, const String& newName) override { ; }

    //==============================================================================
    void getStateInformation(MemoryBlock& destData) override;

    void setStateInformation(const void* data, int sizeInBytes) override;

    //==============================================================================
    void handleNoteOn(MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity) override;

    void handleNoteOff(MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity) override;

    //==============================================================================
    // TODO remove
    float midiDebugNumber;
private:
    MidiKeyboardState keyboardState;

    dsp::ProcessSpec processSpec{};
    int numWahFilters{};
    std::vector<std::unique_ptr<LadderFilter>> ladderFilters;
    double inverseSampleRate;

    ParameterHelper parameterHelper;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiWahAudioProcessor)
};
