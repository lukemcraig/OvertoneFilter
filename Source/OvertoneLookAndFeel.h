/*
  ==============================================================================

    OvertoneLookAndFeel.h
    Created: 17 Oct 2019 4:51:27pm
    Author:  Luke

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================

class OvertoneLookAndFeel final : public LookAndFeel_V4
{
public:
    OvertoneLookAndFeel();

    ~OvertoneLookAndFeel();

    void drawLinearSlider(Graphics&, int x, int y, int width, int height, float sliderPos, float minSliderPos,
                          float maxSliderPos, Slider::SliderStyle, Slider&) override;

    void drawRotarySlider(Graphics&, int x, int y, int width, int height, float sliderPosProportional,
                          float rotaryStartAngle, float rotaryEndAngle, Slider&) override;

private:
    static constexpr float valueTrackWidth{2.0f};
    static constexpr float valueTrackWidthRotary{2.0f};
    static constexpr float trackDarkenAmount{0.33f};
    static constexpr float innerTrackScale{0.5f};
    static constexpr float groupStrokeThickness{1.0f};

    static void drawThumbKnob(Graphics& g, Slider& slider, Colour outline,
                              float valueLineW, Point<float> thumbPoint,
                              float thumbWidth);

    //==============================================================================
    JUCE_LEAK_DETECTOR(OvertoneLookAndFeel)
};
