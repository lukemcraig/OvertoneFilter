/*
  ==============================================================================

    ParameterHelper.cpp
    Created: 25 Sep 2019 12:18:36pm
    Author:  Luke McDuffie Craig

  ==============================================================================
*/

#include "ParameterHelper.h"

ParameterHelper::ParameterHelper(AudioProcessor& processorToConnectTo): valueTreeState(
    processorToConnectTo, nullptr, "OvertoneFilterParameters", createParameterLayout())
{
}

ParameterHelper::~ParameterHelper()
{
}

//==============================================================================
void ParameterHelper::prepare(const int numChannels)
{
    smoothStandard.resize(numChannels);
    smoothResonance.resize(numChannels);
    smoothMix.resize(numChannels);

    smoothInGain.resize(numChannels);
    smoothWetGain.resize(numChannels);
    smoothOutGain.resize(numChannels);

    useInternalMix.resize(numChannels);
}

void ParameterHelper::resetSmoothers(const double sampleRate)
{
    this->sampleRate = sampleRate;

    for (auto& smoother : smoothStandard)
        smoother.reset(sampleRate, 0.0);
    for (auto& smoother : smoothResonance)
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
    for (auto& smoother : smoothResonance)
        smoother.setCurrentAndTargetValue(*valueTreeState.getRawParameterValue(pidResonance));
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
    for (auto& smoother : smoothResonance)
        smoother.setTargetValue(*valueTreeState.getRawParameterValue(pidResonance));

    for (auto& smoother : smoothInGain)
        smoother.setTargetValue(*valueTreeState.getRawParameterValue(pidInputGain));
    for (auto& smoother : smoothWetGain)
        smoother.setTargetValue(*valueTreeState.getRawParameterValue(pidWetGain));
    for (auto& smoother : smoothOutGain)
        smoother.setTargetValue(*valueTreeState.getRawParameterValue(pidOutputGain));

    for (auto i = 0; i < smoothMix.size(); ++i)
    {
        if (!useInternalMix[i])
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

float ParameterHelper::getCurrentResonance(const int channel)
{
    return smoothResonance[channel].getCurrentValue();
}

void ParameterHelper::skipResonance(int channel, const int numSamples)
{
    smoothResonance[channel].skip(numSamples);
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

void ParameterHelper::setMixRampTime(const int channel, float mixRampTime)
{
    jassert(sampleRate>0);
    auto cv = smoothMix[channel].getCurrentValue();
    auto tv = smoothMix[channel].getTargetValue();
    smoothMix[channel].reset(sampleRate, mixRampTime);
    smoothMix[channel].setCurrentAndTargetValue(cv);
    smoothMix[channel].setTargetValue(tv);
}

void ParameterHelper::useNoteOffMix(const int channel)
{
    useInternalMix[channel] = true;
    setMixRampTime(channel, mixReleaseTime);
    setMixTarget(channel, 0.0f);
}

void ParameterHelper::useParamMix(const int channel)
{
    useInternalMix[channel] = false;
    setMixRampTime(channel, mixAttackTime);
    setMixTarget(channel, *valueTreeState.getRawParameterValue(pidMix));
}

float ParameterHelper::getCurrentWetGain(const int channel)
{
    return smoothWetGain[channel].getCurrentValue();
}

void ParameterHelper::setCurrentWetGain(const int channel, const float currentGain)
{
    smoothWetGain[channel].setCurrentAndTargetValue(currentGain);
}

//==============================================================================
AudioProcessorValueTreeState::ParameterLayout ParameterHelper::createParameterLayout() const
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;

    params.push_back(std::make_unique<AudioParameterFloat>(pidResonance,
                                                           "Resonance",
                                                           NormalisableRange<float>(0.1f, 0.95f, 0, 1.0f),
                                                           0.85f,
                                                           String(),
                                                           AudioProcessorParameter::genericParameter,
                                                           [](float value, int maximumStringLength)
                                                           {
                                                               return String(value * 100.0f, 2) + " %";
                                                           }));
    params.push_back(std::make_unique<AudioParameterFloat>(pidMix,
                                                           "Mix",
                                                           NormalisableRange<float>(0.0f, 1.0f, 0, 1.0f),
                                                           0.5f,
                                                           String(),
                                                           AudioProcessorParameter::genericParameter,
                                                           [](float value, int maximumStringLength)
                                                           {
                                                               return String(value * 100.0f, 2) + " %";
                                                           }));
    params.push_back(std::make_unique<AudioParameterFloat>(pidInputGain,
                                                           "Dry Gain",
                                                           NormalisableRange<float>(0.0f, 2.0f, 0, 1.0f),
                                                           1.0f,
                                                           String(),
                                                           AudioProcessorParameter::genericParameter,
                                                           [](float value, int maximumStringLength)
                                                           {
                                                               return String(value * 100.0f, 2) + " %";
                                                           }));
    params.push_back(std::make_unique<AudioParameterFloat>(pidWetGain,
                                                           "Wet Gain",
                                                           NormalisableRange<float>(0.0f, 2.0f, 0, 1.0f),
                                                           0.75f,
                                                           String(),
                                                           AudioProcessorParameter::genericParameter,
                                                           [](float value, int maximumStringLength)
                                                           {
                                                               return String(value * 100.0f, 2) + " %";
                                                           }));
    params.push_back(std::make_unique<AudioParameterFloat>(pidOutputGain,
                                                           "Out Gain",
                                                           NormalisableRange<float>(0.0f, 2.0f, 0, 1.0f),
                                                           1.0f,
                                                           String(),
                                                           AudioProcessorParameter::genericParameter,
                                                           [](float value, int maximumStringLength)
                                                           {
                                                               return String(value * 100.0f, 2) + " %";
                                                           }));
    params.push_back(std::make_unique<AudioParameterFloat>(pidPitchStandard,
                                                           "Pitch Standard",
                                                           NormalisableRange<float>(392.0f, 493.88f, 0, 1.0f),
                                                           440.0f,
                                                           String(),
                                                           AudioProcessorParameter::genericParameter,
                                                           [](float value, int maximumStringLength)
                                                           {
                                                               return String(value, 2) + " Hz";
                                                           }));
    return {params.begin(), params.end()};
}
