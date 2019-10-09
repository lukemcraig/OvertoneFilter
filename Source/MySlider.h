/*
  ==============================================================================

    MySlider.h
    Created: 8 Oct 2019 1:22:11pm
    Author:  Luke

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class MySlider : public Slider
{
public:
    MySlider();

    ~MySlider();

    void mouseDown(const MouseEvent&) override;

    void mouseEnter(const MouseEvent&) override;

    void mouseMove(const MouseEvent&) override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MySlider)
};
