/*
  ==============================================================================

    MyMidiKeyboardComponent.cpp
    Created: 1 Oct 2019 4:28:55pm
    Author:  Luke McDuffie Craig

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "MyMidiKeyboardComponent.h"

//==============================================================================
MyMidiKeyboardComponent::MyMidiKeyboardComponent(OvertoneFilterAudioProcessor& p, MidiKeyboardState& state,
                                                 Orientation orientation, ParameterHelper& ph) :
    MidiKeyboardComponent(state, orientation),
    processor(p), parameterHelper(ph)
{
    setColour(whiteNoteColourId, Colours::white);
    setColour(blackNoteColourId, Colour(0xffB28859));
    setColour(mouseOverKeyOverlayColourId, Colours::grey);
    setColour(keyDownOverlayColourId, Colour(0xff5983B2));
    setColour(shadowColourId, Colours::transparentWhite);
    parameterHelper.valueTreeState.addParameterListener(parameterHelper.pidPitchStandard, this);
}

MyMidiKeyboardComponent::~MyMidiKeyboardComponent()
{
    parameterHelper.valueTreeState.removeParameterListener(parameterHelper.pidPitchStandard, this);
}

void MyMidiKeyboardComponent::paint(Graphics& g)
{
    MidiKeyboardComponent::paint(g);
    g.setColour(Colour(0xff353279));
    g.drawRoundedRectangle(getLocalBounds().toFloat(), 5.0f, 5.0f);
}

void MyMidiKeyboardComponent::parameterChanged(const String& parameterID, float newValue)
{
    if (parameterID == parameterHelper.pidPitchStandard)
    {
        needToRepaint = true;
    }
}

void MyMidiKeyboardComponent::timerCallback()
{
    MidiKeyboardComponent::timerCallback();
    if (needToRepaint)
    {
        needToRepaint = false;
        repaint();
    }
}

void MyMidiKeyboardComponent::mouseUp(const MouseEvent& e)
{
    MidiKeyboardComponent::mouseUp(e);
    if (currentNoteDown != -1)
    {
        processor.handleNoteOff(static_cast<float>(currentNoteDown));
    }
    currentNoteDown = -1;
}

//==============================================================================
bool MyMidiKeyboardComponent::mouseDraggedToKey(int midiNoteNumber, const MouseEvent& e)
{
    return false;
}

bool MyMidiKeyboardComponent::mouseDownOnKey(int midiNoteNumber, const MouseEvent& e)
{
    currentNoteDown = midiNoteNumber;
    processor.handleNoteOn(static_cast<float>(midiNoteNumber));
    return false;
}

void MyMidiKeyboardComponent::drawWhiteNote(int midiNoteNumber, Graphics& g, const Rectangle<float> area,
                                            bool isDown, bool isOver, Colour lineColour, Colour textColour)
{
    auto c = Colours::transparentWhite;

    if (isDown) c = findColour(keyDownOverlayColourId);
    if (isOver) c = c.overlaidWith(findColour(mouseOverKeyOverlayColourId));

    g.setColour(c);
    g.fillRect(area);

    const auto text = getWhiteNoteText(midiNoteNumber);

    if (text.isNotEmpty())
    {
        const auto freq = parameterHelper.getCurrentPitchStandard(0) * std::pow(2.0f, (midiNoteNumber - 69.0f) / 12.0f);
        const auto freqText = String(freq, 1) + ((midiNoteNumber==0) ? " Hz" : "");

        const auto fontHeight = getKeyWidth();
        const Font freqFont(fontHeight * .7f);
        auto area2 = area;
        const auto areaToDraw = area2.removeFromBottom(freqFont.getStringWidth(freqText) +6.0f).withTrimmedBottom(6.0f);

        g.setColour(textColour);
        g.setFont(freqFont);

        const auto rotation = AffineTransform::rotation(-MathConstants<float>::halfPi, areaToDraw.getCentreX(),
                                                        areaToDraw.getCentreY());
        g.addTransform(rotation);

        g.drawText(freqText, areaToDraw.transformedBy(rotation), Justification::left, false);
        g.addTransform(rotation.inverted());
    }

    if (! lineColour.isTransparent())
    {
        g.setColour(lineColour);
        g.fillRect(area.withWidth(1.0f));

        if (midiNoteNumber == getRangeEnd())
        {
            g.fillRect(area.expanded(1.0f, 0).removeFromRight(1.0f));
        }
    }
}

void MyMidiKeyboardComponent::drawBlackNote(int midiNoteNumber, Graphics& g, Rectangle<float> area, bool isDown,
                                            bool isOver, Colour noteFillColour)
{
    auto c = noteFillColour;

    if (isDown) c = c.overlaidWith(findColour(keyDownOverlayColourId));
    if (isOver) c = c.overlaidWith(findColour(mouseOverKeyOverlayColourId));

    g.setColour(c);
    g.fillRect(area);
    g.setColour(getLookAndFeel().findColour(keySeparatorLineColourId));
    g.drawRect(area);
    area.reduce(2, 2);
    g.drawRect(area);
    area.reduce(2, 2);
    g.drawRect(area);
    area.reduce(2, 2);
    g.drawRect(area);
    area.reduce(2, 2);
    g.drawRect(area);
    area.reduce(2, 2);
    g.drawRect(area);
}
