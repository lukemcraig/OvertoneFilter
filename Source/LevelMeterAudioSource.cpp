/*
  ==============================================================================

    LevelMeterAudioSource.cpp
    Created: 4 Oct 2019 11:05:10am
    Author:  Luke McDuffie Craig

  ==============================================================================
*/

#include "LevelMeterAudioSource.h"

LevelMeterAudioSource::LevelMeterAudioSource()
{
}

LevelMeterAudioSource::~LevelMeterAudioSource()
{
}

void LevelMeterAudioSource::prepare(const float timeConstant, const float sampleRate)
{
    alpha = std::exp(-1.0f / (timeConstant * sampleRate));
    yPrevious = 0.0f;
    prepared = true;
}

void LevelMeterAudioSource::pushSample(const float sample)
{
    jassert(prepared);
    yPrevious = alpha * yPrevious + (1.0f - alpha) * (sample * sample);
}

float LevelMeterAudioSource::getLevel() const
{
    if (prepared)
        return std::sqrt(yPrevious);
    return 0.0f;
}
