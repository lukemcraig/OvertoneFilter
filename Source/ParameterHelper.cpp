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

    params.push_back(std::make_unique<AudioParameterFloat>(PID_Q,
                                                           "Resonance",
                                                           NormalisableRange<float>(0.1f, 1.0f, 0, 1.0f),
                                                           0.9f));
    params.push_back(std::make_unique<AudioParameterFloat>(PID_WETDRY,
                                                           "Wet Dry",
                                                           NormalisableRange<float>(0.0f, 1.0f, 0, 1.0f),
                                                           100.0f));
    params.push_back(std::make_unique<AudioParameterFloat>(PID_GAIN,
                                                           "Out Gain",
                                                           NormalisableRange<float>(0.0f, 2.0f, 0, 1.0f),
                                                           0.5f));
    return {params.begin(), params.end()};
}
