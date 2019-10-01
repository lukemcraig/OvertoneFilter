/*
  ==============================================================================

    ParameterHelper.h
    Created: 25 Sep 2019 12:18:36pm
    Author:  Luke

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

    void resetSmoothers(double sampleRate);

    /**
     * \brief setCurrentAndTargetValue for all the smoothers. 
     * Called from the constructor and from setStateInformation
     */
    void instantlyUpdateSmoothers();

    void updateSmoothers();

    //==============================================================================
    float getQ();

    float getGain();

    float getWetDry();

    void setWetDryTarget(float target);

    float getCurrentWetDry();

    void setCurrentWetDry(const float currentWetDry);

    float getCurrentGain();

    void setCurrentGain(const float currentGain);

    void useNoteOffWetDry();

    void useParamWetDry();

    //==============================================================================
    //TODO rename these?
    const String PID_Q = "q";
    const String PID_GAIN = "gain";
    const String PID_WETDRY = "wetdry";

    //==============================================================================
    AudioProcessorValueTreeState valueTreeState;

private:

    typedef SmoothedValue<float, ValueSmoothingTypes::Linear> SmoothFloat;
    SmoothFloat smoothQ{};
    SmoothFloat smoothGain{};
    SmoothFloat smoothWetDry{};
    //==============================================================================
    bool useInternalWetDry{};

    //==============================================================================
    AudioProcessorValueTreeState::ParameterLayout createParameterLayout() const;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParameterHelper)
};
