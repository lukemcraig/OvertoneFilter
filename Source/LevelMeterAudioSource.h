/*
  ==============================================================================

    LevelMeterAudioSource.h
    Created: 4 Oct 2019 11:05:10am
    Author:  Luke

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

class LevelMeterAudioSource
{
public:
    LevelMeterAudioSource();

    ~LevelMeterAudioSource();

    //==============================================================================
    void prepare(float timeConstant, float sampleRate);

    void pushSample(float sample);

    float getLevel() const { return std::sqrt(yPrevious); }

private:
    float alpha{};
    float yPrevious{};
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LevelMeterAudioSource)
};
