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
                                                 Orientation orientation) : MidiKeyboardComponent(state, orientation),
                                                                            processor(p)
{
}

MyMidiKeyboardComponent::~MyMidiKeyboardComponent()
{
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
    processor.handleNoteOff();
}
