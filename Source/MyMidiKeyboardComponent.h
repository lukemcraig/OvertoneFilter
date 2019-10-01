/*
  ==============================================================================

    MyMidiKeyboardComponent.h
    Created: 1 Oct 2019 4:28:55pm
    Author:  Luke

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class MyMidiKeyboardComponent : public MidiKeyboardComponent
{
public:
    MyMidiKeyboardComponent(MidiKeyboardState& state,
                            Orientation orientation);

    ~MyMidiKeyboardComponent();

protected:
    bool mouseDownOnKey(int midiNoteNumber, const MouseEvent& e) override;

    void mouseUpOnKey(int midiNoteNumber, const MouseEvent& e) override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MyMidiKeyboardComponent)
};
