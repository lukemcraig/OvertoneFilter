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

    //==============================================================================
    float getCurrentResonance(int channel);

    void skipResonance(int channel, const int numSamples);

    //==============================================================================
    float getInputGain(int channel);

    float getOutGain(int channel);

    //==============================================================================
    float getWetGain(int channel);

    float getCurrentWetGain(int channel);

    void setCurrentWetGain(int channel, const float currentGain);

    //==============================================================================
    float getMix(int channel);

    void setMixTarget(int channel, float target);

    float getCurrentMix(int channel);

    void setCurrentMix(int channel, const float currentWetDry);

    //==============================================================================
    void useNoteOffWetDry(int channel);

    void useParamWetDry(int channel);

    //==============================================================================

    const String pidInputGain = "inputGain";
    const String pidWetGain = "wetGain";
    const String pidOutputGain = "outputGain";
    const String pidMix = "mix";
    const String pidPitchStandard = "standard";
    const String pidResonance = "q";
    //==============================================================================
    AudioProcessorValueTreeState valueTreeState;

private:

    typedef SmoothedValue<float, ValueSmoothingTypes::Linear> SmoothFloat;
    std::vector<SmoothFloat> smoothStandard{};
    std::vector<SmoothFloat> smoothResonance{};
    std::vector<SmoothFloat> smoothInGain{};
    std::vector<SmoothFloat> smoothWetGain{};
    std::vector<SmoothFloat> smoothOutGain{};
    std::vector<SmoothFloat> smoothMix{};
    //==============================================================================
    std::vector<bool> useInternalWetDry{};

    //==============================================================================
    AudioProcessorValueTreeState::ParameterLayout createParameterLayout() const;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParameterHelper)
};
