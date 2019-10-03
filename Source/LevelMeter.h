/*
  ==============================================================================

    LevelMeter.h
    Created: 2 Oct 2019 3:52:35pm
    Author:  Luke

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class LevelMeter : public Component,
                   public Timer
{
public:
    LevelMeter(float&,int&);

    ~LevelMeter();

    void paint(Graphics&) override;

    void resized() override;

    void timerCallback() override;

private:
    float& level;
    int& rmsWindowLength;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LevelMeter)
};
