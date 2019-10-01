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
    instantlyUpdateSmoothers();
}

ParameterHelper::~ParameterHelper()
{
}

void ParameterHelper::resetSmoothers(double sampleRate)
{
    smoothQ.reset(sampleRate, 0.0);
    smoothGain.reset(sampleRate, 0.0);
    smoothWetDry.reset(sampleRate, 0.0);
}

//==============================================================================
void ParameterHelper::instantlyUpdateSmoothers()
{
    smoothQ.setCurrentAndTargetValue(*valueTreeState.getRawParameterValue(PID_Q));
    smoothGain.setCurrentAndTargetValue(*valueTreeState.getRawParameterValue(PID_GAIN));
    smoothWetDry.setCurrentAndTargetValue(*valueTreeState.getRawParameterValue(PID_WETDRY));
}

void ParameterHelper::updateSmoothers()
{
    smoothQ.setTargetValue(*valueTreeState.getRawParameterValue(PID_Q));
    smoothGain.setTargetValue(*valueTreeState.getRawParameterValue(PID_GAIN));
    if (!useInternalWetDry)
        smoothWetDry.setTargetValue(*valueTreeState.getRawParameterValue(PID_WETDRY));
}

float ParameterHelper::getQ()
{
    return smoothQ.getNextValue();
}

float ParameterHelper::getGain()
{
    return smoothGain.getNextValue();
}

float ParameterHelper::getWetDry()
{
    return smoothWetDry.getNextValue();
}

void ParameterHelper::setWetDryTarget(float target)
{
    smoothWetDry.setTargetValue(target);
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
