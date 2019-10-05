/*
  ==============================================================================

    LevelMeter.h
    Created: 2 Oct 2019 3:52:35pm
    Author:  Luke

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "LevelMeterAudioSource.h"

//==============================================================================
/*
*/
class LevelMeter : public Component,
                   public Timer
{
public:
    LevelMeter(LevelMeterAudioSource&, Colour c = Colours::red, int numLEDs = 5);

    ~LevelMeter();

    void paint(Graphics&) override;

    void resized() override;

    void timerCallback() override;

private:
    LevelMeterAudioSource& levelMeterAudioSource;
    Colour clipColour;
    int numLEDs;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LevelMeter)
};
