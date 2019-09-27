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
    //TODO rename these?
    //const String PID_CENTERFREQ = "center_freq";
    const String PID_Q = "q";
    const String PID_GAIN = "gain";
    const String PID_WETDRY = "drive";
    //TODO remove these?
    const String pidToggleMidiSource = "toggle";

    //==============================================================================
    AudioProcessorValueTreeState valueTreeState;
private:

    AudioProcessorValueTreeState::ParameterLayout createParameterLayout() const;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParameterHelper)
};
