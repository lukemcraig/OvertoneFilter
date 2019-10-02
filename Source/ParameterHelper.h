/*
  ==============================================================================

    ParameterHelper.h
    Created: 25 Sep 2019 12:18:36pm
    Author:  Luke McDuffie Craig

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class ParameterHelper
{
public:
    //==============================================================================
    ParameterHelper(AudioProcessor& processorToConnectTo);

    ~ParameterHelper();

    //==============================================================================
    /**
     * \brief Resizes each vector of smoothers to match the number of channels
     */
    void prepare(int numChannels);

    void resetSmoothers(double sampleRate);

    /**
     * \brief setCurrentAndTargetValue for all the smoothers. 
     */
    void instantlyUpdateSmoothers();

    void updateSmoothers();

    //==============================================================================
    float getCurrentPitchStandard(int channel);

    void skipPitchStandard(int channel, int numSamples);

    float getQ(int channel);

    float getGain(int channel);

    float getWetDry(int channel);

    void setWetDryTarget(int channel, float target);

    float getCurrentWetDry(int channel);

    void setCurrentWetDry(int channel, const float currentWetDry);

    float getCurrentGain(int channel);

    void setCurrentGain(int channel, const float currentGain);

    //==============================================================================
    void useNoteOffWetDry(int channel);

    void useParamWetDry(int channel);

    //==============================================================================
    //TODO rename these?
    const String PID_Q = "q";
    const String PID_GAIN = "gain";
    const String PID_WETDRY = "wetdry";
    const String PID_PITCH_STANDARD = "standard";

    //==============================================================================
    AudioProcessorValueTreeState valueTreeState;

private:

    typedef SmoothedValue<float, ValueSmoothingTypes::Linear> SmoothFloat;
        std::vector<SmoothFloat> smoothStandard{};
    std::vector<SmoothFloat> smoothQ{};
    std::vector<SmoothFloat> smoothGain{};
    std::vector<SmoothFloat> smoothWetDry{};
    //==============================================================================
    std::vector<bool> useInternalWetDry{};

    //==============================================================================
    AudioProcessorValueTreeState::ParameterLayout createParameterLayout() const;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParameterHelper)
};
