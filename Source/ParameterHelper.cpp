/*
  ==============================================================================

    ParameterHelper.cpp
    Created: 25 Sep 2019 12:18:36pm
    Author:  Luke McDuffie Craig

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
    smoothMix.resize(numChannels);

    smoothInGain.resize(numChannels);
    smoothWetGain.resize(numChannels);
    smoothOutGain.resize(numChannels);

    useInternalWetDry.resize(numChannels);
}

void ParameterHelper::resetSmoothers(const double sampleRate)
{
    for (auto& smoother : smoothStandard)
        smoother.reset(sampleRate, 0.0);
    for (auto& smoother : smoothQ)
        smoother.reset(sampleRate, 0.0);
    for (auto& smoother : smoothMix)
        smoother.reset(sampleRate, 0.1);

    for (auto& smoother : smoothInGain)
        smoother.reset(sampleRate, 0.1);
    for (auto& smoother : smoothWetGain)
        smoother.reset(sampleRate, 0.1);
    for (auto& smoother : smoothOutGain)
        smoother.reset(sampleRate, 0.1);
}

void ParameterHelper::instantlyUpdateSmoothers()
{
    for (auto& smoother : smoothStandard)
        smoother.setCurrentAndTargetValue(*valueTreeState.getRawParameterValue(pidPitchStandard));
    for (auto& smoother : smoothQ)
        smoother.setCurrentAndTargetValue(*valueTreeState.getRawParameterValue(pidQ));
    for (auto& smoother : smoothMix)
        smoother.setCurrentAndTargetValue(*valueTreeState.getRawParameterValue(pidMix));

    for (auto& smoother : smoothInGain)
        smoother.setCurrentAndTargetValue(*valueTreeState.getRawParameterValue(pidInputGain));
    for (auto& smoother : smoothWetGain)
        smoother.setCurrentAndTargetValue(*valueTreeState.getRawParameterValue(pidWetGain));
    for (auto& smoother : smoothOutGain)
        smoother.setCurrentAndTargetValue(*valueTreeState.getRawParameterValue(pidOutputGain));
}

void ParameterHelper::updateSmoothers()
{
    for (auto& smoother : smoothStandard)
        smoother.setTargetValue(*valueTreeState.getRawParameterValue(pidPitchStandard));
    for (auto& smoother : smoothQ)
        smoother.setTargetValue(*valueTreeState.getRawParameterValue(pidQ));

    for (auto& smoother : smoothInGain)
        smoother.setTargetValue(*valueTreeState.getRawParameterValue(pidInputGain));
    for (auto& smoother : smoothWetGain)
        smoother.setTargetValue(*valueTreeState.getRawParameterValue(pidWetGain));
    for (auto& smoother : smoothOutGain)
        smoother.setTargetValue(*valueTreeState.getRawParameterValue(pidOutputGain));

    for (auto i = 0; i < smoothMix.size(); ++i)
    {
        if (!useInternalWetDry[i])
            smoothMix[i].setTargetValue(*valueTreeState.getRawParameterValue(pidMix));
    }
}

float ParameterHelper::getCurrentPitchStandard(int channel)
{
    return smoothStandard[channel].getCurrentValue();
}

void ParameterHelper::skipPitchStandard(int channel, int numSamples)
{
    smoothStandard[channel].skip(numSamples);
}

float ParameterHelper::getQ(const int channel)
{
    return smoothQ[channel].getNextValue();
}

float ParameterHelper::getInputGain(int channel)
{
    return smoothInGain[channel].getNextValue();
}

float ParameterHelper::getOutGain(int channel)
{
     return smoothOutGain[channel].getNextValue();
}

float ParameterHelper::getWetGain(const int channel)
{
    return smoothWetGain[channel].getNextValue();
}

float ParameterHelper::getMix(const int channel)
{
    return smoothMix[channel].getNextValue();
}

void ParameterHelper::setMixTarget(const int channel, const float target)
{
    smoothMix[channel].setTargetValue(target);
}

float ParameterHelper::getCurrentMix(const int channel)
{
    return smoothMix[channel].getCurrentValue();
}

void ParameterHelper::setCurrentMix(const int channel, const float currentWetDry)
{
    smoothMix[channel].setCurrentAndTargetValue(currentWetDry);
}

float ParameterHelper::getCurrentWetGain(const int channel)
{
    return smoothWetGain[channel].getCurrentValue();
}

void ParameterHelper::setCurrentWetGain(const int channel, const float currentGain)
{
    smoothWetGain[channel].setCurrentAndTargetValue(currentGain);
}

void ParameterHelper::useNoteOffWetDry(const int channel)
{
    useInternalWetDry[channel] = true;
    setMixTarget(channel, 0.0f);
}

void ParameterHelper::useParamWetDry(const int channel)
{
    useInternalWetDry[channel] = false;
    setMixTarget(channel, *valueTreeState.getRawParameterValue(pidMix));
}

//==============================================================================
AudioProcessorValueTreeState::ParameterLayout ParameterHelper::createParameterLayout() const
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;

    params.push_back(std::make_unique<AudioParameterFloat>(pidQ,
                                                           "Resonance",
                                                           NormalisableRange<float>(0.1f, 0.95f, 0, 1.0f),
                                                           0.85f));
    params.push_back(std::make_unique<AudioParameterFloat>(pidMix,
                                                           "Mix",
                                                           NormalisableRange<float>(0.0f, 1.0f, 0, 1.0f),
                                                           0.5f));
    params.push_back(std::make_unique<AudioParameterFloat>(pidInputGain,
                                                           "Dry Gain",
                                                           NormalisableRange<float>(0.0f, 2.0f, 0, 1.0f),
                                                           1.0f));
    params.push_back(std::make_unique<AudioParameterFloat>(pidWetGain,
                                                           "Wet Gain",
                                                           NormalisableRange<float>(0.0f, 2.0f, 0, 1.0f),
                                                           0.75f));
    params.push_back(std::make_unique<AudioParameterFloat>(pidOutputGain,
                                                           "Out Gain",
                                                           NormalisableRange<float>(0.0f, 2.0f, 0, 1.0f),
                                                           1.0f));
    params.push_back(std::make_unique<AudioParameterFloat>(pidPitchStandard,
                                                           "Pitch Standard",
                                                           NormalisableRange<float>(392.0f, 493.88f, 0, 1.0f),
                                                           440.0f));
    return {params.begin(), params.end()};
}
