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
void ParameterHelper::prepare(const int numChannels)
{
    smoothStandard.resize(numChannels);
    smoothQ.resize(numChannels);
    smoothGain.resize(numChannels);
    smoothWetDry.resize(numChannels);
    useInternalWetDry.resize(numChannels);
}

void ParameterHelper::resetSmoothers(const double sampleRate)
{
    for (auto& smoother : smoothStandard)
        smoother.reset(sampleRate, 0.0);
    for (auto& smoother : smoothQ)
        smoother.reset(sampleRate, 0.0);
    for (auto& smoother : smoothGain)
        smoother.reset(sampleRate, 0.1);
    for (auto& smoother : smoothWetDry)
        smoother.reset(sampleRate, 0.1);
}

void ParameterHelper::instantlyUpdateSmoothers()
{
    for (auto& smoother : smoothStandard)
        smoother.setCurrentAndTargetValue(*valueTreeState.getRawParameterValue(PID_PITCH_STANDARD));
    for (auto& smoother : smoothQ)
        smoother.setCurrentAndTargetValue(*valueTreeState.getRawParameterValue(PID_Q));
    for (auto& smoother : smoothGain)
        smoother.setCurrentAndTargetValue(*valueTreeState.getRawParameterValue(PID_GAIN));
    for (auto& smoother : smoothWetDry)
        smoother.setCurrentAndTargetValue(*valueTreeState.getRawParameterValue(PID_WETDRY));
}

void ParameterHelper::updateSmoothers()
{
    for (auto& smoother : smoothStandard)
        smoother.setTargetValue(*valueTreeState.getRawParameterValue(PID_PITCH_STANDARD));
    for (auto& smoother : smoothQ)
        smoother.setTargetValue(*valueTreeState.getRawParameterValue(PID_Q));
    for (auto& smoother : smoothGain)
        smoother.setTargetValue(*valueTreeState.getRawParameterValue(PID_GAIN));
    for (auto i = 0; i < smoothWetDry.size(); ++i)
    {
        if (!useInternalWetDry[i])
            smoothWetDry[i].setTargetValue(*valueTreeState.getRawParameterValue(PID_WETDRY));
    }
}

float ParameterHelper::getCurrentPitchStandard(int channel)
{
    return smoothStandard[channel].getCurrentValue();
}

void ParameterHelper::skipPitchStandard(int channel,int numSamples)
{
    smoothStandard[channel].skip(numSamples);
}

float ParameterHelper::getQ(const int channel)
{
    return smoothQ[channel].getNextValue();
}

float ParameterHelper::getGain(const int channel)
{
    return smoothGain[channel].getNextValue();
}

float ParameterHelper::getWetDry(const int channel)
{
    return smoothWetDry[channel].getNextValue();
}

void ParameterHelper::setWetDryTarget(const int channel, const float target)
{
    smoothWetDry[channel].setTargetValue(target);
}

float ParameterHelper::getCurrentWetDry(const int channel)
{
    return smoothWetDry[channel].getCurrentValue();
}

void ParameterHelper::setCurrentWetDry(const int channel, const float currentWetDry)
{
    smoothWetDry[channel].setCurrentAndTargetValue(currentWetDry);
}

float ParameterHelper::getCurrentGain(const int channel)
{
    return smoothGain[channel].getCurrentValue();
}

void ParameterHelper::setCurrentGain(const int channel, const float currentGain)
{
    smoothGain[channel].setCurrentAndTargetValue(currentGain);
}

void ParameterHelper::useNoteOffWetDry(const int channel)
{
    useInternalWetDry[channel] = true;
    setWetDryTarget(channel, 0.0f);
}

void ParameterHelper::useParamWetDry(const int channel)
{
    useInternalWetDry[channel] = false;
    setWetDryTarget(channel, *valueTreeState.getRawParameterValue(PID_WETDRY));
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
                                                           90.0f));
    params.push_back(std::make_unique<AudioParameterFloat>(PID_GAIN,
                                                           "Out Gain",
                                                           NormalisableRange<float>(0.0f, 2.0f, 0, 1.0f),
                                                           0.5f));
    params.push_back(std::make_unique<AudioParameterFloat>(PID_PITCH_STANDARD,
                                                           "Pitch Standard",
                                                           NormalisableRange<float>(392.0f, 493.88f, 0, 1.0f),
                                                           440.0f));
    return {params.begin(), params.end()};
}
