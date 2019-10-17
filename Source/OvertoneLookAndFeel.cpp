/*
  ==============================================================================

    OvertoneLookAndFeel.cpp
    Created: 17 Oct 2019 4:51:27pm
    Author:  Luke

  ==============================================================================
*/

#include "OvertoneLookAndFeel.h"

OvertoneLookAndFeel::OvertoneLookAndFeel()
{
}

OvertoneLookAndFeel::~OvertoneLookAndFeel()
{
}

void OvertoneLookAndFeel::drawRotarySlider(Graphics& g, const int x, const int y, const int width, const int height,
                                           const float sliderPosProportional, const float rotaryStartAngle,
                                           const float rotaryEndAngle, Slider& slider)
{
    // most of this is copied from the overridden library code
    const auto outline = slider.findColour(Slider::rotarySliderOutlineColourId);
    const auto fill = slider.findColour(Slider::rotarySliderFillColourId);

    const auto bounds = Rectangle<int>(x, y, width, height).toFloat().reduced(10);

    const auto radius = jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
    const auto toAngle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);
    const auto lineW = jmin(8.0f, radius * 0.5f);
    // changed this
    const auto valueLineW = jmin(valueTrackWidthRotary, radius * 0.5f);
    const auto arcRadius = radius - lineW * 0.5f;

    {
        Path backgroundArc;
        backgroundArc.addCentredArc(bounds.getCentreX(),
                                    bounds.getCentreY(),
                                    arcRadius,
                                    arcRadius,
                                    0.0f,
                                    rotaryStartAngle,
                                    rotaryEndAngle,
                                    true);

        g.setColour(outline);
        g.strokePath(backgroundArc, PathStrokeType(lineW, PathStrokeType::curved, PathStrokeType::rounded));
        g.setColour(outline.brighter(trackDarkenAmount));
        g.strokePath(backgroundArc,
                     PathStrokeType(valueLineW * innerTrackScale, PathStrokeType::curved, PathStrokeType::rounded));
    }
    if (slider.isEnabled())
    {
        Path valueArc;
        valueArc.addCentredArc(bounds.getCentreX(),
                               bounds.getCentreY(),
                               arcRadius,
                               arcRadius,
                               0.0f,
                               rotaryStartAngle,
                               toAngle,
                               true);

        g.setColour(fill);
        g.strokePath(valueArc, PathStrokeType(valueLineW, PathStrokeType::curved, PathStrokeType::rounded));
    }

    const Point<float> thumbPoint(bounds.getCentreX() + arcRadius * std::cos(toAngle - MathConstants<float>::halfPi),
                                  bounds.getCentreY() + arcRadius * std::sin(toAngle - MathConstants<float>::halfPi));

    const auto thumbWidth = lineW * 2.0f;
    // changed this
    drawThumbKnob(g, slider, outline, valueLineW, thumbPoint, thumbWidth);
}

void OvertoneLookAndFeel::drawThumbKnob(Graphics& g, Slider& slider, const Colour outline, const float valueLineW,
                                        const Point<float> thumbPoint, const float thumbWidth)
{
    const auto thumbArea = Rectangle<float>(thumbWidth, thumbWidth).withCentre(thumbPoint);
    Path thumbPath;
    thumbPath.addEllipse(thumbArea);

    g.setColour(outline);
    g.fillPath(thumbPath);

    g.setColour(slider.findColour(Slider::thumbColourId));
    g.strokePath(thumbPath, PathStrokeType(valueLineW));

    thumbPath.applyTransform(AffineTransform::scale(0.66f, 0.66f, thumbPoint.x, thumbPoint.y));
    g.fillPath(thumbPath);
}
