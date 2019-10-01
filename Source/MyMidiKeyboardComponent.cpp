/*
  ==============================================================================

    MyMidiKeyboardComponent.cpp
    Created: 1 Oct 2019 4:28:55pm
    Author:  Luke

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "MyMidiKeyboardComponent.h"

//==============================================================================
MyMidiKeyboardComponent::MyMidiKeyboardComponent(MidiWahAudioProcessor& p, MidiKeyboardState& state,
                                                 Orientation orientation) : MidiKeyboardComponent(state, orientation),
                                                                            processor(p)
{
}

MyMidiKeyboardComponent::~MyMidiKeyboardComponent()
{
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
