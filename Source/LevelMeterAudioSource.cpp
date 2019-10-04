/*
  ==============================================================================

    LevelMeterAudioSource.cpp
    Created: 4 Oct 2019 11:05:10am
    Author:  Luke

  ==============================================================================
*/

#include "LevelMeterAudioSource.h"

LevelMeterAudioSource::LevelMeterAudioSource()
{
}

LevelMeterAudioSource::~LevelMeterAudioSource()
{
}

void LevelMeterAudioSource::prepare(float timeConstant, float sampleRate)
{
    alpha = std::exp(-1.0f / (timeConstant * sampleRate));
    yPrevious = 0.0f;
    prepared = true;
}

void LevelMeterAudioSource::pushSample(float sample)
{
    yPrevious = alpha * yPrevious + (1.0f - alpha) * sample;
}

float LevelMeterAudioSource::getLevel() const
{
    jassert(prepared);
    return std::sqrt(yPrevious);
}
