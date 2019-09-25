/*
  ==============================================================================

    ParameterHelper.cpp
    Created: 25 Sep 2019 12:18:36pm
    Author:  Luke

  ==============================================================================
*/

#include "ParameterHelper.h"

ParameterHelper::ParameterHelper(AudioProcessor& processorToConnectTo): valueTreeState(
    processorToConnectTo, nullptr, "MidiWahParameters", createParameterLayout())
{
}

ParameterHelper::~ParameterHelper()
{
}

//==============================================================================
AudioProcessorValueTreeState::ParameterLayout ParameterHelper::createParameterLayout() const
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;

    params.push_back(std::make_unique<AudioParameterFloat>(PID_CENTERFREQ,
                                                           "Wah Center Frequency",
                                                           NormalisableRange<float>(20.0f, 19000.0f, 0, 0.5f),
                                                           600.0f,
                                                           "Hz"));
    params.push_back(std::make_unique<AudioParameterFloat>(PID_Q,
                                                           "Wah Q",
                                                           NormalisableRange<float>(0.1f, 1.0f, 0, 1.0f),
                                                           0.5f));
    params.push_back(std::make_unique<AudioParameterFloat>(PID_GAIN,
                                                           "Makeup Gain",
                                                           NormalisableRange<float>(0.0f, 2.0f, 0, 1.0f),
                                                           1.0f));
    params.push_back(std::make_unique<AudioParameterFloat>(PID_DRIVE,
                                                           "Drive",
                                                           NormalisableRange<float>(1.0f, 10.0f, 0, 1.0f),
                                                           1.0f));
    return {params.begin(), params.end()};
}
