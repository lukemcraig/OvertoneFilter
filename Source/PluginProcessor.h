#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "ParameterHelper.h"

//==============================================================================
/**
*/
class OvertoneFilterAudioProcessor : public AudioProcessor
{
public:
    //==============================================================================
    OvertoneFilterAudioProcessor();

    ~OvertoneFilterAudioProcessor();

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;

    void releaseResources() override;

    void reset() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    /**
     * \brief Handles note on for all channels (called from the GUI)
     * \param noteNumber 
     */
    void handleNoteOn(float noteNumber);

    void handleNoteOn(int channel, float noteNumber);

    /**
     * \brief Handles note off for all channels (called from the GUI)
     */
    void handleNoteOff();

    void handleNoteOff(int channel);

    void processSubBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages, int subBlockSize, int channel,
                         dsp::AudioBlock<float> blockChannel, int startSample);
#endif

    void processBlock(AudioBuffer<float>&, MidiBuffer&) override;

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
private:
    MidiKeyboardState keyboardState;
    ParameterHelper parameterHelper;

    dsp::ProcessSpec processSpec{};

    typedef dsp::LadderFilter<float> LadderFilter;
    std::vector<std::unique_ptr<LadderFilter>> filters;

    int numInputChannels{};
    int numFilters{};
    const int numFiltersPerChannel = 2;
    std::vector<float> filterCutoff{};

    AudioBuffer<float> wetMix;
    float level{};
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OvertoneFilterAudioProcessor)
};
