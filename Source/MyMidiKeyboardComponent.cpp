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
    //setColour(whiteNoteColourId, Colours::transparentWhite);
    //setColour(blackNoteColourId, Colour(0x88000000));
    parameterHelper.valueTreeState.addParameterListener(parameterHelper.pidPitchStandard, this);
}

MyMidiKeyboardComponent::~MyMidiKeyboardComponent()
{
    parameterHelper.valueTreeState.removeParameterListener(parameterHelper.pidPitchStandard, this);
}

void MyMidiKeyboardComponent::parameterChanged(const String& parameterID, float newValue)
{
    if (parameterID == parameterHelper.pidPitchStandard)
    {
        repaint();
    }
}

//==============================================================================
bool MyMidiKeyboardComponent::mouseDraggedToKey(int midiNoteNumber, const MouseEvent& e)
{
    processor.handleNoteOn(static_cast<float>(midiNoteNumber));
    return false;
}

bool MyMidiKeyboardComponent::mouseDownOnKey(int midiNoteNumber, const MouseEvent& e)
{
    processor.handleNoteOn(static_cast<float>(midiNoteNumber));
    return false;
}

void MyMidiKeyboardComponent::mouseUpOnKey(int midiNoteNumber, const MouseEvent& e)
{
    processor.handleNoteOff(static_cast<float>(midiNoteNumber));
}

//String MyMidiKeyboardComponent::getWhiteNoteText(int midiNoteNumber)
//{
//    const auto freq = 440.0f * std::pow(2.0f, (midiNoteNumber - 69.0f) / 12.0f);
//    return MidiKeyboardComponent::getWhiteNoteText(midiNoteNumber) +"\n"+ String(freq) + " Hz";
//}

void MyMidiKeyboardComponent::drawWhiteNote(int midiNoteNumber, Graphics& g, Rectangle<float> area,
                                            bool isDown, bool isOver, Colour lineColour, Colour textColour)
{
    auto c = Colours::transparentWhite;

    if (isDown) c = findColour(keyDownOverlayColourId);
    if (isOver) c = c.overlaidWith(findColour(mouseOverKeyOverlayColourId));

    g.setColour(c);
    g.fillRect(area);

    auto text = getWhiteNoteText(midiNoteNumber);

    const auto freq = parameterHelper.getCurrentPitchStandard(0) * std::pow(2.0f, (midiNoteNumber - 69.0f) / 12.0f);
    auto freqText = String(freq);
    //text += NewLine::getDefault() + freqText;
    if (text.isNotEmpty())
    {
        auto fontHeight = jmin(12.0f, getKeyWidth() * 0.9f);

        g.setColour(textColour);
        g.setFont(Font(fontHeight).withHorizontalScale(0.5f));

        switch (getOrientation())
        {
        case horizontalKeyboard: g.drawText(freqText, area.withTrimmedLeft(1.0f).withTrimmedBottom(2.0f),
                                            Justification::centredBottom, false);
            /*g.drawText(freqText, area.withTrimmedLeft(1.0f).withTrimmedBottom(2.0f),
                                            Justification::centredBottom, false);*/
            break;
        case verticalKeyboardFacingLeft: g.drawText(text, area.reduced(2.0f), Justification::centredLeft, false);
            break;
        case verticalKeyboardFacingRight: g.drawText(text, area.reduced(2.0f), Justification::centredRight, false);
            break;
        default: break;
        }
    }

    if (! lineColour.isTransparent())
    {
        g.setColour(lineColour);

        switch (getOrientation())
        {
        case horizontalKeyboard: g.fillRect(area.withWidth(1.0f));
            break;
        case verticalKeyboardFacingLeft: g.fillRect(area.withHeight(1.0f));
            break;
        case verticalKeyboardFacingRight: g.fillRect(area.removeFromBottom(1.0f));
            break;
        default: break;
        }

        if (midiNoteNumber == getRangeEnd())
        {
            switch (getOrientation())
            {
            case horizontalKeyboard: g.fillRect(area.expanded(1.0f, 0).removeFromRight(1.0f));
                break;
            case verticalKeyboardFacingLeft: g.fillRect(area.expanded(0, 1.0f).removeFromBottom(1.0f));
                break;
            case verticalKeyboardFacingRight: g.fillRect(area.expanded(0, 1.0f).removeFromTop(1.0f));
                break;
            default: break;
            }
        }
    }
}
