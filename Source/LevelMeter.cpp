/*
  ==============================================================================

    LevelMeter.cpp
    Created: 2 Oct 2019 3:52:35pm
    Author:  Luke

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "LevelMeter.h"

//==============================================================================
LevelMeter::LevelMeter(LevelMeterAudioSource& lmas, Colour c, int numLEDs) : levelMeterAudioSource(lmas), clipColour(c), numLEDs(numLEDs)
{
    startTimer(50);
}

LevelMeter::~LevelMeter()
{
}

void LevelMeter::paint(Graphics& g)
{
    //adapted from LookAndFeel_V4::drawLevelMeter()
    //getLookAndFeel().drawLevelMeter(g,getWidth(),getHeight(),1.0f);

    const auto width = getWidth();
    const auto height = getHeight();

    const auto outerCornerSize = 3.0f;
    const auto outerBorderWidth = 2.0f;
    const auto spacingFraction = 0.03f;

    g.setColour(findColour(ResizableWindow::backgroundColourId));
    g.fillRoundedRectangle(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height), outerCornerSize);

    const auto doubleOuterBorderWidth = 2.0f * outerBorderWidth;

    const auto rms = 1.4125375446227544f * levelMeterAudioSource.getLevel();
    const auto db = Decibels::gainToDecibels(rms);
    const auto noiseFloor = -60.0f;
    const auto a = (numLEDs - 1.0f) / (noiseFloor * noiseFloor);
    const auto dbGated = std::max(0.0f, db - noiseFloor);

    const auto numBlocks = static_cast<int>(std::ceil(a * dbGated * dbGated));

    const auto blockWidth = width - doubleOuterBorderWidth;
    const auto blockHeight = (height - doubleOuterBorderWidth) / static_cast<float>(numLEDs); 
    const auto blockRectHeight = (1.0f - 2.0f * spacingFraction) * blockHeight;
    const auto blockRectSpacing = spacingFraction * blockHeight;

    const auto blockCornerSize = 0.1f * blockWidth;

    const auto c = findColour(Slider::thumbColourId);

    for (auto i = 0; i < numLEDs; ++i)
    {
        if (i >= numBlocks)
            g.setColour(c.withAlpha(0.5f));
        else
            g.setColour(i < numLEDs - 1 ? c : clipColour);

        g.fillRoundedRectangle(outerBorderWidth,
                               outerBorderWidth + ((numLEDs - i - 1) * blockHeight) + blockRectSpacing,
                               blockWidth,
                               blockRectHeight,
                               blockCornerSize);
    }
}

void LevelMeter::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
}

void LevelMeter::timerCallback()
{
    repaint();
}
