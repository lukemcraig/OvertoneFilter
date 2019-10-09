/*
  ==============================================================================

    SpectrumDisplay.h
    Created: 9 Oct 2019 12:19:27am
    Author:  Luke

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class SpectrumDisplay    : public Component
{
public:
    SpectrumDisplay();
    ~SpectrumDisplay();

    void paint (Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SpectrumDisplay)
};
